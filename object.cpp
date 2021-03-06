#include "object.h"
#include "model.h"

#include "sdk/amxxmodule.h"

#include "euler.h"

//fix bad bullet header file
#undef SIMD_2_PI
#define SIMD_2_PI			(btScalar(2.0) * SIMD_PI)

bmodObject::bmodObject(const char * model, float mass) {
	if(!getModelShape(model, &shape)) {
		MF_Log("BAD model! (%s)", model);
		shape = new btEmptyShape();
	}

	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	bmodMotionState* objectMotionState = new bmodMotionState(this);
	btRigidBody::btRigidBodyConstructionInfo objectRigidBodyCI(mass, objectMotionState, shape, inertia);
	rigidBody = new btRigidBody(objectRigidBodyCI);

	g_bt_dynamicsWorld->addRigidBody(rigidBody);
}



std::list<int> * bmodObject::getEntities() {
	return &entities;
}

int bmodObject::assignEntity(int entity) {
	entities.push_back(entity);
	return entities.size();
}

int bmodObject::removeEntity(int entity) {
	entities.remove(entity);
	return entities.size();
}

void bmodObject::setMass(float mass) {
	g_bt_dynamicsWorld->removeRigidBody(rigidBody);

	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	rigidBody->setMassProps(mass, inertia);
	rigidBody->updateInertiaTensor();
	rigidBody->activate();

	g_bt_dynamicsWorld->addRigidBody(rigidBody);
}

btRigidBody * bmodObject::getRigidBody() {
	return rigidBody;
}

/*
copy entity's position&stuff to rigidbody
*/
void bmodObject::update() {
	if(entities.size() == 0)
		return;

	//TODO: use getWorldTransform instead!
	edict_t * entity = INDEXENT(entities.front());

	btTransform worldTrans(btQuaternion(0, 0, 0, 1), btVector3(entity->v.origin.x, entity->v.origin.y, entity->v.origin.z));
	btVector3 angles;
	entity->v.angles.CopyToArray(angles.m_floats);
	EulerMatrix(angles, worldTrans.getBasis());

	rigidBody->setWorldTransform(worldTrans);

	rigidBody->setLinearVelocity(btVector3(entity->v.velocity.x, entity->v.velocity.y, entity->v.velocity.z));
	rigidBody->setAngularVelocity(btVector3(entity->v.avelocity.z, -entity->v.avelocity.x, entity->v.avelocity.y) * SIMD_RADS_PER_DEG);
}

void bmodObject::registerIndex(int index) {
	rigidBody->setUserPointer((void*)index);
}

bmodObject::~bmodObject() {
	entities.clear();
	g_bt_dynamicsWorld->removeRigidBody(rigidBody);
	delete rigidBody->getMotionState();
	delete rigidBody;
}

//bmod motion state (move callback)

bmodMotionState::bmodMotionState(bmodObject * obj) {
	this->obj = obj;
}

bmodMotionState::~bmodMotionState() {

}

void bmodMotionState::getWorldTransform(btTransform &worldTrans) const {
	if(obj->getEntities()->size() == 0) {
		worldTrans = btTransform(btQuaternion(0, 0, 0, 1));
		return;
	}

	edict_t * entity = INDEXENT(obj->getEntities()->front());

	worldTrans = btTransform(btQuaternion(0, 0, 0, 1), btVector3(entity->v.origin.x, entity->v.origin.y, entity->v.origin.z));
	btVector3 angles;
	entity->v.angles.CopyToArray(angles.m_floats);
	EulerMatrix(angles, worldTrans.getBasis());
}

void bmodMotionState::setWorldTransform(const btTransform &worldTrans) {
	//don't affect immovable objects
	if(obj->getRigidBody()->isStaticOrKinematicObject())
		return;

	Vector origin = Vector((float*)(worldTrans.getOrigin().m_floats));
	
	btVector3 btAngles;
	MatrixEuler(worldTrans.getBasis(), btAngles);
	Vector angles = Vector((float*)btAngles.m_floats);
	
	btRigidBody * body = obj->getRigidBody();
	Vector velocity;
	Vector avelocity;
	if(!body->getDeactivationTime()) {
		velocity = Vector((float*)body->getLinearVelocity().m_floats);
		btVector3 btAvelocity = body->getAngularVelocity();
		avelocity = Vector(-btAvelocity[1], btAvelocity[2], btAvelocity[0]) * SIMD_DEGS_PER_RAD;
	}

	std::list<int> * entities = obj->getEntities();
	for(std::list<int>::iterator it = entities->begin(); it != entities->end(); ++it) {
		edict_t * entity = INDEXENT(*it);
		SET_ORIGIN(entity, origin);
		entity->v.angles = angles;
		entity->v.velocity = velocity;
		entity->v.avelocity = avelocity;
	}
}

#include "object.h"

bmodObject::bmodObject(edict_t * e){
	entity=e;
	shape = new btBoxShape(btVector3(24,24,24));
	bmodMotionState* objectMotionState = new bmodMotionState(
		btTransform(btQuaternion(0,0,0,1), //TODO: get rotation from entity
		btVector3(entity->v.origin.x,entity->v.origin.y,entity->v.origin.z)),
		entity);
	btScalar objectMass = 1;
	btVector3 objectInertia(0,0,0);
	shape->calculateLocalInertia(objectMass,objectInertia);
	btRigidBody::btRigidBodyConstructionInfo objectRigidBodyCI(objectMass,objectMotionState,shape,objectInertia);
	rigidBody = new btRigidBody(objectRigidBodyCI);
	g_bt_dynamicsWorld->addRigidBody(rigidBody);
	objectMotionState->setRigidBody(rigidBody);
}

bmodObject::~bmodObject(){
	g_bt_dynamicsWorld->removeRigidBody(rigidBody);
	delete rigidBody->getMotionState();
	delete rigidBody;
	delete shape;
}

//bmod motion state (move callback)

bmodMotionState::bmodMotionState(const btTransform &initialpos,edict_t * e) {
	bt_initpos=initialpos;
	entity=e;
	bt_body=NULL;
}

bmodMotionState::~bmodMotionState(){

}

void bmodMotionState::getWorldTransform(btTransform &worldTrans) const {
	worldTrans = bt_initpos;
}

void bmodMotionState::setRigidBody(btRigidBody * body){
	bt_body = body;
}

void bmodMotionState::setWorldTransform(const btTransform &worldTrans) {
	btVector3 vec3 = worldTrans.getOrigin();
	//TODO: macro for this vector bullshit
	entity->v.origin.x=vec3.x();
	entity->v.origin.y=vec3.y();
	entity->v.origin.z=vec3.z();
	float rot[3];
	worldTrans.getBasis().getEulerZYX(rot[0],rot[1],rot[2]);
	entity->v.angles.x=-rot[1]*RAD2DEG;	//Pitch (Y)
	entity->v.angles.y=rot[0]*RAD2DEG;	//Yaw (Z)
	entity->v.angles.z=rot[2]*RAD2DEG;	//Roll (X)
	if(bt_body){
		vec3 = bt_body->getLinearVelocity();
		entity->v.velocity.x=vec3.x();
		entity->v.velocity.y=vec3.y();
		entity->v.velocity.z=vec3.z();
		vec3 = bt_body->getAngularVelocity();
		entity->v.avelocity.x=vec3.x();
		entity->v.avelocity.y=vec3.y();
		entity->v.avelocity.z=vec3.z();
	}
}
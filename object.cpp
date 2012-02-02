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
}

/*void bmodObject::Test(int x){
	//test=x;
}*/

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
}

bmodMotionState::~bmodMotionState(){

}

void bmodMotionState::getWorldTransform(btTransform &worldTrans) const {
	worldTrans = bt_initpos;
}

void bmodMotionState::setWorldTransform(const btTransform &worldTrans) {
	btVector3 pos = worldTrans.getOrigin();
	entity->v.origin.x=pos.x();
	entity->v.origin.y=pos.y();
	entity->v.origin.z=pos.z();
	float rot[3];
	worldTrans.getBasis().getEulerZYX(rot[0],rot[1],rot[2]);
	entity->v.angles.x=rot[0]*RAD2DEG;
	entity->v.angles.y=rot[1]*RAD2DEG;
	entity->v.angles.x=rot[2]*RAD2DEG;
//	printf("%f %f %f %f %f %f\n",pos.x(),pos.y(),pos.z(),entity->v.origin.x,entity->v.origin.y,entity->v.origin.z);
}
#include "object.h"

bmodObject::bmodObject(edict_t * e, int type, float mass, float x, float y, float z){
	entity=e;
	//set object shape
	switch(type){
	case 0:
		shape = new btSphereShape(x);
		break;
	case 1:
		shape = new btBoxShape(btVector3(x,y,z));
		break;
	case 2:
		shape = new btCylinderShapeZ(btVector3(x,y,z));
		break;
	case 3:
		shape = new btCapsuleShapeZ(x,y);
		break;
	case 4:
		shape = new btConeShapeZ(x,y);
		break;
	}
		
	bmodMotionState* objectMotionState = new bmodMotionState(entity);
	btVector3 objectInertia(0,0,0);
	shape->calculateLocalInertia(mass,objectInertia);
	btRigidBody::btRigidBodyConstructionInfo objectRigidBodyCI(mass,objectMotionState,shape,objectInertia);
	rigidBody = new btRigidBody(objectRigidBodyCI);
	g_bt_dynamicsWorld->addRigidBody(rigidBody);
	objectMotionState->setRigidBody(rigidBody);
}

edict_t *bmodObject::getEntity(){
	return entity;
}

btRigidBody * bmodObject::getRigidBody(){
	return rigidBody;
}

bmodObject::~bmodObject(){
	g_bt_dynamicsWorld->removeRigidBody(rigidBody);
	delete rigidBody->getMotionState();
	delete rigidBody;
	delete shape;
}

//bmod motion state (move callback)

//#define NEWORIGIN(a) ((entity->v.absmin.a + entity->v.absmax.a) * 0.5)

bmodMotionState::bmodMotionState(edict_t * e) {
	entity=e;
	bt_body=NULL;
	//brushes with origin in 0,0,0 are fucked up anyway >> unsupported
	/*offset[0]=NEWORIGIN(x)-entity->v.origin.x;
	offset[1]=NEWORIGIN(y)-entity->v.origin.y;
	offset[2]=NEWORIGIN(z)-entity->v.origin.z;*/
}

bmodMotionState::~bmodMotionState(){

}

void bmodMotionState::getWorldTransform(btTransform &worldTrans) const {
	worldTrans = btTransform(btQuaternion(0,0,0,1),btVector3(entity->v.origin.x,entity->v.origin.y,entity->v.origin.z));
	worldTrans.getBasis().setEulerZYX(entity->v.angles.y,-entity->v.angles.x,entity->v.angles.z);
}

void bmodMotionState::setRigidBody(btRigidBody * body){
	bt_body = body;
	bt_body->setLinearVelocity(btVector3(entity->v.velocity.x, entity->v.velocity.y, entity->v.velocity.z));
	bt_body->setAngularVelocity(btVector3(entity->v.avelocity.x, entity->v.avelocity.y, entity->v.avelocity.z));
	//btVector3 gravity = g_bt_dynamicsWorld->getGravity();
	//bt_body->setGravity(btVector3(gravity.x()*entity->v.gravity, gravity.y()*entity->v.gravity, gravity.z()*entity->v.gravity));	//relative gravity
}

//TODO: solve this shit with overloading
#define BT2HLVEC(hl,bt) {hl.x=bt.x();\
	hl.y=bt.y();\
	hl.z=bt.z();}

void bmodMotionState::setWorldTransform(const btTransform &worldTrans) {
	btVector3 vec3 = worldTrans.getOrigin();
	BT2HLVEC(entity->v.origin,vec3);
	entity->v.absmin.x=entity->v.origin.x-entity->v.size.x*0.5;
	entity->v.absmin.y=entity->v.origin.y-entity->v.size.y*0.5;
	entity->v.absmin.z=entity->v.origin.z-entity->v.size.z*0.5;
	entity->v.absmax.x=entity->v.origin.x+entity->v.size.x*0.5;
	entity->v.absmax.y=entity->v.origin.y+entity->v.size.y*0.5;
	entity->v.absmax.z=entity->v.origin.z+entity->v.size.z*0.5;
	/*entity->v.origin.x=vec3.x()-offset[0];
	entity->v.origin.y=vec3.y()-offset[1];
	entity->v.origin.z=vec3.z()-offset[2];*/
	float rot[3];
	worldTrans.getBasis().getEulerZYX(rot[0],rot[1],rot[2]);
	entity->v.angles.x=-rot[1]*RAD2DEG;	//Pitch (Y)
	entity->v.angles.y=rot[0]*RAD2DEG;	//Yaw (Z)
	entity->v.angles.z=rot[2]*RAD2DEG;	//Roll (X)
	if(bt_body){
		vec3 = bt_body->getLinearVelocity();
		BT2HLVEC(entity->v.velocity,vec3);
		/*entity->v.velocity.x=vec3.x();
		entity->v.velocity.y=vec3.y();
		entity->v.velocity.z=vec3.z();*/
		vec3 = bt_body->getAngularVelocity();
		BT2HLVEC(entity->v.avelocity,vec3);
		/*entity->v.avelocity.x=vec3.x();
		entity->v.avelocity.y=vec3.y();
		entity->v.avelocity.z=vec3.z();*/
	}
}
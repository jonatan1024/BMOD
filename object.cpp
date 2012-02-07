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

int bmodObject::getIslandTag(){
	return rigidBody->getIslandTag();
}

void bmodObject::activate(){
	rigidBody->activate(true);
}

bmodObject::~bmodObject(){
	g_bt_dynamicsWorld->removeRigidBody(rigidBody);
	delete rigidBody->getMotionState();
	delete rigidBody;
	delete shape;
}

//bmod motion state (move callback)

#define NEWORIGIN(a) ((entity->v.absmin.a + entity->v.absmax.a) * 0.5)

bmodMotionState::bmodMotionState(edict_t * e) {
	entity=e;
	bt_body=NULL;
	//in-fucking-credible, is there any way how to move origin into real center of brush entity?
	offset[0]=NEWORIGIN(x)-entity->v.origin.x;
	offset[1]=NEWORIGIN(y)-entity->v.origin.y;
	offset[2]=NEWORIGIN(z)-entity->v.origin.z;
}

bmodMotionState::~bmodMotionState(){

}

void bmodMotionState::getWorldTransform(btTransform &worldTrans) const {
	//TODO: get rotation from entity; i should also get velocity and avelocity and apply it on created bullet object
	worldTrans = btTransform(btQuaternion(0,0,0,1),
		btVector3(NEWORIGIN(x),
		NEWORIGIN(y),
		NEWORIGIN(z)));
}

void bmodMotionState::setRigidBody(btRigidBody * body){
	bt_body = body;
}

//TODO: solve this shit with overloading
#define BT2HLVEC(hl,bt) {hl.x=bt.x();\
	hl.y=bt.y();\
	hl.z=bt.z();}

void bmodMotionState::setWorldTransform(const btTransform &worldTrans) {
	btVector3 vec3 = worldTrans.getOrigin();
	entity->v.absmin.x=vec3.x()-entity->v.size.x*0.5;
	entity->v.absmin.y=vec3.y()-entity->v.size.y*0.5;
	entity->v.absmin.z=vec3.z()-entity->v.size.z*0.5;
	entity->v.absmax.x=vec3.x()+entity->v.size.x*0.5;
	entity->v.absmax.y=vec3.y()+entity->v.size.y*0.5;
	entity->v.absmax.z=vec3.z()+entity->v.size.z*0.5;
	entity->v.origin.x=vec3.x()-offset[0];
	entity->v.origin.y=vec3.y()-offset[1];
	entity->v.origin.z=vec3.z()-offset[2];
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
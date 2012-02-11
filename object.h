#ifndef __BMOD_OBJECT__
#define __BMOD_OBJECT__

#include <stdio.h>
#include <extdll.h>//TODO: is it really nessesary?
#include <btBulletDynamicsCommon.h>

extern btDiscreteDynamicsWorld* g_bt_dynamicsWorld;

#define RAD2DEG 57.2957795

class bmodObject{
private:
	btRigidBody* rigidBody;
	btCollisionShape* shape;
	edict_t * entity;
public:
	bmodObject(edict_t * e, int type, float mass, float x, float y, float z);
	edict_t * getEntity();
	int getIslandTag();
	void activate();
	btRigidBody* getRigidBody();
	~bmodObject();
};

class bmodMotionState : public btMotionState {
public:
	bmodMotionState(edict_t * entity);
	virtual ~bmodMotionState();
	virtual void getWorldTransform(btTransform &worldTrans) const;
	virtual void setWorldTransform(const btTransform &worldTrans);
	void setRigidBody(btRigidBody*body);
protected:
	//float offset[3];//origin offset
	edict_t * entity;
	btRigidBody * bt_body;
};
#endif
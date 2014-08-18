#ifndef __BMOD_OBJECT__
#define __BMOD_OBJECT__

#include <stdio.h>
#include <btBulletDynamicsCommon.h>

#include <list>

extern btDiscreteDynamicsWorld* g_bt_dynamicsWorld;

class bmodObject {
private:
	btRigidBody* rigidBody;
	btCollisionShape* shape;
	std::list<int> entities;
public:
	bmodObject(const char * model, float mass);
	std::list<int> * getEntities();
	int assignEntity(int entity);
	int removeEntity(int entity);
	void setMass(float mass);
	btRigidBody* getRigidBody();
	void update();
	void registerIndex(int index);
	~bmodObject();
};

class bmodMotionState : public btMotionState {
public:
	bmodMotionState(bmodObject * obj);
	virtual ~bmodMotionState();
	virtual void getWorldTransform(btTransform &worldTrans) const;
	virtual void setWorldTransform(const btTransform &worldTrans);
protected:
	bmodObject * obj;
};
#endif
#include <stdio.h>
#include <extdll.h>//TODO: is it really nessesary?
#include <btBulletDynamicsCommon.h>

extern btDiscreteDynamicsWorld* g_bt_dynamicsWorld;

#define RAD2DEG 57.2957795

class bmodObject{
private:
	edict_t * entity;
	btRigidBody* rigidBody;
	btCollisionShape* shape;
public:
	bmodObject(edict_t * entity);
	//void Test(int x);
	~bmodObject();
};

class bmodMotionState : public btMotionState {
public:
	bmodMotionState(const btTransform &initialpos,edict_t * entity);
	virtual ~bmodMotionState();
	virtual void getWorldTransform(btTransform &worldTrans) const;
	virtual void setWorldTransform(const btTransform &worldTrans);
protected:
	edict_t * entity;
	btTransform bt_initpos;
};
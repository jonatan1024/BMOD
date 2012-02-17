#ifndef __BMOD_OBJECTLIST__
#define __BMOD_OBJECTLIST__

#include "object.h"
#include <string.h>

class bmodObjectList{
private:
	bmodObject**list;
	int list_c;
public:
	bmodObjectList();
	bool add(bmodObject*);
	bool remove(edict_t*);
	bool activate(int);
	bmodObject* find(edict_t*);
	bmodObject* find_b(btCollisionObject*);
	~bmodObjectList();
};
#endif
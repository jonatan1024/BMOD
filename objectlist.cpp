#include "objectlist.h"

bmodObjectList::bmodObjectList() {
	list_c = 0;
	list = NULL;
}

bool bmodObjectList::add(bmodObject*obj) {
	list_c++;
	if(list = (bmodObject**)realloc(list, sizeof(bmodObject*)*list_c)) {
		list[list_c - 1] = obj;
		return 1;
	}
	return 0;
}

bool bmodObjectList::remove(edict_t*e) {
	for(int i = 0; i < list_c; i++) {
		if(list[i]->getEntity() == e) {
			activate(i);//reactivate that fucking island
			delete list[i];
			list[i] = list[--list_c];	//replace with last object
			return 1;
		}
	}
	return 0;
}

bool bmodObjectList::activate(int obj) {
	for(int i = 0; i < list_c; i++) {
		if(i != obj && list[i]->getRigidBody()->getIslandTag() == list[obj]->getRigidBody()->getIslandTag()) {
			list[i]->getRigidBody()->activate();
			return 1;
		}
	}
	return 0;
}

bmodObject* bmodObjectList::find(edict_t*e) {
	for(int i = 0; i < list_c; i++) {
		if(list[i]->getEntity() == e) {
			return list[i];
		}
	}
	return 0;
}

bmodObject* bmodObjectList::find_b(const btCollisionObject*body) {
	for(int i = 0; i < list_c; i++) {
		if(list[i]->getRigidBody() == body) {
			return list[i];
		}
	}
	return 0;
}

bmodObjectList::~bmodObjectList() {
	for(int i = 0; i < list_c; i++) {
		delete list[i];
	}
	free(list);
}
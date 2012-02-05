#include "objectlist.h"

bmodObjectList::bmodObjectList(){
	list_c=0;
	list=NULL;
}

bool bmodObjectList::add(bmodObject*obj){
	list_c++;
	if(list = (bmodObject**)realloc(list,sizeof(bmodObject*)*list_c)){
		list[list_c-1]=obj;
		return 1;
	}
	return 0;
}

bool bmodObjectList::remove(edict_t*e){
	for(int i=0;i<list_c;i++){
		if(list[i]->entity == e){
			delete list[i];
			list[i]=list[--list_c];	//replace with last object
			return 1;
		}
	}
	return 0;
}

bmodObjectList::~bmodObjectList(){
	for(int i=0;i<list_c;i++){
		delete list[i];
	}
	free(list);
}
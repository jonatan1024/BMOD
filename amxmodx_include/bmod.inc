/*
Creates a bmod object of given type, mass and size. Bmod object is subsequently hooked to given entity.
*/
native bmod_object_add(ent,type,Float:mass,Float:x,Float:y,Float:z)

/*
Types of bmod objects.
*/
enum {
	bmod_sphere = 0,	/* x is radius */
	bmod_box,	/* x, y and z are half of box dimensions */
	bmod_cylinder,	/* same as box, x is radius; cylinder is situated along Z axis */
	bmod_capsule,	/* x is radius, y is distance between half spheres; capsule is situated along Z axis */
	bmod_cone	/* x is radius, y is height; cone is situated along Z axis */
};

/*
Destroys the bmod object hooked to given entity.
*/
native bmod_object_remove(ent)

/*
Set object friction.
*/
native bmod_object_set_friction(ent,Float:friction)

/*
Set object restitution (bounciness)
*/
native bmod_object_set_restitution(ent,Float:restitution)

/*
Set world friction.
*/
native bmod_world_set_friction(Float:friction)

/*
Set world restitution (bounciness)
*/
native bmod_world_set_restitution(Float:restitution)
/* AMXX-Studio Notes - DO NOT MODIFY BELOW HERE
*{\\ rtf1\\ ansi\\ deff0{\\ fonttbl{\\ f0\\ fnil Tahoma;}}\n\\ viewkind4\\ uc1\\ pard\\ lang5129\\ f0\\ fs16 \n\\ par }
*/

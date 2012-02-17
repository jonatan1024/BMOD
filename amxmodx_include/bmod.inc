/*
Creates a bmod object of given type, mass and size (x,y,z). Bmod object is subsequently hooked to given entity.
*/
native bmod_object_add(ent,type,Float:mass,Float:size[3])

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
Return 1 if there is any object hooked to given entity.
*/
native bmod_object_check(ent)

/*
Set object friction.
*/
native bmod_object_set_friction(ent,Float:friction)

/*
Set object restitution (bounciness)
*/
native bmod_object_set_restitution(ent,Float:restitution)

/*
Set object gravity (default gravity is {0,0,-525})
*/
native bmod_object_set_gravity(ent,Float:gravity[3])

/*
Set object linear velocity
*/
native bmod_object_set_velocity(ent,Float:velocity[3])

/*
Set object angular velocity
*/
native bmod_object_set_avelocity(ent,Float:avelocity[3])

/*
Apply object force
*/
native bmod_object_apply_force(ent,Float:force[3])

/*
Apply object torque
*/
native bmod_object_apply_torque(ent,Float:torque[3])

/*
Apply object force at relative position
*/
native bmod_object_apply_force_at(ent,Float:force[3],Float:pos[3])

/*
Get object friction.
*/
native Float:bmod_object_get_friction(ent)

/*
Get object restitution (bounciness)
*/
native Float:bmod_object_get_restitution(ent)

/*
Get object gravity
*/
native bmod_object_get_gravity(ent,Float:gravity[3])

/*
Set world friction.
*/
native bmod_world_set_friction(Float:friction)

/*
Set world restitution (bounciness)
*/
native bmod_world_set_restitution(Float:restitution)

/*
Get world friction.
*/
native Float:bmod_world_get_friction()

/*
Get world restitution (bounciness)
*/
native Float:bmod_world_get_restitution()

/*
Do a traceline, return entity id or zero. Also may return hit point or normal.
*/
native bmod_traceline(Float:start[3],Float:end[3],Float:point[3],Float:normal[3])
/* AMXX-Studio Notes - DO NOT MODIFY BELOW HERE
*{\\ rtf1\\ ansi\\ deff0{\\ fonttbl{\\ f0\\ fnil Tahoma;}}\n\\ viewkind4\\ uc1\\ pard\\ lang5129\\ f0\\ fs16 \n\\ par }
*/

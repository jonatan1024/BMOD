/*
Creates a bmod object of given type, mass and size. Bmod object is subsequently hooked to given entity.
Types:
0 - sphere (x is radius)
1 - box (x, y and z are half of box dimensions)
2 - cylinder (same as box, x is radius; cylinder is situated along Z axis)
3 - capsule (x is radius, y is distance between half spheres; capsule is situated along Z axis)
4 - cone (x is radius, y is height; cone is situated along Z axis)
*/
native bmod_object_add(ent,type,Float:mass,Float:x,Float:y,Float:z)

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


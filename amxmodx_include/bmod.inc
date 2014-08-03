/*
Creates a bmod object from given model and mass. If mass is zero, the object is static.
Returns handle to bmod object. -1 on error.
You can use three types of models:

1, BMOD primitives - boxes, spheres, etc.. Valid examples:
"BMOD/box/16/24/24" - creates a box with half-extents of 16,24,24; so it is going to be 32x48x48 box
"BMOD/capsule/8/32" - creates upright capsule with radius of 8 and height of 32
"BMOD/cone/16/64" - cone, same parameters as capsule
"BMOD/cylinder/32/32/128" - cylinder, same parameters as box (half-extents)
"BMOD/sphere/32" - creates a sphere of radius 32

2, BSP models - solid models on current map, or whole maps:
"*3" - creates an object from solid model no.3
"maps/de_dust2.bsp" - creates an from map de_dust2 (solid model no.0)
"maps/cs_assault.bsp/11" - creates an object from solid model no.11 on map cs_assault

3, MDL models - static studio models:
"models/qtruck.mdl" - creates an object from qtruck.mdl (part 0, submodel 0)
"models/bonegibs.mdl/0/1" - creates an object from bonegibs.mdl, part 0, submodel 1 (when indexing from zero)
*/
native bmod_obj_new(model[], Float:mass = 0.0)

/*
Deletes bmod object given by parameter.
Returns true on success.
*/
native bmod_obj_delete(obj)

/*
Creates a bmod object from given entity. It uses entity's model, origin and angles. Mass is zero, so object is static.
Returns handle to bmod object. -1 on error.
*/
native bmod_obj_from_ent(ent)

/*
Assigns an entity to given object. Assigned entites are moved accordingy to object movement.
Returns number of assigned entites. -1 on error.
*/
native bmod_obj_assign_ent(obj, ent)

/*
Removes an entity from given object's list of assigned entites.
Returns number of assigned entites. -1 on error.
*/
native bmod_obj_remove_ent(obj, ent)

/*
Gets all entites that are assigned to given object. Len is length of input array.
Returns number of assigned entites. -1 on error.
*/
native bmod_obj_get_ents(obj, ents[], len)

/*
Finds bmod object by assigned entity.
Returns bmod object. -1 on error.
*/
native bmod_obj_by_ent(ent)

/*
CollisionFlags for get/setCollisionFlags
*/
enum CollisionFlags
{
	CF_STATIC_OBJECT = 1,
	CF_KINEMATIC_OBJECT = 2,
	CF_NO_CONTACT_RESPONSE = 4,
	CF_CUSTOM_MATERIAL_CALLBACK = 8,//this allows per-triangle material (friction/restitution)
	CF_CHARACTER_OBJECT = 16,
	CF_DISABLE_VISUALIZE_OBJECT = 32, //disable debug drawing
	CF_DISABLE_SPU_COL_PROCESSING = 64//disable parallel/SPU processing
};

enum ActivationState
{
	ACTIVE_TAG = 1,
	ISLAND_SLEEPING = 2,
	WANTS_DEACTIVATION = 3,
	DISABLE_DEACTIVATION = 4,
	DISABLE_SIMULATION = 5,
}

/*
Calls a function on given object.
Returns true on succesful call.
You can use following functions:

"activate"

"applyCentralForce"
"applyCentralImpulse"
"applyDamping"
"applyForce"
"applyGravity"
"applyImpulse"
"applyTorque"
"applyTorqueImpulse"

"forceActivationState"

"getActivationState"
"getAngularDamping"
"getAngularFactor"
"getAngularSleepingThreshold"
"getAngularVelocity"
"getCollisionFlags"
"getFlags"
"getFriction"
"getGravity"
"getInvMass"
"getLinearDamping"
"getLinearFactor"
"getLinearSleepingThreshold"
"getLinearVelocity"
"getRestitution"
"getRollingFriction"
"getUserIndex"
"getWorldTransform"

"isActive"
"isKinematicObject"
"isStaticObject"

"setActivationState"
"setAngularFactor"
"setAngularVelocity"
"setCollisionFlags"
"setDamping"
"setFlags"
"setFriction"
"setGravity"
"setLinearFactor"
"setLinearVelocity"
"setRestitution"
"setRollingFriction"
"setSleepingThresholds"
"setUserIndex"
"setWorldTransform"

See http://www.continuousphysics.com/Bullet/BulletFull/classbtRigidBody.html#pub-methods for more information.
*/
native bmod_obj_call(obj, func[], any:...)

/*
Set objects mass. You can use this function to make object static and vice versa.
Returns true on success.
*/
native bmod_obj_set_mass(obj, Float:mass)

/*
Makes object kinematic (or static, when bool_kinematic is false). Kinematic objects move accordingly to assigned entity's movement.
Returns true on success.
*/
native bmod_obj_set_kinematic(obj, bool_kinematic)

/*
Moves objects to assigned entity's position.
Returns true on success.
*/
native bmod_obj_update_pos(obj)

/*
See lower.
*/
enum trimesh_shape_type {
	TST_concave_static,
	TST_concave_gimpact,
	TST_convex,
};

/*
Set shape type, origin and scale for all next objects created.
Types of trimesh:
TST_concave_static - fast standard concave trimesh, can be used for static or kinematic objects.
TST_concave_gimpact - slow concave trimesh, can be used for dynamic objects.
TST_convex - fast convex hull created from trimesh, should be preferably used for dynamic objects instead of concave_gimpact

Origin is just offset of shape, if you need to shift the center of mass.
Scale is self-explainatory.

Returns true on success.
*/
native bmod_shape_cfg(trimesh_shape_type:type, Float:origin[3] = {0.0, 0.0, 0.0}, Float:scale[3] = {1.0, 1.0, 1.0})

/*
Raycast in bmod world.
Input vectors are start and end of the ray.
Output vectors are impact point of the ray and normal of that impact point.
Returns bmod object hit. -1 on error.
*/
native bmod_traceline(Float:start[3], Float:end[3], Float:hitpoint[3], Float:hitnormal[3])


/*
Forward called on objects collision.
*/
forward bmod_forward_contact(obj1, obj2, Float:distance)

/*
Configure bullet's stepSimulation. For more info see:
http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World
http://bulletphysics.com/Bullet/BulletFull/classbtDynamicsWorld.html#a5ab26a0d6e8b2b21fbde2ed8f8dd6294
Default: timeStep=variable, maxSubSteps=6, fixedTimeStep=1/60
*/
native bmod_stepcfg(maxSubSteps, Float:fixedTimeStep)
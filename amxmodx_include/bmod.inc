/*
Types of bmod objects.
*/
enum {
	BMOD_sphere = 0,	/* x is radius */
	BMOD_box,	/* x, y and z are half of box dimensions */
	BMOD_cylinder,	/* same as box, x is radius; cylinder is situated along Z axis */
	BMOD_capsule,	/* x is radius, y is distance between half spheres; capsule is situated along Z axis */
	BMOD_cone	/* x is radius, y is height; cone is situated along Z axis */
};

/*
Creates a bmod object of given type, mass and size (x,y,z). Bmod object is subsequently hooked to given entity.
*/
native bmod_object_add(ent,type,Float:mass,Float:size[3])

/*
Destroys the bmod object hooked to given entity.
*/
native bmod_object_remove(ent)

/*
Return 1 if there is any object hooked to given entity.
*/
native bmod_object_check(ent)

enum BMOD_FL {
	BMOD_FL_adamping,	//set/getAngularDamping
	BMOD_FL_afactor,	//setAngularFactor only
	BMOD_FL_damping,	//set/getLinearDamping
	BMOD_FL_friction,	//set/getFriction
	BMOD_FL_restitution	//set/getRestitution
};

enum BMOD_VEC {
	BMOD_VEC_afactor,	//set/getAngularFactor
	BMOD_VEC_avelocity,	//set/getAngularVelocity
	BMOD_VEC_afriction,	//set/getAnisotropicFriction
	BMOD_VEC_gravity,	//set/getGravity
	BMOD_VEC_factor,	//set/getLinearFactor
	BMOD_VEC_velocity,	//set/getLinearVelocity
	BMOD_VEC_force,	//applyCentralForce / getTotalForce
	BMOD_VEC_torque	//applyTorque / getTotalTorque
};

/*
Set object vars.
*/
native bmod_object_set_float(ent,BMOD_FL:key,Float:float)
native bmod_object_set_vector(ent,BMOD_VEC:key,Float:vector[3])

/*
Apply object force at relative position.
*/
native bmod_object_apply_force_at(ent,Float:force[3],Float:pos[3])

/*
Get object vars.
*/
native Float:bmod_object_get_float(ent,BMOD_FL:key)
native bmod_object_get_vector(ent,BMOD_VEC:key,Float:vector[3])

/*
Activate callback(forward) for given entity.
*/
native bmod_object_set_callback(ent,bool)

/*
Set/get world vars. (friction & restitution only)
*/
native bmod_world_set_float(BMOD_FL:key,Float:float)
native Float:bmod_world_get_float(BMOD_FL:key)

/*
Do a traceline, return entity id or zero. Also may return hit point or normal.
*/
native bmod_traceline(Float:start[3],Float:end[3],Float:point[3],Float:normal[3])

/*
Forward called on objects collision.
*/
forward bmod_forward_contact(ent1,ent2,Float:distance)

/*
Configure bullet's stepSimulation. For more info see:
http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World
http://bulletphysics.com/Bullet/BulletFull/classbtDynamicsWorld.html#a5ab26a0d6e8b2b21fbde2ed8f8dd6294
Default: timeStep=variable, maxSubSteps=6, fixedTimeStep=1/60
*/
native bmod_stepcfg(maxSubSteps,Float:fixedTimeStep)

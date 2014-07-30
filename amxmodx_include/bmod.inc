native bmod_obj_new(model[], Float:mass = 0.0)
native bmod_obj_delete(obj)
native bmod_obj_from_ent(ent)
native bmod_obj_assign_ent(obj, ent)
native bmod_obj_remove_ent(obj, ent)
native bmod_obj_get_ents(obj)
native bmod_obj_by_ent(ent)
native bmod_obj_call(obj, func[], ...)
native bmod_obj_set_mass(obj, Float:mass)
native bmod_obj_set_kinematic(obj, bool_kinematic)
native bmod_obj_update_pos(obj)

enum trimesh_shape_type {
	TST_concave_static,
	TST_concave_gimpact,
	TST_convex,
	TST_NUM_TYPES,
};

native bmod_shape_cfg(trimesh_shape_type, Float:origin[3] = {0.0, 0.0, 0.0}, Float:scale[3] = {1.0, 1.0, 1.0})
native bmod_traceline()


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
#include <btBulletDynamicsCommon.h>

enum trimesh_shape_type {
	TST_concave_static,
	TST_concave_gimpact,
	TST_convex,
	TST_NUM_TYPES,
};

bool getModelShape(const char * model, btCollisionShape** shape);
void clearModels();
bool setModelConfig(trimesh_shape_type tst, float origin[3], float scale[3]);
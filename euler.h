#include <btBulletDynamicsCommon.h>

void EulerMatrix(const btVector3& euler, btMatrix3x3& matrix);
void MatrixEuler(const btMatrix3x3& matrix, btVector3& euler);
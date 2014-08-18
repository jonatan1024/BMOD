#include "euler.h"

#include <stdio.h>
#include "sdk/amxxmodule.h"

//oh my god you must be fucking kidding me!
//previous definition, as it is in btScalar.h:
//#define SIMD_2_PI         btScalar(2.0) * SIMD_PI
//guess what's wrong :)
#undef SIMD_2_PI
#define SIMD_2_PI			(btScalar(2.0) * SIMD_PI)

void EulerMatrix(const btVector3& in_euler, btMatrix3x3& out_matrix) {
	btVector3 angles = in_euler;
	angles *= SIMD_RADS_PER_DEG;

	btScalar c1(btCos(angles[0]));
	btScalar c2(btCos(angles[1]));
	btScalar c3(btCos(angles[2]));
	btScalar s1(btSin(angles[0]));
	btScalar s2(btSin(angles[1]));
	btScalar s3(btSin(angles[2]));

	out_matrix.setValue(	c1 * c2,	- c3 * s2 - s1 * s3 * c2,		s3 * s2 - s1 * c3 * c2,
							c1 * s2,	c3 * c2 - s1 * s3 * s2,			- s3 * c2 - s1 * c3 * s2,
							s1,			c1 * s3,						c1 * c3);
}

void MatrixEuler(const btMatrix3x3& in_matrix, btVector3& out_euler) {
	out_euler[0] = btAsin(in_matrix[2][0]);

	if(btFabs(in_matrix[2][0]) < (1 - 0.001f)) {
		out_euler[1] = btAtan2(in_matrix[1][0], in_matrix[0][0]);
		out_euler[2] = btAtan2(in_matrix[2][1], in_matrix[2][2]);
	}
	else {
		out_euler[1] = btAtan2(in_matrix[1][2], in_matrix[1][1]);
		out_euler[2] = 0;
	}

	out_euler *= SIMD_DEGS_PER_RAD;
}
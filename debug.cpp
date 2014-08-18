#include "sdk/amxxmodule.h"
#include <btBulletDynamicsCommon.h>

extern btDiscreteDynamicsWorld* g_bt_dynamicsWorld;
extern char g_game_dir[];

//fix bad bullet header file
#undef SIMD_2_PI
#define SIMD_2_PI			(btScalar(2.0) * SIMD_PI)

const char * get_avail_file(char * buffer, const char * ext) {
	for(int i = 0; i < 10000; i++) {
		sprintf(buffer, "%s/%s%04d%s", g_game_dir, STRING(gpGlobals->mapname), i, ext);
		FILE * tmpfile = fopen(buffer, "r");
		if(!tmpfile)
			return buffer;
		fclose(tmpfile);
	}
	return NULL;
}

void bmod_serialize() {
	const char * filename;
	if(CMD_ARGC() >= 2) {
		filename = CMD_ARGS();
	}
	else {
		char tmpname[MAX_PATH];
		filename = get_avail_file(tmpname, ".bullet");
	}
	FILE * file = fopen(filename, "wb");
	if(!file) {
		MF_PrintSrvConsole("Couldn't open file '%s' for writing\n", filename);
		return;
	}

	btDefaultSerializer* serializer = new btDefaultSerializer();
	g_bt_dynamicsWorld->serialize(serializer);
	fwrite(serializer->getBufferPointer(), serializer->getCurrentBufferSize(), 1, file);
	fclose(file);

	MF_PrintSrvConsole("Wrote %d bytes into output file '%s'\n", serializer->getCurrentBufferSize(), filename);
	delete serializer;
}

void bmod_snapshot() {
	const char * filename;
	if(CMD_ARGC() >= 2) {
		filename = CMD_ARGV(1);
	}
	else {
		char tmpname[MAX_PATH];
		filename = get_avail_file(tmpname, "_bmod.bmp");
	}
	FILE * file = fopen(filename, "wb");
	if(!file) {
		MF_PrintSrvConsole("Couldn't open file '%s' for writing\n", filename);
		return;
	}

	edict_t * player = NULL;
	for(int i = 1; i <= gpGlobals->maxClients; i++) {
		player = INDEXENT(i);
		if(player && !strcmp("player", STRING(player->v.classname)))
			break;
		player = NULL;
	}
	if(!player) {
		MF_PrintSrvConsole("No valid player found!\n");
		fclose(file);
		return;
	}

	Vector origin = player->v.origin;
	origin.z += 18;
	Vector angles = player->v.v_angle;
	float fov = player->v.fov;
	int width = 1024;
	int height = 768;
	if(CMD_ARGC() >= 3) {
		width = atoi(CMD_ARGV(2));
	}
	if(CMD_ARGC() >= 4) {
		height = atoi(CMD_ARGV(3));
	}

	//bmp header
	fwrite("\x42\x4D", 2, 1, file);
	int bmpsize = 54 + width * height * 4;
	fwrite(&bmpsize, 4, 1, file);
	fwrite("\x00\x00\x00\x00\x36\x00\x00\x00", 8, 1, file);

	//dib header
	fwrite("\x28\x00\x00\x00", 4, 1, file);
	fwrite(&width, 4, 1, file);
	fwrite(&height, 4, 1, file);
	fwrite("\x01\x00\x20\x00\x00\x00\x00\x00", 8, 1, file);
	int bitsize = bmpsize - 54;
	fwrite(&bitsize, 4, 1, file);
	fwrite("\x13\x0B\x00\x00\x13\x0B\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16, 1, file);

	//farplane
	float distance = 99999;
	Vector forward, right, up;
	g_engfuncs.pfnAngleVectors(angles, forward, right, up);
	forward = forward * distance;
	float fov_tan = btTan(0.5f * fov * SIMD_RADS_PER_DEG);
	right = right * distance * fov_tan;
	up = up * distance * fov_tan * ((double)height/width);

	//pixels
	btVector3 from;
	origin.CopyToArray(from.m_floats);
	btVector3 bottomLeft;
	for(int i = 0; i < 3; i++)
		bottomLeft[i] = from[i] + forward[i] - right[i] - up[i];
	byte color[4];
	color[3] = 0;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			color[0] = color[1] = color[2] = 127;
			btVector3 to = bottomLeft;
			for(int i = 0; i < 3; i++) {
				to[i] += ((right[i] * 2) * x) / width;
				to[i] += ((up[i] * 2) * y) / height;
			}
			btCollisionWorld::ClosestRayResultCallback ray(from, to);
			g_bt_dynamicsWorld->rayTest(from, to, ray);
			if(ray.hasHit()) {
				for(int i = 0; i < 3; i++) {
					color[2 - i] = ((ray.m_hitNormalWorld[i] + 1)/2)*255;
				}
			}
			fwrite(color, 1, 4, file);
		}
	}

	fclose(file);
	MF_PrintSrvConsole("Wrote %d byte snapshot into output file '%s'; resolution=%dx%d; origin=[%f;%f;%f]; yaw=%f; pitch=%f; fov=%f\n", bmpsize, filename, width, height, origin.x, origin.y, origin.z, angles.y, angles.x, fov);
}

void reg_debug_cmds() {
	REG_SVR_COMMAND("bmod_serialize", bmod_serialize);
	REG_SVR_COMMAND("bmod_snapshot", bmod_snapshot);
}
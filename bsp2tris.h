
typedef struct{
	float*vertices;
	int*indices;
	int vertices_c;
	int indices_c;
}tris_s;

tris_s * bsp2tris(const char*,int);
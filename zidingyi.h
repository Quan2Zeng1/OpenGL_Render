#pragma once
#include <string>

#define TEXTURE_N 1
#define MATERIAL_N 2
#define VERTICES_N 3093
#define TEXCOORDS_N 2815
#define NORMOLS_N 3090
#define MODELS_N 2
#define TRIANGLE_NA 1718
#define TRIANGLE_NB 4282

using namespace std;

struct Model_data
{
	int model_num;
	int point_num;
	int texture_index;
	char texture_name[20];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emission[4];
	float shininess;
	float* model;
};

Model_data* ReadTxt(string filename);
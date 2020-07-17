#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "zidingyi.h"
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

//Model_data ReadTxt(string filename);

Model_data* ReadTxt(string filename)
{
	ifstream infile(filename);

	//纹理贴图文件名
	int n;
	infile >> n;
	char sss[4][20];
	char* sssss[4];
	for (int i = 0; i < n; i++)
	{
		infile >> sss[i];
		sssss[i] = sss[i];
	}

	//材质
	infile >> n;
	int* textureIndex = (int*)calloc(n, sizeof(int));
	float* material = (float*)calloc(n*18, sizeof(float));
	for (int i = 0; i < n*18; i++)
	{
		if ((i+1) % 18== 0 && i != 0)
		{
			infile >> textureIndex[(i+1) / 18 - 1];
		}
		else {
			infile >> material[i];
			/*for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					infile >> model_data->material[i][j][k];
				}
			}
			infile >> model_data->shininess[i];
			infile >> model_data->tex_index[i];*/
		}
	}
	//顶点坐标
	infile >> n;
	float* vertices = (float* )calloc(n * 3, sizeof(float));
	for (int i = 0; i < n * 3; i++)
	{
		//infile >> model_data->vertices[i];
		infile >> vertices[i];
	}

	//纹理坐标
	infile >> n;
	float* texCoords = (float*)calloc(n * 2, sizeof(float));
	for (int i = 0; i < n*2; i++)
	{
		infile >> texCoords[i];
	}
	
	//法线坐标
	infile >> n;
	float* normals = (float*)calloc(n * 3, sizeof(float));
	for (int i = 0; i < n * 3; i++)
	{
		infile >> normals[i];
	}

	//索引
	infile >> n;
	Model_data* model = (Model_data*)calloc(n, sizeof(Model_data));
	model[0].model_num = n;
	for (int i = 0; i < 3; i++)
	{
		float k;
		infile >> k;
	}

	int m;
	int indx;

	for (int i = 0; i < n; i++)
	{
		infile >> m;
		float* data = (float*)calloc(m * 3 * 8, sizeof(float));
		model[i].point_num = m;
		//model[i] 
		int fileindex;
		infile >> fileindex;

		//索引数据
		for (int j = 0; j < m * 3; j++)
		{
			infile >> indx;
			for (int k = 0; k < 3; k++)
			{
				data[j * 8 + k] = vertices[(indx - 1) * 3 + k];
				//model_data->model1[j * (3 + 2 + 3) + k] = vertices[(indx-1) * 3 + k];
			}
			infile >> indx;
			for (int k = 0; k < 2; k++)
			{
				data[j * 8 + 3 + k] = texCoords[(indx - 1) * 2 + k];
				//model_data->model1[j * (3 + 2 + 3) + 3 + k] = texCoords[(indx-1) * 2 + k];
			}
			infile >> indx;
			for (int k = 0; k < 3; k++)
			{
				data[j * 8 + 5 + k] = normals[(indx - 1) * 3 + k];
				//model_data->model1[j * (3 + 2 + 3) +3+2+ k] = normals[(indx-1) * 3 + k];
			}
		}
		model[i].model = data;
		model[i].texture_index = fileindex;
		if (fileindex != 0)
		{
			for (int c = 0; c < 4; c++)
			{
				model[i].ambient[c] = material[c + (fileindex - 1) * 18];
				model[i].diffuse[c] = material[c + 4 + (fileindex - 1) * 18];
				model[i].specular[c] = material[c + 8 + (fileindex - 1) * 18];
				model[i].emission[c] = material[c + 12 + (fileindex - 1) * 18];
			}
			model[i].shininess = material[(fileindex - 1) * 18 + 16];

			for (int v = 0; v < 20; v++)
			{
				model[i].texture_name[v] = sssss[textureIndex[fileindex - 1] - 1][v];
			}
			char z;
			for (int p = 0; p < 20; p++) {
			}
		}
		//strcpy(model[i].textureFile, sssss[textureIndex[fileindex - 1] - 1]);
	}

	free(vertices);
	free(texCoords);
	free(normals);
	free(material);
	//free(model);

	return model;
}
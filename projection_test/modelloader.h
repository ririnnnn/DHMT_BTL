#include "Angel.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int get_vertices_number(string path)
{
	ifstream file(path.c_str());
	string line;
	int i = 0;
	while (getline(file, line))
	{

		if (line[0] == 'v' && line[1] != 'n') i++;
	}
	file.close();
	return i;
}

int get_numpoints(string path)
{
	ifstream file(path.c_str());
	string line;
	int i = 0;
	while (getline(file, line))
	{

		if (line[0] == 'f') i++;
	}
	file.close();
	return i * 3;
}

int get_vn(string path)
{
	ifstream file(path.c_str());
	string line;
	int i = 0;
	while (getline(file, line))
	{
		if (line[0] == 'v' && line[1] == 'n') i++;
	}
	file.close();
	return i;
}

void load_model(vec4 out_vertices[], vec3 out_normal[], string path)
{
	ifstream file(path.c_str());
	string line;
	string processed_file;
	int vertices_number = get_vertices_number(path);
	vec4 vertices_temp[5000];
	vec3 normals_temp[5000];
	vec4 vec4_temp;
	vec3 vec3_temp;
	int i = 0;
	int j = 0;
	int k = 0;
	while (getline(file, line))
	{
		string temp;
		stringstream vertices;
		stringstream s;
		s << line;
		if (line[0] == 'v' && line[1] != 'n')
		{
			s >> temp >> vec4_temp.x >> vec4_temp.y >> vec4_temp.z;
			vec4_temp.w = 1.0;
			vertices_temp[i] = vec4_temp;
			i++;
		}
		else if (line[0] == 'v' && line[1] == 'n')
		{
			s >> temp >> vec3_temp.x >> vec3_temp.y >> vec3_temp.z;
			out_normal[k] = vec3_temp;
			k++;
		}
		else if (line[0] == 'f')
		{
			s >> temp;
			string junk;
			while (!s.eof())
			{
				s >> temp;
				string index;
				stringstream ss(temp);
				getline(ss, index, '/');
				out_vertices[j] = vertices_temp[stoi(index) - 1];
				getline(ss, index, '/');
				getline(ss, index, '/');
				out_normal[j] = normals_temp[stoi(index) - 1];
				j++;
			}
		}
	}
	file.close();
}

#include "Angel.h"

struct material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	double shininess;
	int id;
};

const struct material Black_plastic = {
		vec4(0.0, 0.0, 0.0, 1.0),
		vec4(0.01, 0.01, 0.01, 1.0),
		vec4(0.5, 0.5, 0.5, 1.0),
		32,
		0
};

const struct material Brass = {
		vec4(0.329412, 0.223529, 0.027451, 1.0),
		vec4(0.780392, 0.568627, 0.1137251, 1.0),
		vec4(0.002157, 0.941176, 0.807843, 1.0),
		27.8974,
		1
};

const struct material Polished_silver = {
		vec4(0.23125, 0.23125, 0.23125, 1.0),
		vec4(0.2775, 0.2775, 0.2775, 1.0),
		vec4(0.773911, 0.773911, 0.773911, 1.0),
		89.6,
		2
};
const struct material Grass = {
		vec4(0.02, 0.4, 0.1, 1.0),
		vec4(0.02, 0.4, 0.1, 1.0),
		vec4(0.0, 0.1, 0.1, 1.0),
		10.0,
		3
};
const struct material Sorlar_panel = {
		vec4(0.11372, 0.11372, 0.46667, 1.0),
		vec4(0.11372, 0.11372, 0.46667, 1.0),
		vec4(0.45, 0.45, 0.7, 1.0),
		40,
		4
};
const struct material Green = {
		vec4(0.01, 1, 0.01, 1.0),
		vec4(0.0, 1, 0, 1.0),
		vec4(0.0, 1, 0, 1.0),
		40,
		5
};
const struct material Kinh = {
		vec4(0.1, 0.18725, 0.1745),
		vec4(0.396, 0.74151, 0.69102),
		vec4(0.297254, 0.30829, 0.306678),
		40,
		6
};
const struct material MatTrang = {
		vec4(1.0, 0.0, 0.0),
		vec4(0.9, 0.9, 0.9),
		vec4(1.0, 1.0, 1.0),
		30,
		7
};
const struct material MatTroi = {
		vec4(1.0, 0.0, 0.0),
		vec4(0.9, 0.9, 0.9),
		vec4(1.0, 1.0, 1.0),
		30,
		8
};
const struct material SonTrang = {
		vec4(0.8, 0.8, 0.8, 0.5),
		vec4(0.8, 0.8, 0.8, 0.5),
		vec4(1.0, 1.0, 1.0, 0.5),
		10,
		9
};
const struct material Bac = {
		vec4(0.19225, 0.19225, 0.19225),
		vec4(0.50754, 0.50754, 0.50754),
		vec4(0.508273, 0.508273, 0.508273),
		60,
		10
};
const struct material Grass_leaf = {
		vec4(0.05, 1.0, 0.25, 1.0),
		vec4(0.02, 0.4, 0.1, 1.0),
		vec4(0.0, 0.1, 0.1, 1.0),
		10.0,
		11
};
const struct material Test_cloud = {
		vec4(1.0, 1.0, 1.0, 1.0),
		vec4(0.0, 0.0, 0.0, 1.0),
		vec4(0.0, 0.0, 0.0, 1.0),
		10.0,
		12
};
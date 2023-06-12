//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/
#include "camera.h"
#include "material.h"
#include <string>
#include <thread>
#include "modelloader.h"

// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints];

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program, shadow_program;

//################################################################

const float max_fps = 60.0f;
const float pi = 3.1415926535897932;

unsigned int loc_projMatrix;
unsigned int loc_viewMatrix;
unsigned int loc_modelMatrix;
unsigned int loc_AmbientProduct;
unsigned int loc_DiffuseProduct;
unsigned int loc_SpecularProduct;
unsigned int loc_LightPosition;
unsigned int loc_Shininess;
unsigned int loc_EyePosition;
unsigned int loc_grass_bend;
unsigned int loc_is_grass;
unsigned int loc_is_sun;

unsigned int loc_shadow;
unsigned int loc_LightMatrix;
unsigned int depthMapFBO, sceneFBO;
unsigned int depthMap;

unsigned int ground_map;

float grass_bend;

const unsigned int SHADOW_WIDTH = 2000, SHADOW_HEIGHT = 2000;

int cyl_numpoints = 0;
vec4 cyl_vertices[372];
vec3 cyl_normals[372];

int wing_numpoints = 0;
vec4 wing_vertices[780];
vec3 wing_normals[780];

int sphere_numpoints = 0;
vec4 sphere_vertices[11904];
vec3 sphere_normals[11904];

vec4 grass_data[400];

int grass_numpoints = 1200;
vec4 grass_vertices[1200];
vec3 grass_normals[1200];

material lastmat;

float quat_quay, pin_quay;
bool nd;

const int cloud_count = 100;
vec3 cloud_data_size[cloud_count];
vec3 cloud_data_pos[cloud_count];

void load_cyl()
{
	load_model(cyl_vertices, cyl_normals, "cyl.txt");
}

void load_wing()
{
	load_model(wing_vertices, wing_normals, "wing.txt");
}

void load_sphere()
{
	load_model(sphere_vertices, sphere_normals, "sphere.txt");
}

bool control = false, open = false;
bool keys[6] = { false };

double vecup = 0.0;

struct mouse
{
	bool left, right, middle;
	int x, y;
	mouse()
	{
		left = middle = right = false;
		x = 0, y = 0;
	}
};

struct light_source
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
};

light_source light_test = {
	vec4(0.3, 0.3, 0.3, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 170.0, 0.0, 1.0)
};

mouse m;

cameraman player;
cl timer;
double frame_time = 0.001;

double time_counter = 0.0f;
int frame_counter = 0;

//################################################################

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void generate_cyl_nomal()
{
	vec4 u, v;
	vec3 normal;
	for (int i = 0; i < cyl_numpoints; i += 3)
	{
		u = cyl_vertices[i + 1] - cyl_vertices[i];
		v = cyl_vertices[i] - cyl_vertices[i + 2];
		normal = -normalize(cross(u, v));
		cyl_normals[i] = cyl_normals[i + 1] = cyl_normals[i + 2] = normal;
	}
}
void generate_wing_nomal()
{
	vec4 u, v;
	vec3 normal;
	for (int i = 0; i < wing_numpoints; i += 3)
	{
		u = wing_vertices[i + 1] - wing_vertices[i];
		v = wing_vertices[i] - wing_vertices[i + 2];
		normal = -normalize(cross(u, v));
		wing_normals[i] = wing_normals[i + 1] = wing_normals[i + 2] = normal;
	}
}
void generate_sphere_nomal()
{
	vec4 u, v;
	vec3 normal;
	for (int i = 0; i < sphere_numpoints; i += 3)
	{
		u = sphere_vertices[i + 1] - sphere_vertices[i];
		v = sphere_vertices[i] - sphere_vertices[i + 2];
		normal = -normalize(cross(u, v));
		sphere_normals[i] = sphere_normals[i + 1] = sphere_normals[i + 2] = normal;
	}
}
void generate_grass()
{
	vec4 u, v;
	vec3 normal;
	for (int i = 0; i < grass_numpoints; i += 3)
	{
		mat4 model = Scale(0.3, 0.3, 0.3) * Translate(0.0, 0.0, 0.5);
		model = Translate(grass_data[i / 3].x, grass_data[i / 3].z + 0.15, grass_data[i / 3].y) * RotateY(grass_data[i / 3].w) * model;
		
		grass_vertices[i] = model * cyl_vertices[0];
		grass_vertices[i + 1] = model * cyl_vertices[1];
		grass_vertices[i + 2] = model * cyl_vertices[2];

		u = grass_vertices[i + 1] - grass_vertices[i];
		v = grass_vertices[i] - grass_vertices[i + 2];
		normal = -normalize(cross(u, v));
		grass_normals[i] = grass_normals[i + 1] = grass_normals[i + 2] = normal;
	}
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
	load_cyl();
	cyl_numpoints = get_numpoints("cyl.txt");
	generate_cyl_nomal();
	load_wing();
	wing_numpoints = get_numpoints("wing.txt");
	generate_wing_nomal();
	load_sphere();
	sphere_numpoints = get_numpoints("sphere.txt");
	generate_sphere_nomal();

	for (int i = 0; i < 400; i++)
	{
		grass_data[i].x = (float)((rand() % 4000) / 1000.0) - 2.0;
		grass_data[i].y = (float)((rand() % 4000) / 1000.0) - 2.0;
		grass_data[i].z = (float)(rand() % 30) / 200.0;
		grass_data[i].w = (float)(rand() % 90);
	}
	for (int i = 0; i < cloud_count; i++)
	{
		cloud_data_size[i].x = float(rand() % 1000) / 100.0 + 5.0;
		cloud_data_size[i].z = float(rand() % 1000) / 100.0 + 5.0;
		cloud_data_size[i].y = 5.0;
		cloud_data_pos[i].x = float(rand() % 1000) / 5.0 - 100.0;
		cloud_data_pos[i].z = float(rand() % 1000) / 5.0 - 100.0;
		cloud_data_pos[i].y = 100;
	}
	generate_grass();
}

void initGPUBuffers( void )
{
	// Tạo một VAO - vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );     
    glBindVertexArray( vao );

	int vertices_size = sizeof(points) + sizeof(cyl_vertices) + sizeof(wing_vertices) + sizeof(sphere_vertices) + sizeof(grass_vertices);
	int color_size = sizeof(colors);
	int normal_size = sizeof(normals) + sizeof(cyl_normals) + sizeof(wing_normals) + sizeof(sphere_normals) + sizeof(grass_normals);
	GLuint buffer;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices_size + color_size + normal_size, NULL, GL_STATIC_DRAW);

	int prevsize = 0;
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(points), points);
	prevsize += sizeof(points);
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(cyl_vertices), cyl_vertices);
	prevsize += sizeof(cyl_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(wing_vertices), wing_vertices);
	prevsize += sizeof(wing_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(sphere_vertices), sphere_vertices);
	prevsize += sizeof(sphere_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(grass_vertices), grass_vertices);
	prevsize += sizeof(grass_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(colors), colors);
	prevsize += sizeof(colors);
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(normals), normals);
	prevsize += sizeof(normals);
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(cyl_normals), cyl_normals);
	prevsize += sizeof(cyl_normals);
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(wing_normals), wing_normals);
	prevsize += sizeof(wing_normals);
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(sphere_normals), sphere_normals);
	prevsize += sizeof(grass_normals);
	glBufferSubData(GL_ARRAY_BUFFER, prevsize, sizeof(grass_normals), grass_normals);

	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void shaderSetup( void )
{
	// Nạp các shader và sử dụng chương trình shader
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );   // hàm InitShader khai báo trong Angel.h
    glUseProgram( program );

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(cyl_vertices) + sizeof(wing_vertices) + sizeof(sphere_vertices) + sizeof(grass_vertices)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(cyl_vertices) + sizeof(wing_vertices) + sizeof(sphere_vertices) + sizeof(grass_vertices) + sizeof(colors)));

	//###########################################################

	loc_projMatrix = glGetUniformLocation(program, "projMatrix");
	mat4 projMatrix = player.projMat;
	glUniformMatrix4fv(loc_projMatrix, 1, GL_TRUE, projMatrix);

	loc_viewMatrix = glGetUniformLocation(program, "viewMatrix");

	loc_modelMatrix = glGetUniformLocation(program, "modelMatrix");
	mat4 modelMatrix = Translate(0.0, 0.0, 0.0);
	glUniformMatrix4fv(loc_modelMatrix, 1, GL_TRUE, modelMatrix);

	loc_AmbientProduct = glGetUniformLocation(program, "AmbientProduct");
	loc_DiffuseProduct = glGetUniformLocation(program, "DiffuseProduct");
	loc_SpecularProduct = glGetUniformLocation(program, "SpecularProduct");
	loc_Shininess = glGetUniformLocation(program, "Shininess");
	loc_LightPosition = glGetUniformLocation(program, "LightPosition");
	loc_EyePosition = glGetUniformLocation(program, "EyePosition");
	loc_grass_bend = glGetUniformLocation(program, "grass_bend");
	loc_is_grass = glGetUniformLocation(program, "is_grass");
	loc_is_sun = glGetUniformLocation(program, "is_sun");

	loc_shadow = glGetUniformLocation(program, "shadow");
	loc_LightMatrix = glGetUniformLocation(program, "LightMatrix");

	//###########################################################
	glEnable(GL_DEPTH_TEST);
}

//###########################################################

void draw_shape(vec3 trans, vec3 scale, material mat, light_source source, int shape)
{	
	if (lastmat.id != mat.id)
	{
		vec4 ambient_product = source.ambient * mat.ambient;
		vec4 diffuse_product = source.diffuse * mat.diffuse;
		vec4 specular_product = source.specular * mat.specular;

		glUniform4fv(loc_AmbientProduct, 1, ambient_product);
		glUniform4fv(loc_DiffuseProduct, 1, diffuse_product);
		glUniform4fv(loc_SpecularProduct, 1, specular_product);
		glUniform1f(loc_Shininess, mat.shininess);
		
		lastmat = mat;
	}
	glUniform4fv(loc_LightPosition, 1, source.position);
	mat4 model = Translate(trans) * Scale(scale);
	glUniformMatrix4fv(loc_modelMatrix, 1, GL_TRUE, model);
	if (shape == 0) glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	else if (shape == 1) glDrawArrays(GL_TRIANGLES, 36, cyl_numpoints);
	else if (shape == 2) glDrawArrays(GL_TRIANGLES, NumPoints + cyl_numpoints, wing_numpoints);
	else if (shape == 3) glDrawArrays(GL_TRIANGLES, NumPoints + cyl_numpoints + wing_numpoints, sphere_numpoints);
}

void draw_shape(vec3 trans, vec3 scale, mat4 instance, material mat, light_source source, int shape)
{
	if (lastmat.id != mat.id)
	{
		vec4 ambient_product = source.ambient * mat.ambient;
		vec4 diffuse_product = source.diffuse * mat.diffuse;
		vec4 specular_product = source.specular * mat.specular;

		glUniform4fv(loc_AmbientProduct, 1, ambient_product);
		glUniform4fv(loc_DiffuseProduct, 1, diffuse_product);
		glUniform4fv(loc_SpecularProduct, 1, specular_product);
		glUniform1f(loc_Shininess, mat.shininess);
		lastmat = mat;
	}
	glUniform4fv(loc_LightPosition, 1, source.position);
	mat4 model = instance * Translate(trans) * Scale(scale);
	glUniformMatrix4fv(loc_modelMatrix, 1, GL_TRUE, model);
	if(shape == 0) glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	else if(shape == 1) glDrawArrays(GL_TRIANGLES, 36, cyl_numpoints);
	else if(shape == 2) glDrawArrays(GL_TRIANGLES, NumPoints + cyl_numpoints, wing_numpoints);
	else if (shape == 3) glDrawArrays(GL_TRIANGLES, NumPoints + cyl_numpoints + wing_numpoints, sphere_numpoints);
}

void draw_ground_square(int x, int z, material mat, light_source source, float grass_density)
{
	if (lastmat.id != mat.id)
	{
		vec4 ambient_product = source.ambient * mat.ambient;
		vec4 diffuse_product = source.diffuse * mat.diffuse;
		vec4 specular_product = source.specular * mat.specular;

		glUniform4fv(loc_AmbientProduct, 1, ambient_product);
		glUniform4fv(loc_DiffuseProduct, 1, diffuse_product);
		glUniform4fv(loc_SpecularProduct, 1, specular_product);
		glUniform1f(loc_Shininess, mat.shininess);
		lastmat = mat;
	}
	glUniform4fv(loc_LightPosition, 1, source.position);
	mat4 model = Translate(x * 4.0, -0.5, z * 4.0) * Scale(4.0, 1.0, 4.0);
	glUniformMatrix4fv(loc_modelMatrix, 1, GL_TRUE, model);
	glDrawArrays(GL_TRIANGLES, 18, 6);
	model = Translate(x * 4.0, 0.0, z * 4.0);
	glUniformMatrix4fv(loc_modelMatrix, 1, GL_TRUE, model);
	if (grass_density < 0) grass_density = 0;
	else if (grass_density > 1) grass_density = 1.0;

	glUniform1i(loc_is_grass, 1);
	glDrawArrays(GL_TRIANGLES, NumPoints + cyl_numpoints + wing_numpoints + sphere_numpoints, grass_numpoints * grass_density);
	glUniform1i(loc_is_grass, 0);
}

void draw_ground_follow(int range)
{
	int camera_x = (player.eye.x - 2) / 4;
	int camera_z = (player.eye.z - 2) / 4;
	int half = range / 2;
	for (int i = -half + camera_x; i < half + camera_x; i++)
		for (int j = -half + camera_z; j < half + camera_z; j++)
		{
			int distance = (i - camera_x) * (i - camera_x) + (j - camera_z) * (j - camera_z);
			if (distance <= 25) draw_ground_square(i, j, Grass, light_test, 1.0);
			else if (distance <= 100) draw_ground_square(i, j, Grass, light_test, 0.5);
			else draw_ground_square(i, j, Grass, light_test, 0.0);
		}
}
// vẽ mặt trời
void mattroi(mat4 all) {
	material mattroi = MatTroi;
	glUniform1i(loc_is_sun, 1);
	draw_shape(vec3(0.0, 200.0, 0.0), vec3(20.0, 20.0, 20.0), all, mattroi, light_test, 3);
	glUniform1i(loc_is_sun, 0);
}
// vẽ mặt trăng
void mattrang(mat4 all) {
	material mattroi = Kinh;
	glUniform1i(loc_is_sun, 2);
	draw_shape(vec3(0.0, -200.0, 0.0), vec3(20.0, 20.0, 20.0), all, mattroi, light_test, 3);
	glUniform1i(loc_is_sun, 0);
}
// vẽ pin mặt trời
void de(mat4 all) {
	material brass = Bac;
	draw_shape(vec3(0, -0.63, 0), vec3(0.3, 0.16, 0.3), all, brass, light_test, 1);
}
void than(mat4 all) {
	material brass = Bac;
	draw_shape(vec3(0, -0.24, 0), vec3(0.12, 0.8, 0.12), all, brass, light_test, 1);
}
void thanhngang(mat4 all) {
	material brass = Bac;
	draw_shape(vec3(0, 0.04, 0), vec3(1.05, 0.08, 0.08), all, brass, light_test, 0);
}
void khunggiu(mat4 all) {
	material brass = Bac;
	draw_shape(vec3(0.476, 0.08, 0), vec3(0.06, 0.02, 1.99), all, brass, light_test, 0);
	draw_shape(vec3(-0.476, 0.08, 0), vec3(0.06, 0.02, 1.99), all, brass, light_test, 0);
	draw_shape(vec3(0.516, 0.1, 0), vec3(0.02, 0.06, 1.99), all, brass, light_test, 0);
	draw_shape(vec3(-0.516, 0.1, 0), vec3(0.02, 0.06, 1.99), all, brass, light_test, 0);
}
void khung_pin(mat4 all) {
	material bac = Bac;
	draw_shape(vec3(0, 0.11, -0.983), vec3(0.964, 0.04, 0.024), all, bac, light_test, 0);
	draw_shape(vec3(0, 0.11, 0.983), vec3(0.964, 0.04, 0.024), all, bac, light_test, 0);
	draw_shape(vec3(0.494, 0.11, 0), vec3(0.024, 0.04, 1.99), all, bac, light_test, 0);
	draw_shape(vec3(-0.494, 0.11, 0), vec3(0.024, 0.04, 1.99), all, bac, light_test, 0);
}
void kinh_pin(mat4 all) {
	material kinh = Kinh;
	draw_shape(vec3(0, 0.11, 0), vec3(0.964, 0.035, 1.942), all, kinh, light_test, 0);
}
void duong_ke(mat4 all) {
	material bac = Bac;
	draw_shape(vec3(0.0974, 0.131, 0), vec3(0.006, 0.002, 1.942), all, bac, light_test, 0);
	draw_shape(vec3(-0.0974, 0.131, 0), vec3(0.006, 0.002, 1.942), all, bac, light_test, 0);
	draw_shape(vec3(0.2902, 0.131, 0), vec3(0.006, 0.002, 1.942), all, bac, light_test, 0);
	draw_shape(vec3(-0.2902, 0.131, 0), vec3(0.006, 0.002, 1.942), all, bac, light_test, 0);
	draw_shape(vec3(0.483, 0.131, 0), vec3(0.006, 0.002, 1.942), all, bac, light_test, 0);
	draw_shape(vec3(-0.483, 0.131, 0), vec3(0.006, 0.002, 1.942), all, bac, light_test, 0);

	draw_shape(vec3(0, 0.132, 0.1942), vec3(0.964, 0.008, 0.004), all, bac, light_test, 0);
	draw_shape(vec3(0, 0.132, -0.1942), vec3(0.964, 0.008, 0.004), all, bac, light_test, 0);
	draw_shape(vec3(0, 0.132, 0.5826), vec3(0.964, 0.008, 0.004), all, bac, light_test, 0);
	draw_shape(vec3(0, 0.132, -0.5826), vec3(0.964, 0.008, 0.004), all, bac, light_test, 0);

}

void draw_frame(mat4 all) {
	material st = SonTrang;
	draw_shape(vec3(0, 0, 0), vec3(0.16, 2.4, 0.16), all, st, light_test, 1);
	draw_shape(vec3(0, 1.27, 0), vec3(0.5, 0.14, 0.14), all, st, light_test, 0);
	draw_shape(vec3(0.285, 1.27, 0.0), vec3(0.14, 0.14, 0.14), all, st, light_test, 1);
}

void draw_wind(mat4 all) {
	material st = SonTrang;
	all = all * RotateY(40);
	draw_shape(vec3(0.0, 0.0, 0.0), vec3(0.2, 0.15, 0.1), all, st, light_test, 2);
}

void draw_tuabine(vec3 T, float theta) {
	mat4 instance;

	instance = Translate(T) * Scale(10.0, 10.0, 10.0);
	draw_frame(instance);
	instance = instance * Translate(0.25, 1.23, 0) * RotateX(theta);
	draw_wind(instance);
	instance = instance * RotateX(120);
	draw_wind(instance);
	instance = instance * RotateX(120);
	draw_wind(instance);
}

void pin(vec3 T, float theta) {
	mat4 x;
	x = Translate(T);
	de(x);
	than(x);
	if(theta < -80)
	{
		theta = -80;
	}
	if (theta > 80)
	{
		theta = 80;
	}
	x = x * Translate(0, 0.16, 0) * RotateZ(theta);
	thanhngang(x);
	khunggiu(x);
	khung_pin(x);
	kinh_pin(x);
	duong_ke(x);
}

void perf_test(int load)
{
	for (int i = 0; i < load; i++)
		for (int j = 0; j < load; j++)
			draw_shape(vec3(2.0 * i - (load / 2 * 2.0), -4.0, 2.0 * j - (load / 2 * 2.0)), vec3(1.0, 1.0, 1.0), Polished_silver, light_test, 0);
	for (int i = 0; i < load; i++)
		for (int j = 0; j < load; j++)
			draw_shape(vec3(2.0 * i - (load / 2 * 2.0), -1.0, 2.0 * j - (load / 2 * 2.0)), vec3(1.0, 1.0, 1.0), Sorlar_panel, light_test, 1);
	for (int i = 0; i < load; i++)
		for (int j = 0; j < load; j++)
			draw_shape(vec3(2.0 * i - (load / 2 * 2.0), 2.0, 2.0 * j - (load / 2 * 2.0)), vec3(1.0, 1.0, 1.0), Grass, light_test, 2);
	for (int i = 0; i < load; i++)
		for (int j = 0; j < load; j++)
			draw_shape(vec3(2.0 * i - (load / 2 * 2.0), 13.0, 2.0 * j - (load / 2 * 2.0)), vec3(1.0, 1.0, 1.0), Polished_silver, light_test, 3);
}

void main_draw(vec3 T)
{
	for(int i = 0; i < 12; i++)
		for(int j = 0; j < 19; j++)
			pin(vec3(1.2 * j + 6, 0.725, 2.0 * i - 9) + T, pin_quay);
	draw_tuabine(vec3(0.0, 12.0, 22.0) + T, quat_quay);
}

void render_shadow()
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUniform1i(loc_shadow, 1);
	mat4 LightMatrix = LookAt(light_test.position, player.eye, vec3(0.0, 1.0, 0.0));
	LightMatrix = Perspective(45, 1, 100, 300) * LightMatrix;
	glUniformMatrix4fv(loc_LightMatrix, 1, GL_TRUE, LightMatrix);

	for (int i = 0; i < 2; i++)
	{
		draw_tuabine(vec3(24.0 * (i - 1), 12.0, -58.0), quat_quay);
		for (int j = 0; j < 2; j++)
		{
			main_draw(vec3(24.0 * (i - 1), 0.0, 40 * (j - 1)));
		}
	}
	for (int j = 0; j < 3; j++)
	{
		draw_tuabine(vec3(24.0, 12.0, 22 - (40 * j)), quat_quay);
	}

	glUniform1i(loc_shadow, 0);
}

void render_scene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	grass_bend += 1.5 * frame_time;
	if (grass_bend > 2 * pi) grass_bend -= 2* pi;
	glUniform1f(loc_grass_bend, grass_bend);

	if (nd)
		pin_quay += frame_time * 10.0f;
	if (pin_quay > 180)
		pin_quay = pin_quay - 360;


	if (pin_quay >= -90 && pin_quay <= 90)
	{
		light_test.position = RotateZ(pin_quay) * vec4(0.0, 170.0, 0.0, 1.0);
		light_test.ambient = vec4(0.3, 0.3, 0.3, 1.0);
		light_test.diffuse = vec4(1.0, 1.0, 1.0, 1.0);
		light_test.specular = vec4(1.0, 1.0, 1.0, 1.0);
		glClearColor(0.0, 0.5, 1.0, 0.0);
	}
	else
	{
		light_test.position = RotateZ(pin_quay) * vec4(0.0, -170.0, 0.0, 1.0);
		light_test.ambient = vec4(0.2, 0.2, 0.2, 1.0);
		light_test.diffuse = vec4(0.2, 0.2, 0.2, 1.0);
		light_test.specular = vec4(0.2, 0.2, 0.2, 1.0);
		glClearColor(0.0, 0.2, 0.5, 0.0);
	}
	mat4 all = RotateZ(pin_quay);
	mattroi(all);
	mattrang(all);
	quat_quay -= frame_time * 180.0f;

	for (int i = 0; i < 2; i++)
	{
		draw_tuabine(vec3(24.0 * (i - 1), 12.0, -58.0), quat_quay);
		for (int j = 0; j < 2; j++)
		{
			main_draw(vec3(24.0 * (i - 1), 0.0, 40 * (j - 1)));
		}
	}
	for (int j = 0; j < 3; j++)
	{
		draw_tuabine(vec3(24.0, 12.0, 22 - (40 * j)), quat_quay);
	}

	draw_ground_follow(30);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniform1i(loc_is_sun, 3);
	for (int i = 0; i < 20; i++)
		draw_shape(cloud_data_pos[i], cloud_data_size[i], Brass, light_test, 0);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glUniform1i(loc_is_sun, 0);
}
//###########################################################

void display( void )
{
	
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//update camera
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	player.update(frame_time, width, height);
	//player.setCamera_y(0.0);
	mat4 projMatrix = player.projMat;
	glUniformMatrix4fv(loc_projMatrix, 1, GL_TRUE, projMatrix);
	mat4 viewMatrix = player.viewMat;
	glUniformMatrix4fv(loc_viewMatrix, 1, GL_TRUE, viewMatrix);
	glUniform3fv(loc_EyePosition, 1, player.eye);
	
	//draw
	render_shadow();
	render_scene();

	//

	frame_time = timer.check();
	if (frame_time < 1000.0 / max_fps)
	{
		std::this_thread::sleep_for(microseconds((int)(1000.0 / max_fps - frame_time) * 1000));
	}

	frame_time = timer.gettime() / 1000.0f;
	time_counter += frame_time;
	frame_counter++;
	if (time_counter >= 0.1)
	{
		string title = "FPS: " + std::to_string(1.0 / (time_counter / frame_counter));
		title += " x=" + std::to_string(player.eye.x) + " ";
		title += " y=" + std::to_string(player.eye.y) + " ";
		title += " z=" + std::to_string(player.eye.z) + " ";
		glutSetWindowTitle(title.c_str());
		time_counter = 0.0f;
		frame_counter = 0;
	}
	glutSwapBuffers();
	glutPostRedisplay();
}


void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
    case 033:			// 033 is Escape key octal value
        exit(1);		// quit program
        break;
	case 'a':
		keys[0] = true;
		break;
	case 'd':
		keys[1] = true;
		break;
	case 's':
		keys[2] = true;
		break;
	case 'w':
		keys[3] = true;
		break;
	case 'e':
		keys[4] = true;
		break;
	case 'q':
		keys[5] = true;
		break;
	case 'n':
		nd = !nd;
		break;
	case 32:
		vecup = 0.01;
		break;
    }

}

void keyboardup(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case 'a':
		keys[0] = false;
		break;
	case 'd':
		keys[1] = false;
		break;
	case 's':
		keys[2] = false;
		break;
	case 'w':
		keys[3] = false;
		break;
	case 'e':
		keys[4] = false;
		break;
	case 'q':
		keys[5] = false;
		break;
	}

}

void mouse_bt(int button, int stage, int x, int y)
{
	if (stage == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON) m.left = true;
		else if (button == GLUT_RIGHT_BUTTON) 
		{
			control = true;
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		else
		{
			control = false;
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		}
		m.x = x;
		m.y = y;
	}
	else m.left = m.right = m.middle = false;
}

void mouse_move(int x, int y)
{
	if (m.left)
	{
		player.yaw((double)(x - m.x) / 10);
		player.pitch((double)(y - m.y) / 10);
		m.x = x;
		m.y = y;
	}
}

void passive_mouse(int x, int y)
{
	if (control)
	{
		player.yaw((double)(x - m.x) / 10);
		player.pitch((double)(y - m.y) / 10);
		glutWarpPointer(100, 100);
		m.x = 100;
		m.y = 100;
	}
}

void moveCamera()
{
	if (keys[0]) player.press_A(frame_time);
	if (keys[1]) player.press_D(frame_time);
	if (keys[2]) player.press_S(frame_time);
	if (keys[3]) player.press_W(frame_time);
	if (keys[4]) player.press_E(frame_time);
	if (keys[5]) player.press_Q(frame_time);
}

int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize( 640, 640 );
	glutInitWindowPosition(100,150);
    glutCreateWindow( "Drawing a Cube" );

	std::cout << get_numpoints("sphere.txt");
	
	glewInit();

    generateGeometry( );
    initGPUBuffers( );			   
    shaderSetup( );                      

    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse_bt);
	glutMotionFunc(mouse_move);
	glutPassiveMotionFunc(passive_mouse);
	glutIdleFunc(moveCamera);

	glutMainLoop();
    return 0;
}
#include "Angel.h"
#include <chrono>
using namespace std::chrono;
class cl
{
public:
	steady_clock::time_point t;
	cl()
	{
		t = high_resolution_clock::now();
	}
	double gettime()
	{
		steady_clock::time_point t2 = t;
		t = high_resolution_clock::now();
		return ((double)duration_cast<microseconds>((t - t2)).count()) / 1000.0;
	}
	double check()
	{
		steady_clock::time_point t2 = high_resolution_clock::now();
		return ((double)duration_cast<microseconds>((t2 - t)).count()) / 1000.0;
	}
};

class camera
{
public:
	vec3 eye, up, at;
	double fovy, nearf, farf;
	mat4 viewMat, projMat;
	camera()
	{
		fovy = 60.0f;
		eye = vec3(0.0, 1.0, 2.0);
		up = vec3(0.0, 1.0, 0.0);
		at = vec3(0.0, 0.0, -1.0);
		nearf = 0.01;
		farf = 1000.0;
		viewMat = LookAt(eye, eye + at, up);
		projMat = Perspective(fovy, 1, nearf, farf);
	}
	camera(vec3 pos)
	{
		fovy = 60.0f;
		eye = pos;
		up = vec3(0.0, 1.0, 0.0);
		at = vec3(0.0, 0.0, -1.0);
		nearf = 0.1;
		farf = 1000.0;
		viewMat = LookAt(eye, eye + at, up);
		projMat = Perspective(fovy, 1, nearf, farf);
	}
	void setCamera_y(double x)
	{
		eye.y = x;
	}
	void update(int width, int height)
	{
		viewMat = LookAt(eye, eye + at, up);
		double aspect = (double)width / height;
		projMat = Perspective(fovy, aspect, nearf, farf);
		up = vec3(0.0, 1.0, 0.0);
	}
	void move_x(bool dir)
	{
		vec3 v = -normalize(vec3(cross(-at, up)));
		vec3 x_axis = vec3(v.x, v.y, v.z);
		if (dir)
			eye += x_axis * 0.001;
		else eye -= x_axis * 0.001;
	}
	void move_z(bool dir)
	{
		if (dir)
			eye += at * 0.001;
		else eye -= at * 0.001;
	}
	void move_y(bool dir)
	{
		if (dir)
			eye += up * 0.05;
		else eye -= up * 0.05;
	}
	void pitch(double angle)
	{
		double cs = cos(3.142 / 180 * (-angle));
		double sn = sin(3.142 / 180 * (-angle));
		vec3 t = -at;
		at = -vec3(cs * t.x - sn * up.x, cs * t.y - sn * up.y, cs * t.z - sn * up.z);
		up = vec3(sn * t.x + cs * up.x, sn * t.y + cs * up.y, sn * t.z + cs * up.z);
	}
	void roll(double angle)
	{
		double cs = cos(3.142 / 180 * angle);
		double sn = sin(3.142 / 180 * angle);
		vec4 v = -normalize(vec4(cross(-at, up), 0));
		vec3 x_axis = vec3(v.x, v.y, v.z);
		vec3 t = x_axis;
		x_axis = vec3(cs * t.x - sn * up.x, cs * t.y - sn * up.y, cs * t.z - sn * up.z);
		up = vec3(sn * t.x + cs * up.x, sn * t.y + cs * up.y, sn * t.z + cs * up.z);
	}
	void yaw(double angle)
	{
		vec4 temp = RotateY(-angle) * vec4(at, 1.0);
		at = vec3(temp.x, temp.y, temp.z);
	}
};

class cameraman : public camera
{
public:
	vec3 v;
	double a;
	cameraman()
	{
		v = vec3(0.0, 0.0, 0.0);
		a = 30.0f;
		camera();
	}
	cameraman(vec3 pos)
	{
		v = vec3(0.0, 0.0, 0.0);
		camera();
		a = 30.0f;
	}
	void update(double time_step, int width, int height)
	{
		eye += v * (time_step);
		viewMat = LookAt(eye, eye + at, up);
		double aspect = (double)width / height;
		projMat = Perspective(fovy, aspect, nearf, farf);
		up = vec3(0.0, 1.0, 0.0);
		v = v * (1.0 - 1.0 * time_step);
	}
	void press_W(double time_step)
	{
		vec3 dir = at;
		dir.y = 0.0f;
		dir = normalize(dir);
		v += dir * a * time_step;
	}
	void press_S(double time_step)
	{
		vec3 dir = at;
		dir.y = 0.0f;
		dir = normalize(dir);
		v -= dir * a * time_step;
	}
	void press_A(double time_step)
	{
		vec3 dir = -normalize(vec3(cross(-at, up)));
		dir.y = 0.0f;
		dir = normalize(dir);
		v -= dir * a * time_step;
	}
	void press_D(double time_step)
	{
		vec3 dir = -normalize(vec3(cross(-at, up)));
		dir.y = 0.0f;
		dir = normalize(dir);
		v += dir * a * time_step;
	}
	void press_E(double time_step)
	{
		v += up * a * time_step;
	}
	void press_Q(double time_step)
	{
		v -= up * a * time_step;
	}
};

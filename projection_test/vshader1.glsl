#version 400
in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
out vec4 color;
out vec3 normal;
out vec3 FragPos;
out vec4 FragPos_light;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float grass_bend;
uniform int is_grass;

uniform int shadow;
uniform mat4 LightMatrix;

void main()
{
	if(shadow == 1)
    {
        gl_Position = LightMatrix * modelMatrix * vPosition;
    }
    else
    {
		if(is_grass == 1)
        {
            vec4 grassPos = modelMatrix * vPosition;
            float bend = 0.6 * sin(grass_bend + grassPos.x / 2);
            grassPos = vec4(grassPos.x + (bend * grassPos.y), vPosition.y, grassPos.z, 1.0);
            gl_Position = projMatrix * viewMatrix * grassPos;
        }
        else gl_Position = projMatrix * viewMatrix * modelMatrix * vPosition;
		FragPos = vec3(modelMatrix * vPosition);
        FragPos_light = LightMatrix * vec4(FragPos, 1.0);
		normal = normalize(modelMatrix * vec4(vNormal, 0.0) ).xyz;
    }
}//
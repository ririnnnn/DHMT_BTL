#version 400
in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
out vec4 color;
out vec3 normal;
out vec3 FragPos;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float grass_bend;

void main()
{

        gl_Position = projMatrix * viewMatrix * modelMatrix * vPosition;
	    FragPos = vec3(modelMatrix * vPosition);
	    normal = normalize(modelMatrix * vec4(vNormal, 0.0) ).xyz;
}//
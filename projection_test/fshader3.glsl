#version 400
in vec3 normal;
in vec3 FragPos;
out vec4 fColor;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;
uniform vec3 EyePosition;
uniform int is_sun;

void main()
{
	if(is_sun == 1) fColor = vec4(1.0, 1.0, 0.8, 1.0);
	else if(is_sun == 2) fColor = vec4(0.68, 0.68, 0.68, 1.0);
	else
	{
		vec4 ambient = AmbientProduct;
		vec3 lightDir = normalize(LightPosition.xyz - FragPos);
		float Kd = max( dot(lightDir, normal), 0.0 );
		vec4 diffuse = Kd * DiffuseProduct;
		diffuse = Kd * DiffuseProduct;
		vec3 viewDir = normalize(EyePosition - FragPos);
		vec3 reflectDir = reflect(-lightDir, normal);
		float Ks = pow( max(dot(reflectDir, viewDir), 0.0), Shininess );
		vec4 specular = Ks * SpecularProduct;
		fColor = ambient + diffuse + specular;
	}
}//
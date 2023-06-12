#version 400
in vec3 normal;
in vec3 FragPos;
in vec4 FragPos_light;
out vec4 fColor;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;
uniform vec3 EyePosition;
uniform int is_sun;
uniform int shadow;

uniform sampler2D depthMap;

float shadow_cal()
{
	vec3 projCoords = FragPos_light.xyz / FragPos_light.w;
	projCoords = projCoords * 0.5 + 0.5;
	if(projCoords.x  > 1.0 || projCoords.x < 0.0 || projCoords.y  > 1.0 || projCoords.y < 0.0) return 0.0;
	float currentDepth = projCoords.z;
	float shadow_value = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	float bias = 0.0003;
	float pcfDepth = texture(depthMap, projCoords.xy).r; 
	shadow_value += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	shadow_value *= 3;
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow_value += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow_value /= 12.0;
	return shadow_value;
}

void main()
{
	if(shadow == 0)
	{
		if(is_sun == 1) fColor = vec4(1.0, 0.65, 0.2, 1.0);
		else if(is_sun == 2) fColor = vec4(0.68, 0.68, 0.68, 1.0);
		else if(is_sun == 3) fColor = vec4(0.8, 0.8, 0.8, 0.3);
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

			fColor = ambient + (1 - shadow_cal()) * (diffuse + specular);

			//float gamma = 1.5;
			//fColor.rgb = pow(fColor.rgb, vec3(1.0/gamma));
		}
	}
}//

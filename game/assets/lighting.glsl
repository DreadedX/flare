#pragma version 330 core

#pragma vertex
layout (location = 0) in vec3 position;

uniform mat4 model;

void main() {

	gl_Position = model * vec4(position, 1.0);
}

#pragma fragment
#pragma include Light
uniform Light light;
uniform vec3 viewPosition;

uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;
uniform sampler2D gTexCoordMap;

uniform sampler2D gDiffuseColorMap;
uniform sampler2D gSpecularColorMap;
uniform sampler2D ssaoBlurTexture;

out vec4 FragColor;

#pragma include CalcTexCoord

const float kPi = 3.14159265;

void main() {

	vec2 TexCoord = CalcTexCoord();

	vec4 color = vec4(texture(gColorMap, TexCoord).rgb, 1.0);

	// Ambient
	vec4 ambient = vec4(light.ambient, 1.0) * color;
	//vec4 ambient = vec4(1.0) * color;

	// Diffuse
	vec3 norm = normalize(texture(gNormalMap, TexCoord).rgb);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = vec4(texture(gDiffuseColorMap, TexCoord).rgb, 1.0) * diff * color;

	// Specular
	float shininess = texture(gSpecularColorMap, TexCoord).a;
	shininess = 32.0;
	float energyConservation = (8.0 + shininess) / (8.0 * kPi);
	vec3 viewDir = normalize(viewPosition - texture(gPositionMap, TexCoord).rgb);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = energyConservation * pow(max(dot(norm, halfwayDir), 0.0), shininess);
	vec4 specular = vec4(texture(gSpecularColorMap, TexCoord).rgb * spec * texture(gSpecularColorMap, TexCoord).rgb, 1.0);

	FragColor = vec4((ambient + diffuse + specular).rgb, 1.0) * texture(ssaoBlurTexture, TexCoord).r;

	float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}
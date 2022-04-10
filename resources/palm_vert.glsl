#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPos;
uniform vec3 lightPos2;
uniform vec3 lightPos3;
uniform vec3 lightPos4;

uniform vec3 offsets[56];

out vec3 fragNor;
out vec3 lightDir;
out vec3 lightDir2;
out vec3 lightDir3;
out vec3 lightDir4;
out vec3 EPos;

void main()
{
	vec4 palmPos = vertPos;
	palmPos.x += offsets[gl_InstanceID].x;
	palmPos.y += offsets[gl_InstanceID].y;
	palmPos.z += offsets[gl_InstanceID].z;

	gl_Position = P * V * M * palmPos;

	fragNor = (V*M * vec4(vertNor, 0.0)).xyz;
	lightDir = vec3(V*(vec4(lightPos - (M*vertPos).xyz, 0.0)));
	lightDir2 = vec3(V*(vec4(lightPos2 - (M*vertPos).xyz, 0.0)));
	lightDir3 = vec3(V*(vec4(lightPos3 - (M*vertPos).xyz, 0.0)));
	lightDir4 = vec3(V*(vec4(lightPos4 - (M*vertPos).xyz, 0.0)));
	EPos = vec3(1); //PULLED for release
}

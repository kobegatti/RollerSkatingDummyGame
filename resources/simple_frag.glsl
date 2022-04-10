#version 330 core 

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;
uniform vec3 discoColor;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
in vec3 lightDir2;
in vec3 lightDir3;
in vec3 lightDir4;
//position of the vertex in camera space
in vec3 EPos;

vec4 calcDirLight(vec3 fragNor, vec3 lightDir, vec3 EPos) 
{
	vec3 MattyDif = discoColor * vec3(0.5f);
	vec3 MattyAmb = MattyDif * vec3(0.2f);

	vec3 normal = normalize(fragNor); //frag normal
	vec3 light = normalize(lightDir);
	vec3 camera = normalize(EPos);
	vec3 H = normalize((light + camera) / 2);

	float dC = max(0, dot(normal, light));
	float dS = pow(max(0.0, dot(normal, H)), MatShine);

	return vec4(MatAmb + dC*MattyDif + dS*MatSpec, 1.0);
}

void main()
{
	vec4 result = calcDirLight(fragNor, lightDir, EPos);

	result += calcDirLight(fragNor, lightDir2, EPos);
	result += calcDirLight(fragNor, lightDir3, EPos);
	result += calcDirLight(fragNor, lightDir4, EPos);

	color = result;
}

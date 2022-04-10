#version 330 core

uniform sampler2D Texture0;
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;
uniform vec3 discoColor;

uniform int flip;

in vec2 vTexCoord;

out vec4 Outcolor;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
in vec3 lightDir2;
in vec3 lightDir3;
in vec3 lightDir4;
//position of the vertex in camera space
in vec3 EPos;

vec4 calcDirLight(vec3 fragNor, vec3 lightDir, vec3 EPos, vec4 texColor0) 
{
	vec3 MattyDif = discoColor * vec3(0.5f);
	vec3 MattyAmb = MattyDif * vec3(0.2f);

	vec3 normal = normalize(fragNor); //frag normal
	if (flip < 1)
  		normal *= -1.0f;
	vec3 light = normalize(lightDir);
	vec3 camera = normalize(EPos);
	vec3 H = normalize((light + camera) / 2);

	float dC = max(0, dot(normal, light));
	float dS = pow(max(0.0, dot(normal, H)), MatShine);

	return vec4(MattyAmb + dC*texColor0.xyz + dS*texColor0.xyz, 1.0);
}

void main() {
  vec4 texColor0 = texture(Texture0, vTexCoord);

  vec4 result = calcDirLight(fragNor, lightDir, EPos, texColor0);
  result += calcDirLight(fragNor, lightDir2, EPos, texColor0);
  result += calcDirLight(fragNor, lightDir3, EPos, texColor0);
  result += calcDirLight(fragNor, lightDir4, EPos, texColor0);

  Outcolor = result;

  //to confirm texture coordinates
  //Outcolor = vec4(vTexCoord.x, vTexCoord.y, 0, 0);
}


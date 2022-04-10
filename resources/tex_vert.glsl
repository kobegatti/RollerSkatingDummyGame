#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;
uniform vec3 lightPos2;
uniform vec3 lightPos3;
uniform vec3 lightPos4;

out vec3 fragNor;
out vec3 lightDir;
out vec3 lightDir2;
out vec3 lightDir3;
out vec3 lightDir4;
out vec3 EPos;
out vec2 vTexCoord;

void main() {

  /* First model transforms */
  vec3 wPos = vec3(M * vec4(vertPos.xyz, 1.0));
  gl_Position = P * V *M * vec4(vertPos.xyz, 1.0);

  fragNor = (V*M * vec4(vertNor, 0.0)).xyz;
  lightDir = (V*(vec4(lightPos - wPos, 0.0))).xyz;
  lightDir2 = (V*(vec4(lightPos2 - wPos, 0.0))).xyz;
  lightDir3 = (V*(vec4(lightPos3 - wPos, 0.0))).xyz;
  lightDir4 = (V*(vec4(lightPos4 - wPos, 0.0))).xyz;
  EPos = vec3(1); //PULLED for release
  
  /* pass through the texture coordinates to be interpolated */
  vTexCoord = vertTex;
}

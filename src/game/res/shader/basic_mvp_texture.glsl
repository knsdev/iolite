//----------------- Vertex Shader -----------------
#type vertex
#version 420

layout (std140) uniform UB_matrices
{
	mat4x4 u_mvp;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 _uv;

void main(void)
{
	gl_Position = u_mvp * vec4(position, 1.0f);
	_uv = uv;
}

//----------------- Fragment Shader ---------------
#type fragment
#version 420

uniform sampler2D texture0;

in vec2 _uv;

out vec4 fragmentColor;

void main(void)
{
	vec4 texColor = texture(texture0, _uv);
	fragmentColor = texColor;
}
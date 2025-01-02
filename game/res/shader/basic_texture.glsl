//----------------- Vertex Shader -----------------
#type vertex
#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 _uv;

void main(void)
{
	gl_Position = vec4(position, 1.0f);
	_uv = uv;
}

//----------------- Fragment Shader ---------------
#type fragment
#version 400

uniform sampler2D texture0;

in vec2 _uv;

out vec4 fragmentColor;

void main(void)
{
	vec4 texColor = texture(texture0, _uv);
	fragmentColor = texColor;
}
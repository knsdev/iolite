//----------------- Vertex Shader -----------------
#type vertex
#version 420

layout (std140) uniform UB_matrices
{
	mat4x4 u_mvp;
};

layout (location = 0) in vec3 position;

void main(void)
{
	gl_Position = u_mvp * vec4(position, 1.0f);
}

//----------------- Fragment Shader ---------------
#type fragment
#version 420

out vec4 fragmentColor;

void main(void)
{
	fragmentColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
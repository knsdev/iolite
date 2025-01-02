//----------------- Vertex Shader -----------------
#type vertex
#version 420

layout (std140) uniform UB_matrices
{
	mat4x4 u_mvp;
};

in vec3 position;

void main(void)
{
	gl_Position = u_mvp * vec4(position, 1.0f);
}

//----------------- Fragment Shader ---------------
#type fragment
#version 420

layout (std140) uniform UB_material
{
	vec4 color;
} material;

out vec4 fragmentColor;

void main(void)
{
	fragmentColor = material.color;
}
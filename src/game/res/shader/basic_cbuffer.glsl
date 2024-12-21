//----------------- Vertex Shader -----------------
#type vertex
#version 400

in vec3 position;

void main(void)
{
	gl_Position = vec4(position, 1.0f);
}

//----------------- Fragment Shader ---------------
#type fragment
#version 400

layout(std140) uniform UB_material
{
	vec4 u_color;
};

out vec4 fragmentColor;

void main(void)
{
	fragmentColor = u_color;
}
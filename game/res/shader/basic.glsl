//----------------- Vertex Shader -----------------
#type vertex
#version 400

layout (location = 0) in vec3 position;

void main(void)
{
	gl_Position = vec4(position, 1.0f);
}

//----------------- Fragment Shader ---------------
#type fragment
#version 400

out vec4 fragmentColor;

void main(void)
{
	fragmentColor = vec4(0.25f, 0.5f, 0.0f, 1.0f);
}
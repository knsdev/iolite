//----------------- Vertex Shader -----------------
#type vertex
#version 420

layout (std140) uniform UB_matrices
{
	mat4x4 u_model;
	mat4x4 u_view;
	mat4x4 u_projection;
};

in vec3 position;
in vec3 normal;

out vec3 _normal;
out vec3 _fragPos;

void main(void)
{
	mat3 normalMatrix = mat3(transpose(inverse(u_model)));

	gl_Position = u_projection * u_view * u_model * vec4(position, 1.0f);
	_normal = normalMatrix * normal;
	_fragPos = vec3(u_model * vec4(position, 1.0f));
}

//----------------- Fragment Shader ---------------
#type fragment
#version 420

layout (std140) uniform UB_material
{
	vec4 objectColor;
	vec4 lightColor;
	vec3 lightPos;
	float ambientStrength;
} material;

in vec3 _normal;
in vec3 _fragPos;

out vec4 fragmentColor;

void main(void)
{
	vec3 objColor = material.objectColor.xyz;
	vec3 ambient = material.ambientStrength * material.lightColor.xyz;
	
	vec3 norm = normalize(_normal);
	vec3 lightDir = normalize(material.lightPos - _fragPos);
	float d = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = d * material.lightColor.xyz;
	
	fragmentColor = vec4((ambient + diffuse) * objColor, 1.0f);
}

































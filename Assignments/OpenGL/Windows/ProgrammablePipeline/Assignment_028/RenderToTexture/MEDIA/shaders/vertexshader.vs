#version 450 core

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

uniform mat4 u_m_matrix;
uniform mat4 u_v_matrix;
uniform mat4 u_p_matrix;
uniform int u_lkeypressed;
uniform vec4 u_light_position;

out vec3 tNorm;
out vec2 out_texcoord;
out vec3 light_direction;
out vec3 viewer_vector;
out vec4 out_color;
void main(void)
{   
	if(u_lkeypressed == 1)   
	{       
		vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;       
		tNorm = mat3(u_v_matrix * u_m_matrix) * vNormal;       
		light_direction = vec3(u_light_position - eye_coordinates);       
		viewer_vector = -eye_coordinates.xyz;   
	}   
	
	gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;
	out_texcoord = vTexCoord;
}
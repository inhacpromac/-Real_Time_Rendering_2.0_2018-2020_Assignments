#version 450 core

in vec4 vPosition;
in vec2 vTexCoord;

uniform mat4 u_mvp_matrix;

out vec2 out_texcoord;

void main(void)
{
   gl_Position = u_mvp_matrix * vPosition;
   out_texcoord = vTexCoord;
}
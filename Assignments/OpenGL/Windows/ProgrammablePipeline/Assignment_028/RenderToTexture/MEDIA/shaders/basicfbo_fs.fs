#version 450 core

in vec2 out_texcoord;

uniform sampler2D u_sampler;

out vec4 FragColor;

void main(void)
{
	FragColor = texture(u_sampler, out_texcoord);
}
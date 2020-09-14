#version 450 core

in vec3 tNorm;
in vec3 light_direction;
in vec3 viewer_vector;
in vec2 out_texcoord;

uniform int u_lkeypressed;
uniform vec3 u_la;
uniform vec3 u_ld;
uniform vec3 u_ls;
uniform vec3 u_ka;
uniform vec3 u_kd;
uniform vec3 u_ks;
uniform float u_material_shininess;
uniform sampler2D u_sampler;

out vec4 FragColor;

void main(void)
{
   vec3 phong_ads_light;
   if(u_lkeypressed == 1)
   {
       vec3 normalized_tNorm = normalize(tNorm);
       vec3 normalized_light_direction = normalize(light_direction);
       vec3 normalized_viewer_vector = normalize(viewer_vector);
       float tn_dot_ld = max(dot(normalized_light_direction, normalized_tNorm), 0.0);
       vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tNorm);
       vec3 ambient = u_la * u_ka;
       vec3 diffuse = u_ld * u_kd * tn_dot_ld;
       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);
       phong_ads_light = ambient + diffuse + specular;
   }
   else
   {
       phong_ads_light = vec3(1.0, 1.0, 1.0);
   }
	FragColor = texture(u_sampler, out_texcoord) * vec4(phong_ads_light, 1.0);
}
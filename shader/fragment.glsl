// Version d'OpenGL
#version 150

in vec2 vert_texCoord;

in vec3 vert_normal;
in vec3 light_dir;

out vec4 frag_color;

uniform sampler2D texSampler;

// Fonction appellee pour chaque fragment
void main()
{
  vec3 normal = normalize(vert_normal);

  float t = dot(-light_dir, normal);
  t = max(t, 0.0);

  vec3 color = texture2D( texSampler, vert_texCoord).rgb;
  color = color * (t*0.9 + 0.1);
  
  frag_color = vec4(color, 1.0);
  
}

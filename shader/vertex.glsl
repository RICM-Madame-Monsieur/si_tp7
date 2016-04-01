// Version d'OpenGL
#version 150

// Donnees d'entree
in vec3 in_position;
in vec3 in_normal;
in vec2 in_texCoord;

// Donnees de sortie
out vec2 vert_texCoord;
out vec3 vert_normal;

out vec3 light_dir;

// Parametres
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

// Fonction appellee pour chaque sommet
void main()
{
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_position, 1.0);
  
  vert_normal = (transpose(inverse(ModelMatrix)) * vec4(in_normal, 0.0)).xyz;

  light_dir = vec3(0.0, 0.0, -1.0);
  light_dir = normalize(light_dir);
  light_dir = (inverse(ViewMatrix) * vec4(light_dir, 0.0)).xyz;
  
  vert_texCoord = in_texCoord;

}


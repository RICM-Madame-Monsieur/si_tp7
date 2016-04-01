#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp> //i32vec3
#include <vector>
#include <string>

using namespace glm;
using namespace std;

class Mesh {
 public:
  Mesh(){}
  Mesh(const char* filename);
  ~Mesh();

  i32vec3 get_face(unsigned int i);
  vec3 get_vertex(unsigned int i);
  vec3 get_normal(unsigned int i);
  vec3 get_color(unsigned int i);

  // length 
  unsigned int  nb_vertices;
  unsigned int  nb_faces;
  
  // data
  vector< vec3 > vertices;
  vector< vec3 > normals;
  vector< vec3 > colors;
  vector< vec2 > texCoord;
  vector< unsigned int > faces;
  
  vector< vec3 > computeBB() const ;
  void normalize();
  
  // info
  vec3 center;
  float radius;
};

#endif // MESH_H

#include <cstdlib>
#include <iostream>
#include <math.h>

#include <Mesh.h>

using namespace glm;
using namespace std;

i32vec3 Mesh::get_face(unsigned int i) {
	glm::i32vec3 face( faces[3*i], faces[3*i+1], faces[3*i+2] );
	return face;
}

vec3 Mesh::get_vertex(unsigned int i) {
	return vertices[i];
}

vec3 Mesh::get_normal(unsigned int i) {
	return normals[i];
}

vec3 Mesh::get_color(unsigned int i) {
	return colors[i];
}


Mesh::Mesh(const char* filename) 
{
	int j = 0;
	unsigned int tmp;
	vector<vec3> normalPerFace;
	float norm;
	vector<float> nv;
	float *n;
	FILE *file;
	int   error;
	float r;

	if((file=fopen(filename,"r"))==NULL) 
	{
		std::cout << "Unable to read : " << filename << std::endl;
	}

	// create mesh
	vertices = vector<vec3>();
	normals  = vector<vec3>();
	colors   = vector<vec3>();
	faces    = vector<unsigned int>();

	error = fscanf(file,"OFF\n%d %d %d\n",&(nb_vertices),&(nb_faces),&tmp);
	if(error==EOF) 
	{
		std::cout << "Unable to read : " << filename << std::endl;
	}

	vertices.resize(nb_vertices);
	normals.resize(nb_vertices);
	colors.resize(nb_vertices);
	faces.resize(nb_faces*3);

	// reading vertices
	for(int i=0;i<nb_vertices;++i) 
	{
		error = fscanf(file,"%f %f %f\n",&(vertices[i][0]),&(vertices[i][1]),&(vertices[i][2]));
		if(error==EOF) 
		{
			std::cout << "Unable to read vertices of : " << filename << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// reading faces
	j = 0;
	for(int i=0;i<nb_faces;++i) 
	{
		error = fscanf(file,"%d %d %d %d\n",&tmp,&(faces[j]),&(faces[j+1]),&(faces[j+2]));
//		  faces[j]   --;
//        faces[j+1] --;
//        faces[j+2] --;

		if(error==EOF) 
		{
			std::cout << "Unable to read faces of : " << filename << std::endl;
			exit(EXIT_FAILURE);
		}

		if(tmp!=3) 
		{
			printf("Error : face %d is not a triangle (%d polygonal face!)\n",i/3,tmp);
			exit(EXIT_FAILURE);
		}
		j += 3;
	}

	fclose(file);

	// computing center
	center[0] = center[1] = center[2] = 0.0f;
	for(int i=0 ;i<nb_vertices; i++) 
	{
		center += vertices[i];
	}
	center /= (float)nb_vertices;

	// computing radius
	radius = 0.0;
	glm::vec3 c;
	for(int i=0; i<nb_vertices; i++) {
		c = vertices[i] - center;
		r = glm::length(c);
		radius = r>radius ? r : radius;
	}

	// computing normals per faces
	normalPerFace.resize(nb_faces);
	for(int i=0; i<nb_faces; ++i) {
		glm::i32vec3 face_i = get_face(i);

		// the three vertices of the current face
		glm::vec3 v1 = get_vertex(face_i[0]);
		glm::vec3 v2 = get_vertex(face_i[1]);
		glm::vec3 v3 = get_vertex(face_i[2]);

		// the two vectors of the current face
		glm::vec3 v12;
		v12[0] = v2[0]-v1[0];
		v12[1] = v2[1]-v1[1];
		v12[2] = v2[2]-v1[2];

		glm::vec3 v13;
		v13[0] = v3[0]-v1[0];
		v13[1] = v3[1]-v1[1];
		v13[2] = v3[2]-v1[2];

		// cross product
		normalPerFace[i][0] = v12[1]*v13[2] - v12[2]*v13[1];
		normalPerFace[i][1] = v12[2]*v13[0] - v12[0]*v13[2];
		normalPerFace[i][2] = v12[0]*v13[1] - v12[1]*v13[0];

		// normalization
		norm = sqrt(normalPerFace[i][0]*normalPerFace[i][0] + 
				normalPerFace[i][1]*normalPerFace[i][1] +
				normalPerFace[i][2]*normalPerFace[i][2] );
		normalPerFace[i][0] /= norm;
		normalPerFace[i][1] /= norm;
		normalPerFace[i][2] /= norm;
	}

	// computing normals per vertex
	nv.resize(nb_vertices);
	for(int i=0; i<nb_vertices; ++i) 
	{
		// initialization
		normals[i][0] = 0.0;
		normals[i][1] = 0.0;
		normals[i][2] = 0.0;
		nv[i] = 0.0;
	}
	for(int i=0; i<nb_faces; ++i) 
	{
		// face normals average  
		glm::i32vec3 face_i = get_face(i);
		//n = &(nf[3*i]);

		normals[face_i[0]][0] += normalPerFace[i][0];
		normals[face_i[0]][1] += normalPerFace[i][1];
		normals[face_i[0]][2] += normalPerFace[i][2];
		nv[face_i[0]] ++;

		normals[face_i[1]][0] += normalPerFace[i][0];
		normals[face_i[1]][1] += normalPerFace[i][1];
		normals[face_i[1]][2] += normalPerFace[i][2];
		nv[face_i[1]] ++;

		normals[face_i[2]][0] += normalPerFace[i][0];
		normals[face_i[2]][1] += normalPerFace[i][1];
		normals[face_i[2]][2] += normalPerFace[i][2];
		nv[face_i[2]] ++;
	}
	for(int i=0; i<nb_vertices; ++i) 
	{
		// normalization
		normals[i][0] /= nv[i];
		normals[i][1] /= nv[i];
		normals[i][2] /= nv[i];

		normals[i] = glm::normalize(normals[i]);

		//normals[i][0] /= -nv[i];
		//normals[i][1] /= -nv[i];
		//normals[i][2] /= -nv[i];
	}

	/*

	// computing colors as normals 
	for(i=0;i<3*nb_vertices;++i) {
	colors[i] = (normals[i]+1.0)/2.0;
	}
	 */
}

Mesh::~Mesh() {}


vector< vec3 > Mesh::computeBB() const 
{
    vector< vec3 > output;
    
    output.push_back(vertices[0]);
    output.push_back(vertices[0]);
    
	for(int i=1; i<vertices.size(); ++i) 
	{
	    vec3 v = vertices[i];
	    
	    for(int j = 0; j < 3; j++)
	    {
	        output[0][j] = std::min(output[0][j], v[j]);
	        output[1][j] = std::max(output[1][j], v[j]);
	    }
	}
	
	return output;
}

void Mesh::normalize()
{
//    vector<vec3> bb = Mesh::computeBB();
    
	for(int i=0; i<vertices.size(); ++i) 
	{
	    for(int j = 0; j < 3; j++)
	    {
//	        vertices[i][j] = (vertices[i][j] - bb[0][j]) / (bb[1][j] - bb[0][j]) - 0.5;
	        vertices[i][j] = (vertices[i][j] - center[j]) / radius * 1.333;
	    }
	}
}






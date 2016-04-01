#include <iostream>
#include <vector>
#include <string>

#include <shader.h> // Help to load shaders from files

// Include GLEW : Always include it before glfw.h et gl.h :)
#include <GL/glew.h>    // OpenGL Extension Wrangler Library : http://glew.sourceforge.net/ 
#include <GL/glfw.h>    // Window, keyboard, mouse : http://www.glfw.org/

#include <glm/glm.hpp>  // OpenGL Mathematics : http://glm.g-truc.net/0.9.5/index.html
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <GLFW_define.h>
#include <Mesh.h>


#include <QGLWidget>



// Dimensions de la fenêtre :
#define WIDTH 1000.0f
#define HEIGHT 800.0f

using namespace glm;
using namespace std;


void view_control(mat4& view_matrix, float dx);
void create_cube(Mesh* output);
void create_sphere(Mesh* output);
void tex_parameter_control();

int main()
{

    cout << "Debut du programme..." << endl;

    //==================================================
    //============= Creation de la fenetre =============
    //==================================================


    // Initialisation de GLFW
	if( !glfwInit() )
	{
		cout << "Echec de l'initialisation de GLFW" << endl;
		exit(EXIT_FAILURE);
	}

    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // Anti Aliasing
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3); // OpenGL 2.1
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

    // Ouverture d'une fenêtre en 1024x768
    // et creation d'un contexte OpenGL
    if( !glfwOpenWindow(WIDTH, HEIGHT, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		cout << "Echec de l'ouverture de fenetre OpenGL" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
    }

    // Definition du titre de la fenêtre
	glfwSetWindowTitle( "Polytech RICM 4 - TP7" );

    // Autorise GLFW a recevoir les appuis de touche
	glfwEnable( GLFW_STICKY_KEYS );

    // Initialisation de  GLEW
	if (glewInit() != GLEW_OK) {
		cout << "Echec de l'initialisation de GLEW" << endl;
		exit(EXIT_FAILURE);
	}
    
    // Verification des donnees du contexte OpenGL
    const GLubyte* renderer = glGetString (GL_RENDERER);
    cout << "Carte Graphique : " << renderer << endl;

    const GLubyte* version = glGetString (GL_VERSION);
    cout << "Driver OpenGL : " << version << endl;


    //==================================================
    //================= Initialisation =================
    //==================================================

    cout << "Initialisations..." << endl;


    // Definition de la couleur du fond
    glClearColor(0.1, 0.1, 0.1, 1.0);



//-------------------------------------------------
// Initialisation du shader programm

    // Compilation du shader programm
	GLuint programID = LoadShaders( "../shader/vertex.glsl",      "../shader/fragment.glsl" );
    cout << "programID = " << programID << endl;

	// Obtention de l'ID de l'attribut "vertex_position" dans programID
	GLuint positionID = glGetAttribLocation(programID, "in_position");
    cout << "positionID = " << positionID << endl;

	// Obtention de l'ID de l'attribut "in_color" dans programID
    GLuint normalID = glGetAttribLocation(programID, "in_normal");
    cout << "normalID = " << normalID << endl;

	// Obtention de l'ID de l'attribut "in_color" dans programID
    GLuint textID = glGetAttribLocation(programID, "in_texCoord");
    cout << "textID = " << textID << endl;





//-------------------------------------------------
// Initialisation des arrays de données

    Mesh m;
    create_cube(&m);

    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    cout << "vertexBufferID = " << vertexBufferID << endl;
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m.vertices.size(), m.vertices.data(), GL_STATIC_DRAW);
	
    
    GLuint normalBufferID;
    glGenBuffers(1, &normalBufferID);
    cout << "normalBufferID = " << normalBufferID << endl;
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m.normals.size(), m.normals.data(), GL_STATIC_DRAW);
	
    
    GLuint texCoordBufferID;
    glGenBuffers(1, &texCoordBufferID);
    cout << "texCoordBufferID = " << texCoordBufferID << endl;
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * m.texCoord.size(), m.texCoord.data(), GL_STATIC_DRAW);
    

    GLuint elementBufferID;
    glGenBuffers(1, &elementBufferID);
    cout << "elementBufferID = " << elementBufferID << endl;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m.faces.size(), m.faces.data(), GL_STATIC_DRAW);

    cout <<  m.vertices.size() << " " <<  m.normals.size() << " " <<  m.texCoord.size() << " " <<  m.faces.size() << endl;





//-------------------------------------------------
// Initialisation des matrices MVP


    // Definition des matrices de transformation
    mat4 projection_matrix = perspective(45.0f, WIDTH / HEIGHT, 0.1f, 100.0f);
    mat4 view_matrix = lookAt(vec3(1.0, 2.0, 1.0), vec3(0.0), vec3(0.0, 0.0, 1.0));
    mat4 model_matrix = scale(vec3(1.0f) * 0.5f);

    GLuint PmatrixID = glGetUniformLocation(programID, "ProjectionMatrix");
    cout << "PmatrixID = " << PmatrixID << endl;

    GLuint VmatrixID = glGetUniformLocation(programID, "ViewMatrix");
    cout << "VmatrixID = " << VmatrixID << endl;

    GLuint MmatrixID = glGetUniformLocation(programID, "ModelMatrix");
    cout << "MmatrixID = " << MmatrixID << endl;
    
    
    
    
    
    
    
    
//-------------------------------------------------
// Initialisation de la texture

    
    // Changement de l'image
	QImage img = QGLWidget::convertToGLFormat(QImage("../textures/crate.jpg"));
	if(img.isNull())
    {
        std::cerr << "Echec de chargement de la texture !" << std::endl;
        exit(EXIT_FAILURE);
    }

	// Creation d'une texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// Definition de textureID comme la texture courante
	glBindTexture(GL_TEXTURE_2D, textureID);

    // Transmission de l'image a la carte graphique
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*)img.bits());

    glGenerateMipmap(GL_TEXTURE_2D);

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
// ICI : affectez une valeur aux parametres de texture
//
// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::




		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);





	// Liberation de la texture courante
	glBindTexture(GL_TEXTURE_2D, 0);
	
	
	// Recuperation de l'identifiant de la variable texSampler de fragment.glsl (representant la texture)
	GLuint texSamplerID = glGetUniformLocation( programID, "texSampler" );
	



    cout << "Debut de la boucle principale..." << endl;

    double init_time = glfwGetTime();
    double prec_time = init_time;
    double cur_time = init_time;
    double speed = 2.0;

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);

    // Boucle de dessin
	do{
        // Nettoyage de la zone de dessin
		glClear( GL_COLOR_BUFFER_BIT );
		glClear( GL_DEPTH_BUFFER_BIT );


        //==================================================
        //===================== Calcul =====================
        //==================================================

        prec_time = cur_time;
        cur_time = glfwGetTime() - init_time;
        float delta_time = cur_time - prec_time;

        // Creation du titre de la fenetre pour
        glfwSetWindowTitle( (string("Polytech RICM 4 - TP7 - ") + detail::format("%2.0f", 1.0/ delta_time) + string(" FPS")).c_str() );

        // Controle de la vue
        view_control(view_matrix, speed * delta_time);
        

        //==================================================
        //===================== Dessin =====================
        //==================================================

		// Definition de programID comme le shader courant
		glUseProgram(programID);

        // Transmission des matrices au vertex shader
        glUniformMatrix4fv(PmatrixID, 1, GL_FALSE, value_ptr(projection_matrix));
        glUniformMatrix4fv(VmatrixID, 1, GL_FALSE, value_ptr(view_matrix));
        glUniformMatrix4fv(MmatrixID, 1, GL_FALSE, value_ptr(model_matrix));



        // Activation de l'attribut positionID
		glEnableVertexAttribArray(positionID);

        // Definition de vertexBufferID comme le buffer courant
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

        // On indique comment lire les donnees
		glVertexAttribPointer(
            positionID,   // ID de l'attribut à configurer
            3,                  // nombre de composante par position (x, y, z)
            GL_FLOAT,           // type des composantes
            GL_FALSE,           // normalisation des composantes
            0,                  // decalage des composantes
            (void*)0            // offset des composantes
		);



        // Activation de l'attribut colorPositionID
        glEnableVertexAttribArray(normalID);

        // Definition de colorBufferID comme le buffer courant
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);

        // On indique comment lire les donnees
        glVertexAttribPointer(
            normalID,
            3,
            GL_FLOAT,
            GL_TRUE,
            0,
            (void*)0
        );



        // Activation de l'attribut colorPositionID
        glEnableVertexAttribArray(textID);

        // Definition de colorBufferID comme le buffer courant
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferID);

        // On indique comment lire les donnees
        glVertexAttribPointer(
            textID,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );



        // Index buffer
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
         
         
         
	    // Activation de la texture 0
	    glActiveTexture(GL_TEXTURE0);

        // Definition de textureID comme la texture courante
	    glBindTexture( GL_TEXTURE_2D, textureID );

	    // liaison entre le shader et la texture
	    glUniform1i(texSamplerID, 0);
	
	
        tex_parameter_control();
        


         // Dessin
         glDrawElements(
             GL_TRIANGLES,      // mode
             m.faces.size(),      // nombre
             GL_UNSIGNED_INT,   // type
             (void*)0           // offset
         );



        // Desactivation des attributs
		glDisableVertexAttribArray(positionID);
		glDisableVertexAttribArray(normalID);
		glDisableVertexAttribArray(textID);

	    glBindTexture( GL_TEXTURE_2D, 0 );



		// Echange des zones de dessin buffers
		glfwSwapBuffers();

        cout << "Temps ecoule : " << cur_time << "\ts\r";
        cout.flush();

    } // Execution de la boucle...
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&  // ... jusqu'a appui sur la touche ESC
           glfwGetWindowParam( GLFW_OPENED )        );  // ... ou fermeture de la fenetre

    cout << endl;

    // Ferme GLFW et OpenGL
	glfwTerminate();



    //==================================================
    //============== Nettoyage la memoire ==============
    //==================================================

    // Liberation des buffers
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &texCoordBufferID);
    glDeleteBuffers(1, &elementBufferID);

    // Liberation des textures
	glDeleteTextures(1, &textureID);


    cout << "Fin du programme..." << endl;


	return EXIT_SUCCESS;
}



void create_cube(Mesh* output)
{

    output->vertices.push_back(vec3(-1, -1, -1));
    output->vertices.push_back(vec3( 1, -1, -1));
    output->vertices.push_back(vec3( 1,  1, -1));
    output->vertices.push_back(vec3(-1,  1, -1));
    
    output->vertices.push_back(vec3(-1, -1, -1));
    output->vertices.push_back(vec3(-1,  1, -1));
    output->vertices.push_back(vec3(-1,  1,  1));
    output->vertices.push_back(vec3(-1, -1,  1));
    
    output->vertices.push_back(vec3(-1, -1, -1));
    output->vertices.push_back(vec3(-1, -1,  1));
    output->vertices.push_back(vec3( 1, -1,  1));
    output->vertices.push_back(vec3( 1, -1, -1));
    
    output->vertices.push_back(vec3( 1,  1,  1));
    output->vertices.push_back(vec3(-1,  1,  1));
    output->vertices.push_back(vec3(-1, -1,  1));
    output->vertices.push_back(vec3( 1, -1,  1));
    
    output->vertices.push_back(vec3( 1,  1,  1));
    output->vertices.push_back(vec3( 1, -1,  1));
    output->vertices.push_back(vec3( 1, -1, -1));
    output->vertices.push_back(vec3( 1,  1, -1));
    
    output->vertices.push_back(vec3( 1,  1,  1));
    output->vertices.push_back(vec3( 1,  1, -1));
    output->vertices.push_back(vec3(-1,  1, -1));
    output->vertices.push_back(vec3(-1,  1,  1));
                        
    output->normals.push_back(vec3(0, 0, -1));
    output->normals.push_back(vec3(0, 0, -1));
    output->normals.push_back(vec3(0, 0, -1));
    output->normals.push_back(vec3(0, 0, -1));
    
    output->normals.push_back(vec3(-1, 0, 0));
    output->normals.push_back(vec3(-1, 0, 0));
    output->normals.push_back(vec3(-1, 0, 0));
    output->normals.push_back(vec3(-1, 0, 0));
    
    output->normals.push_back(vec3(0, -1, 0));
    output->normals.push_back(vec3(0, -1, 0));
    output->normals.push_back(vec3(0, -1, 0));
    output->normals.push_back(vec3(0, -1, 0));
    
    output->normals.push_back(vec3(0, 0,  1));
    output->normals.push_back(vec3(0, 0,  1));
    output->normals.push_back(vec3(0, 0,  1));
    output->normals.push_back(vec3(0, 0,  1));
    
    output->normals.push_back(vec3( 1, 0, 0));
    output->normals.push_back(vec3( 1, 0, 0));
    output->normals.push_back(vec3( 1, 0, 0));
    output->normals.push_back(vec3( 1, 0, 0));
    
    output->normals.push_back(vec3(0,  1, 0));
    output->normals.push_back(vec3(0,  1, 0));
    output->normals.push_back(vec3(0,  1, 0));
    output->normals.push_back(vec3(0,  1, 0));
    
    
    output->texCoord.push_back(vec2(-1, -1));
    output->texCoord.push_back(vec2(2, -1));
    output->texCoord.push_back(vec2(2, 3));
    output->texCoord.push_back(vec2(-1, 3));
    
    output->texCoord.push_back(vec2(-1, -1));
    output->texCoord.push_back(vec2(2, -1));
    output->texCoord.push_back(vec2(2, 3));
    output->texCoord.push_back(vec2(-1, 3));
    
    output->texCoord.push_back(vec2(-1, -1));
    output->texCoord.push_back(vec2(2, -1));
    output->texCoord.push_back(vec2(2, 3));
    output->texCoord.push_back(vec2(-1, 3));
    
    output->texCoord.push_back(vec2(-1, -1));
    output->texCoord.push_back(vec2(2, -1));
    output->texCoord.push_back(vec2(2, 3));
    output->texCoord.push_back(vec2(-1, 3));
    
    output->texCoord.push_back(vec2(-1, -1));
    output->texCoord.push_back(vec2(2, -1));
    output->texCoord.push_back(vec2(2, 3));
    output->texCoord.push_back(vec2(-1, 3));
    
    output->texCoord.push_back(vec2(-1, -1));
    output->texCoord.push_back(vec2(2, -1));
    output->texCoord.push_back(vec2(2, 3));
    output->texCoord.push_back(vec2(-1, 3));

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
// ICI : modifiez les coordonnées de textures
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::







    output->faces.push_back(0);
    output->faces.push_back(2);
    output->faces.push_back(1);

    output->faces.push_back(0);
    output->faces.push_back(3);
    output->faces.push_back(2);


    output->faces.push_back(4);
    output->faces.push_back(6);
    output->faces.push_back(5);

    output->faces.push_back(4);
    output->faces.push_back(7);
    output->faces.push_back(6);


    output->faces.push_back(8);
    output->faces.push_back(10);
    output->faces.push_back(9);

    output->faces.push_back(8);
    output->faces.push_back(11);
    output->faces.push_back(10);


    output->faces.push_back(12);
    output->faces.push_back(13);
    output->faces.push_back(14);

    output->faces.push_back(12);
    output->faces.push_back(14);
    output->faces.push_back(15);


    output->faces.push_back(16);
    output->faces.push_back(17);
    output->faces.push_back(18);

    output->faces.push_back(16);
    output->faces.push_back(18);
    output->faces.push_back(19);


    output->faces.push_back(20);
    output->faces.push_back(21);
    output->faces.push_back(22);

    output->faces.push_back(20);
    output->faces.push_back(22);
    output->faces.push_back(23);                            
}




void create_sphere(Mesh* output)
{

    int N = 100;
    int Nu = 2 * N;
    int Nv = N;

    for(int i = 0; i < Nu; i++)
    {
        float u = float(i) / (Nu-1);
        float phi = u * M_PI * 2;
        
        for(int j = 0; j < Nv; j++)
        {
            float v = float(j) / (Nv-1);
            float psi = v * M_PI;
            
            vec3 p(cos(phi)*sin(psi), sin(phi)*sin(psi), cos(psi));
            output->vertices.push_back(p);
            
            output->normals.push_back(p);
            
            vec2 t(u, 1.0 - v);
            output->texCoord.push_back(t);
        }
    }
    
    
    vector<unsigned int> faces;
    for(int i = 0; i < Nu; i++)
    {
        for(int j = 0; j < Nv - 1; j++)
        {
            output->faces.push_back( i * Nv + j);
            output->faces.push_back((i+1)%Nu * Nv + j);
            output->faces.push_back( i * Nv + j+1);
            
            output->faces.push_back( i * Nv + j+1);
            output->faces.push_back((i+1)%Nu * Nv + j);
            output->faces.push_back((i+1)%Nu * Nv + j + 1);
        }
    }
}



void view_control(mat4& view_matrix, float dx)
{
    if (glfwGetKey( GLFW_KEY_LSHIFT ) == GLFW_PRESS)
    {
        dx /= 10.0;
    }

    if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS)
    {
        vec4 axis = vec4(1.0, 0.0, 0.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, dx * 180.0f, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS)
    {
        vec4 axis = vec4(1.0, 0.0, 0.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, -dx * 180.0f, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 1.0, 0.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, dx * 180.0f, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 1.0, 0.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, -dx * 180.0f, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_PAGEUP ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 0.0, 1.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, dx * 180.0f, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_PAGEDOWN ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 0.0, 1.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, -dx * 180.0f, vec3(axis));
    }

    if (glfwGetKey( GLFW_KEY_Z ) == GLFW_PRESS)
    {
        vec3 pos = vec3(view_matrix * vec4(0,0,0,1));
        vec4 axis = vec4(0.0, 0.0, 1.0, 0.0) * dx * length(pos) * 0.5;
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_S ) == GLFW_PRESS)
    {
        vec3 pos = vec3(view_matrix * vec4(0,0,0,1));
        vec4 axis = vec4(0.0, 0.0, 1.0, 0.0) * (-dx) * length(pos) * 0.5;
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_Q) == GLFW_PRESS)
    {
        vec4 axis = vec4(-1.0, 0.0, 0.0, 0.0) * dx;
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_D ) == GLFW_PRESS)
    {
        vec4 axis = vec4(-1.0, 0.0, 0.0, 0.0) * (-dx);
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_A ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 1.0, 0.0, 0.0) * dx;
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_E ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 1.0, 0.0, 0.0) * (-dx);
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
}

void tex_parameter_control(){
    if (glfwGetKey( GLFW_KEY_F1 ) == GLFW_PRESS)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    if (glfwGetKey( GLFW_KEY_F2 ) == GLFW_PRESS)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   
    }
    if (glfwGetKey( GLFW_KEY_F3 ) == GLFW_PRESS)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    }
    if (glfwGetKey( GLFW_KEY_F4 ) == GLFW_PRESS)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    }
    if (glfwGetKey( GLFW_KEY_F5 ) == GLFW_PRESS)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    }
    if (glfwGetKey( GLFW_KEY_F6 ) == GLFW_PRESS)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
}

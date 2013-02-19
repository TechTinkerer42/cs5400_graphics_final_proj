#include "Angel.h"
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <memory>

#ifndef CS5400_FILE_PATH
    #ifdef __APPLE__  
        #define CS5400_FILE_PATH std::string("../../cs5400/")
    #else
        #define CS5400_FILE_PATH std::string("")
    #endif  // __APPLE__
#endif  // CS5400_FILE_PATH

#include "Shader.hpp"
#include "Program.hpp"

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))


GLfloat  fov = 60, aspect = 1;
GLfloat  zNear = 0.1, zFar = 13.0;

GLuint level = 0;

std::vector<std::vector<GLfloat>> modelPoints;
std::vector<std::vector<GLfloat>> modelNormals;
std::vector<std::vector<GLuint>> modelFaces;
std::vector<GLuint> plyEle;
std::vector<GLuint> plyPoints;

GLuint totalModels = 0;

struct point
{
	GLfloat x;
	GLfloat y;
	GLfloat z;

	point operator - (const point & rhs)
	{
		point temp;


		temp.x = x - rhs.x;
		temp.y = y - rhs.y;
		temp.z = z - rhs.z;
		return temp;
	}

	point operator = (const point & rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;

		return *this;
	}

	point operator / (const GLfloat &div)
	{
		point temp;

		temp.x = x /div;
		temp.y = y /div;
		temp.z = z /div;

		return temp;
	}

};

void readFile(const std::string &);
void display();
void init_resources();
void free_resources();
GLuint vPosition;
GLuint vNormal;
GLuint ply_ele;
GLuint ply_points;
std::shared_ptr<cs5400::Program> program;


enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
vec4  Trans = vec4( 0.0, 0.0, 0.5, 1.0 );
GLfloat  ScaleVal = 1.0;

GLuint  theta; 
GLuint  trans;
GLuint  ModelView, Projection;

GLfloat xCol = 0;
GLfloat yCol = 0;
GLfloat zCol = 0;

point calcNormal(int p1, int p2, int p3, std::vector<point> &pointsObj, std::vector<point> &normalObj)
{



	point U = pointsObj[p2] - pointsObj[p1];
	point V = pointsObj[p3] - pointsObj[p1];

	point normal;

	//(U X V) cross product
	normal.x = U.y * V.z - U.z * V.y;
	normal.y = U.z * V.x - U.x * V.z;
	normal.z = U.x * V.y - U.y * V.x;

	//length
	GLfloat unitVec = normal.x * normal.x + normal.y * normal.y + normal.z * normal.z;

	//Unit vector
	normal = normal / std::sqrt(unitVec);

	normalObj[p1] = normal;
	normalObj[p2] = normal;
	normalObj[p3] = normal;

	return normal;
}


void keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033: // Escape Key
//		glutExit();
	case 'w': case 'W':
		Theta[0] += 5.0;
		if ( Theta[0] > 360 ) {
		Theta[0] -= 0.5;
		}
		break;
	case 'a': case 'A':
		Theta[1] += 5.0;
		if ( Theta[1] > 360 ) {
		Theta[1] -= 0.5;
		}
		break;
	case 's': case 'S':
		Theta[0] -= 5.0;
		if ( Theta[0] < -360 ) {
		Theta[0] -= 0.5;
		}
		break;
	case 'd': case 'D':
		Theta[1] -= 5.0;
		if ( Theta[1] < -360 ) {
		Theta[1] -= 0.5;
		}
		break;
	case 'q': case 'Q':
		Theta[2] -= 5.0;
		if ( Theta[2] < -360 ) {
		Theta[2] -= 0.5;
		}
		break;
	case 'r': case 'R':
		Theta[2] += 5.0;
		if ( Theta[2] < 360 ) {
		Theta[2] -= 0.5;
		}
		break;
	case 'j': case 'J':
		Trans[0] += 0.1;
		break;
	case 'l': case 'L':
		Trans[0] -= 0.1;
		break;
	case 'i' : case 'I':
		Trans[1] -= 0.1;
		break;
	case 'k': case 'K':
		Trans[1] += 0.1;
		break;
	case 'z' : case 'Z':
		Trans[2] -= 0.1;
		break;
	case 'x' : case 'X':
		Trans[2] += 0.1;
		break;
	case 'n': case 'N':
		ScaleVal += 0.1;
		break;
	case 'm': case 'M':
		ScaleVal -= 0.1;
		break;
	/*case 'f':
		xCol += .1;
		break;
	case 'F':
		xCol -= .1;
		break;*/
	case 'g':
		yCol += .1;
		break;
	case 'G':
		yCol -= .1;
		break;
	case 'h':
		zCol += .1;
		break;
	case 'H':
		zCol -= .1;
		break;
	case '.':
		if(level < totalModels - 1)
			level++;
		break;
	case ',':
		if(level > 0)
		level--;
		break;
		
	}
	 glutPostRedisplay();
}

GLboolean isNeg = false;

void idle()
{
	if(!isNeg)
	{
		if(xCol > 6.0)
		{
			isNeg = true;
		}
		else
			xCol += .1;
	}
	if(isNeg)
	{
		if(xCol > -6.0)
			xCol -= .1;
		else
			isNeg = false;
	}

	glutPostRedisplay();
}


void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    GLfloat aspect = GLfloat(width)/height;
    mat4  projection = Perspective( 45.0, aspect, 0.5, 0.6 );

    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}


int main(int argv, char **argc)
{
	std::vector<std::string> fileNames;
	fileNames.push_back(CS5400_FILE_PATH + "bun_zipper.ply");
	fileNames.push_back(CS5400_FILE_PATH + "dragon_vrip.ply");
	fileNames.push_back(CS5400_FILE_PATH + "happy_vrip.ply");

	for(int i = 0; i < fileNames.size(); i++)
	{
		std::cout << "Loading file " << i + 1 << " " << fileNames[i] << std::endl;
		readFile(fileNames[i]);
		totalModels++;
	}
	
	

	glutInit(&argv, argc);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Ply loading program");

	GLenum glew_status = glewInit();

	if(glew_status != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
	    return EXIT_FAILURE;
	}

	 try
	{
    program = cs5400::make_program
      (
	cs5400::make_vertexShader(CS5400_FILE_PATH + "vshader36.glsl")
      ,cs5400::make_fragmentShader(CS5400_FILE_PATH + "fshader36.glsl")   
      );
	init_resources();
    glutDisplayFunc(display);
	glutKeyboardFunc( keyboard );
	//glutReshapeFunc( reshape );
	glutIdleFunc( idle );
    glutMainLoop();
	}
	catch(std::exception& e)
	{
	   std::cerr << e.what();
	}

	free_resources();
	return EXIT_SUCCESS;

}


void init_resources()
{

	for(int i = 0; i < totalModels; i++)
	{
		glGenBuffers(1, &ply_points);
		glBindBuffer(GL_ARRAY_BUFFER, ply_points);
		//glBufferData(GL_ARRAY_BUFFER, modelPoints[i].size() * sizeof(GLfloat), modelPoints[i].data(), GL_STATIC_DRAW);

		glBufferData( GL_ARRAY_BUFFER, (modelPoints[i].size() + modelNormals[i].size()) * sizeof(GLfloat), NULL, GL_STATIC_DRAW );

		glBufferSubData( GL_ARRAY_BUFFER, 0, modelPoints[i].size() * sizeof(GLfloat), modelPoints[i].data() );

		glBufferSubData( GL_ARRAY_BUFFER, modelPoints[i].size() * sizeof(GLfloat), modelNormals[i].size() * sizeof(GLfloat), modelNormals[i].data());

		plyPoints.push_back(ply_points);

		glGenBuffers(1, &ply_ele);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ply_ele);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelFaces[i].size()*sizeof(GLuint), modelFaces[i].data(), GL_STATIC_DRAW);

		plyEle.push_back(ply_ele);
	}

		 
    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program->getHandle(), "ModelView" );
    Projection = glGetUniformLocation( program->getHandle(), "Projection" );

	
	//glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	
}

void lightingParams()
{
	vec4 light_position( 0.0 - xCol, 0.0 - yCol, -1.0 - zCol, 0.0 );
    vec4 light_ambient( 0.8, 1.0, 0.5, 1.0 );
    vec4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    vec4 light_specular( 0.8, 0.2, 0.0, 1.0 );

    vec4 material_ambient( 1.0, 0.3, 1.0, 1.0 );
    vec4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    vec4 material_specular( 0.4, 0.8, 0.0, 1.0 );
    float  material_shininess = 5.0;

    vec4 ambient_product = light_ambient * material_ambient;
    vec4 diffuse_product = light_diffuse * material_diffuse;
    vec4 specular_product = light_specular * material_specular;

	glUniform4fv( glGetUniformLocation(program->getHandle(), "AmbientProduct"),
		  1, ambient_product );
    glUniform4fv( glGetUniformLocation(program->getHandle(), "DiffuseProduct"),
		  1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program->getHandle(), "SpecularProduct"),
		  1, specular_product );
	
    glUniform4fv( glGetUniformLocation(program->getHandle(), "LightPosition"),
		  1, light_position );

    glUniform1f( glGetUniformLocation(program->getHandle(), "Shininess"),
		 material_shininess );
}

void display()
{

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	

	lightingParams();

	
    glUseProgram(program->getHandle());

	vec4  at(Trans[0], Trans[1], Trans[2]-1, 1.0 );
    vec4    up( 0.0, 1.0, 0.0, 0.0 );

	//const vec3 viewer_pos( Trans[0], Trans[1], Trans[2] );

    mat4  model_view = ( LookAt( Trans, at, up ) *
						 RotateX( Theta[Xaxis] ) *
						 RotateY( Theta[Yaxis] ) *
						 RotateZ( Theta[Zaxis] ) *
						 Scale(ScaleVal, ScaleVal, ScaleVal));
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );

	mat4  p = Perspective(fov, aspect, zNear, zFar);
    glUniformMatrix4fv( Projection, 1, GL_TRUE, p );


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plyEle[level]);
	glBindBuffer(GL_ARRAY_BUFFER, plyPoints[level]);

	vPosition = glGetAttribLocation( program->getHandle(), "vPosition" );
	vNormal = glGetAttribLocation(program->getHandle(), "vNormal");
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(vNormal);

	glVertexAttribPointer(
		vPosition,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		BUFFER_OFFSET(0)
		);

	glVertexAttribPointer(
		vNormal,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		BUFFER_OFFSET(modelPoints[level].size() * sizeof(GLfloat))
		);

	glDrawElements(GL_TRIANGLES, (GLsizei)modelFaces[level].size(), GL_UNSIGNED_INT,0);

	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vNormal);


	
	glutSwapBuffers();

}

void free_resources()
{
	glDeleteProgram(program->getHandle());
}


void readFile(const std::string &fName)
{
	std::vector<GLuint> faces;
	std::vector<GLfloat> points;
	std::vector<GLfloat> normals;

	std::vector<point> pointsObj;
	std::vector<point> normalObj;

	int numVertices;
	int numTriangles;
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLuint p1;
	GLuint p2;
	GLuint p3;
	GLboolean hasConfidence = false;
	GLboolean hasIntensity = false;

	GLfloat confidence;
	GLfloat intensity;
	std::string line;
	std::string chunk;

	std::ifstream file(fName, std::ios::in);
	std::getline(file, line);

	if(!file)
	{
		std::cout << "Could not open file " << fName << " -- file skipped " << std::endl;
		return;
	}
	

	//Read header
	while(!file.eof())
	{
		std::getline(file, line);

		file >> chunk;

		if(chunk == "element")
		{
			file >> chunk;
			if(chunk == "vertex")
			{
				file >> numVertices;
				point temp;
				for(int i = 0; i < numVertices; i++)
					normalObj.push_back(temp);
				
			}
			else if(chunk == "face")
				file >> numTriangles;
		}

		

		if(chunk == "property")
		{
			file >> chunk;//Read in data type

			file >> chunk;//Read in property

			if(chunk == "confidence")
				hasConfidence = true;
			if(chunk == "intensity")
				hasIntensity = true;
		}

		if(chunk == "end_header")
			break;

	}

	//Store Points
	for(int i = 0; i < numVertices; i++)
	{
		point temp;

		file >> x;
		file >> y;
		file >> z;

		temp.x = x;
		temp.y = y;
		temp.z = z;


		pointsObj.push_back(temp);

		if(hasConfidence)
			file >> confidence;
		if(hasIntensity)
			file >> intensity;

		points.push_back(x);
		points.push_back(y);
		points.push_back(z);
	}

	//Store faces
	for(int i = 0; i < numTriangles; i++)
	{

		file >> chunk;
		file >> p1;
		file >> p2;
		file >> p3;

		faces.push_back(p1);
		faces.push_back(p2);
		faces.push_back(p3);

//		point temp = calcNormal(p1, p2, p3, pointsObj, normalObj);
        calcNormal(p1, p2, p3, pointsObj, normalObj);

	}

	for(int i = 0; i < normalObj.size(); i++)
	{
		normals.push_back(normalObj[i].x);
		normals.push_back(normalObj[i].y);
		normals.push_back(normalObj[i].z);
	}

	modelPoints.push_back(points);
	modelNormals.push_back(normals);
	modelFaces.push_back(faces);

}


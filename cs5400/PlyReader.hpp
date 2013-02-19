#ifndef PLY_READER_X86
#define PLY_READER_X86

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif  // __APPLE__
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>


class PlyOpener
{
	
private:
	int numVertices;
	int numTriangles;
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLushort p1;
	GLushort p2;
	GLushort p3;

	GLfloat confidence;
	GLfloat intensity;
	std::string line;
	std::string chunk;
	std::fstream file;
	
	std::vector<GLfloat> points;
	std::vector<GLushort> faces;

public:
	void openFile(const std::string & filePath);
	void readData();
	std::vector<GLfloat> getPoints();
	std::vector<GLushort> getFaces();
	
};


#endif
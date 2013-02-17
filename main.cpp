#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "Angel.h"
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

//buffers, program, number of points
GLuint indexBuffer;
GLuint pointsBuffer;
GLuint program;
GLuint points;

// Viewing transformation parameters
point4 cameraPosition = point4(0,0,.5,1);
vec3 cameraAngle = vec3(0,0,0);
const GLfloat  dr = 5;
GLuint  model_view;  // model-view matrix uniform shader variable location

// Projection transformation parameters
GLfloat  fov = 60, aspect = 1;
GLfloat  zNear = 0.1, zFar = 3.0;
GLuint  projection; // projection matrix uniform shader variable location

//----------------------------------------------------------------------
// OpenGL initialization
bool
init( void )
{
    // Load shaders and use the resulting shader program
    program = InitShader( "vertex.glsl", "fragment.glsl" );
    
    std::ifstream PLY;
    
    std::string path = "";
    
    do
    {
        if(path != "")
        {
            std::cout << "Error opening path: " << path << "\n";
        }
        
        path = "";
        std::cout << "Enter path to .ply file:\n";
        std::cin >> path;
        
        std::cout << "Attempting to open .ply file...\n";
        
        if(path == "")
        {
            break;
        }
        
        PLY.open(path);
    }
    while(!PLY.is_open());
    
    if(!PLY.good())
    {
        std::cout << "Error with .ply file\n";
        return false;
    }
    
    bool supported = false;
    int vertecies = 0;
    int faces = 0;
    int xProperty = -1;
    int yProperty = -1;
    int zProperty = -1;
    int propertyCount = 0;
    
    std::vector<GLfloat> v;
    std::vector<GLint> f;
    
    std::cout << "Reading .ply file...\n";
    
    while(PLY.good())
    {
        std::string meta;
        std::string buffer;
        std::getline(PLY,meta);
        std::stringstream ss(meta);
        ss >> buffer;
        
        if(buffer == "ply" || buffer == "comment" || buffer == "")
        {}
        else if(buffer == "format")
        {
            ss >> buffer;
            if(buffer == "ascii")
            {
                ss >> buffer;
                if(buffer == "1.0")
                {
                    supported = true;
                }
                else
                {
                    std::cout << "Only files with format ascii 1.0 are supported\n";
                    return false;
                }
            }
            else
            {
                std::cout << "Only files with format ascii 1.0 are supported\n";
                return false;
            }
        }
        else if(buffer == "element")
        {
            ss >> buffer;
            if(buffer == "vertex")
            {
                ss >> vertecies;
                v.reserve(vertecies);
            }
            else if(buffer == "face")
            {
                ss >> faces;
                f.reserve(vertecies);
            }
            else
            {
                std::cout << "Warning: ignoring unrecognized element '" << buffer << "'\n";
            }
        }
        else if(buffer == "property")
        {
            ss >> buffer;
            if(buffer == "float")
            {
                ss >> buffer;
                if(buffer == "x")
                {
                    xProperty = propertyCount;
                }
                else if(buffer == "y")
                {
                    yProperty = propertyCount;
                }
                else if(buffer == "z")
                {
                    zProperty = propertyCount;
                }
                //else ignore
                
            }
            //else ignore
            
            ++propertyCount;
        }
        else if(buffer == "end_header")
        {
            if(!vertecies)
            {
                std::cout << "Error: no verticies\n";
                return false;
            }
            
            if(!faces)
            {
                std::cout << "Error: no faces\n";
                return false;
            }
            
            for(int i = 0; i < vertecies; ++i)
            {
                std::getline(PLY,buffer);
                int index = 0;
                GLfloat token;
                std::stringstream ts(buffer);
                while(ts >> token)
                {
                    if(index == xProperty || index == yProperty || index == zProperty)
                    {
                        v.push_back(token);
                    }
                    ++index;
                }
            }
            
            for(int i = 0; i < faces; ++i)
            {
                std::getline(PLY,buffer);
                GLuint token;
                std::stringstream ts(buffer);
                ts >> token;
                ts >> token;
                f.push_back(token);
                ts >> token;
                f.push_back(token);
                ts >> token;
                f.push_back(token);
            }
        }
        else
        {
            std::cout << "Warning: ignoring unrecognized metadata '" << buffer << "'\n";
        }
        
    }
    PLY.close();
    
    points = faces*3;
    
    glUseProgram(program);
    
    glGenBuffers(1,&indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,f.size()*sizeof(GLuint),f.data(),GL_STATIC_DRAW);
    
    glGenBuffers(1,&pointsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,pointsBuffer);
    glBufferData(GL_ARRAY_BUFFER,v.size()*sizeof(GLfloat),v.data(),GL_STATIC_DRAW);
    
    model_view = glGetUniformLocation( program, "model_view" );
    projection = glGetUniformLocation( program, "projection" );
    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    return true;
}
//----------------------------------------------------------------------
void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    point4  at(cameraPosition[0], cameraPosition[1], cameraPosition[2]-1, 1.0 );
    vec4    up( 0.0, 1.0, 0.0, 0.0 );
    mat4  mv = LookAt( cameraPosition, at, up )
             * RotateX(cameraAngle[0])
             * RotateY(cameraAngle[1])
             * RotateZ(cameraAngle[2]);
    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );
    mat4  p = Perspective(fov, aspect, zNear, zFar);
    glUniformMatrix4fv( projection, 1, GL_TRUE, p );
    
    glUseProgram(program);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,pointsBuffer);
    auto coord3d = glGetAttribLocation(program,"coord3d");
    glEnableVertexAttribArray(coord3d);
    glVertexAttribPointer(coord3d,3,GL_FLOAT,GL_FALSE,0,0);
    
    glDrawElements(GL_TRIANGLES,points,GL_UNSIGNED_INT,0);
    
    glutSwapBuffers();
}
//----------------------------------------------------------------------
void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
        case 033: // Escape Key
            exit( EXIT_SUCCESS );
            break;
        case 'o': cameraAngle[0] += dr; break;
        case 'l': cameraAngle[0] -= dr; break;
        case ';': cameraAngle[1] += dr; break;
        case 'k': cameraAngle[1] -= dr; break;
        case 'p': cameraAngle[2] += dr; break;
        case 'i': cameraAngle[2] -= dr; break;
        case 'w': cameraPosition[1] -= .1; break;
        case 's': cameraPosition[1] += .1; break;
        case 'd': cameraPosition[0] -= .1; break;
        case 'a': cameraPosition[0] += .1; break;
        case 'r': cameraPosition[2] += .1; break;
        case 'f': cameraPosition[2] -= .1; break;
        case ' ':  // reset values to their defaults
            cameraAngle = vec3(0,0,0);
            cameraPosition = point4(0,0,2,1);
            break;
    }
    glutPostRedisplay();
}
//----------------------------------------------------------------------
void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
}
//----------------------------------------------------------------------
int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( ".ply Viewer" );
    glewExperimental = GL_TRUE;
    glewInit();
    if(!init())
    {
        std::cout << "Error initializing.\n";
        return 1;
    }
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutReshapeFunc( reshape );
    glutMainLoop();
    return 0; }
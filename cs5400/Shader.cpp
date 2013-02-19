#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif  // __APPLE__

#include "Shader.hpp"


namespace 
{
  std::string getCode(std::string path)
  {
    std::string contents;
    std::ifstream fin(path,std::ios::in | std::ios::binary);
    if(!fin.good())
    {
      throw std::runtime_error("Could not open shader file: " + path);
    }
    fin.seekg(0,std::ios::end);
    contents.resize(fin.tellg());
    fin.seekg(0,std::ios::beg);
    fin.read(&contents[0],contents.size());
    fin.close();
  #ifdef GL_ES_VERSION_2_0
    std::string header = "#version 100\n"
			"#define GLES2\n";
  #else
    std::string header = "#version 120\n";
  #endif   
    return header + contents;
  }
  
  void setCode(GLuint handle,const std::string & code)
  {
    const GLchar* source[1] = {code.c_str()};
    glShaderSource(handle,1,source,NULL);
    glCompileShader(handle);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(handle,GL_COMPILE_STATUS,&compile_ok);
    if(compile_ok == GL_FALSE)
      throw std::runtime_error("Shader failed to compile");
  }
}


template<typename T>
std::shared_ptr<cs5400::Shader<T>> make_shader(std::string path)
{
  auto code = getCode(path);
  auto shader = std::make_shared<cs5400::Shader<T>>();
  setCode(shader->getHandle(),code);
  return shader;
}

std::shared_ptr<cs5400::VertexShader> cs5400::make_vertexShader(std::string path)
{
  return make_shader<detail::VertexShaderTag>(path);
}

std::shared_ptr<cs5400::FragmentShader> cs5400::make_fragmentShader(std::string path)
{
  return make_shader<detail::FragmentShaderTag>(path);
}

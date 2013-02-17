attribute vec3 coord3d;
varying vec3 color;
uniform mat4 model_view;
uniform mat4 projection;

//rand function from http://www.ozone3d.net/blogs/lab/20110427/glsl-random-generator/

float rand(vec2 n)
{
  return 0.65 + 0.15 * 
     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main() {
    gl_Position = projection*model_view*vec4(coord3d,1);
    color = vec3(coord3d.x * 5.0,coord3d.y * 2.0,coord3d.z * 8.0);
    float c = rand(coord3d.yz);
    color = vec3(c,c,c);
}
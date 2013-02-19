
attribute  vec3 vPosition;
attribute  vec3 vNormal;
varying vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

uniform float xColor;
uniform float yColor;
uniform float zColor;

uniform mat4 ModelView;
uniform mat4 Projection;

void main() 
{

	 // Transform vertex  position into eye coordinates
    vec3 pos = (ModelView * vec4(vPosition, 0.0)).xyz;
	
    vec3 L = normalize( LightPosition.xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( ModelView * vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

	
	color = diffuse + specular + ambient;
	
	//color =
       //vec4( ( vPosition.x * vNormal.x * xColor ), (vPosition.y * vNormal.y * yColor ),
              //( vPosition.z * vNormal.z * zColor), 0.0 );
	
    color.a = 1.0;
    
    gl_Position = ModelView  * vec4(vPosition, 1.0);
} 

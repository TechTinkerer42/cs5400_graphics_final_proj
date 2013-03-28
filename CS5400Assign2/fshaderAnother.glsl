
varying vec4 color;
varying vec3 varyingNormalDirection;


void main() 
{


    float val = dot(normalize(varyingNormalDirection * varyingNormalDirection), vec3(0,0,1));

    if(val < .6 && val > -.6)
    {
        gl_FragColor = color * vec4((val+.3),(val+.3),(val+.3),0);
    }
    else
    {
        gl_FragColor = color;
    }
    
} 


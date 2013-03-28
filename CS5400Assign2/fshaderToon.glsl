
varying vec4 color;
varying vec3 varyingNormalDirection;

float round(float n)
{
    return floor(n+.5);
}

vec4 bitReduction(vec4 c, float bits)
{
    return vec4(round(c.x*bits)/bits,round(c.y*bits)/bits,round(c.z*bits)/bits,c.w);
}

void main() 
{


    float val = dot(normalize(varyingNormalDirection * varyingNormalDirection), vec3(0,0,1));

    if(val < .6 && val > -.6)
    {
        gl_FragColor = vec4(0,0,0,0);
    }
    else
    {
        gl_FragColor = bitReduction(color, 4);
    }
    
} 


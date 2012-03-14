# version 120 


// Mine is an old machine.  For version 130 or higher, do 
// out vec4 color ;  
// out vec4 mynormal ; 
// out vec4 myvertex ;
// That is certainly more modern

varying vec4 color ; 
varying vec3 mynormal ; 
varying vec4 myvertex ; 
varying vec4 lambertVert;

uniform vec4 diffuse ; 
uniform int vertexShading;
/* Color and Position for lights */
uniform int numLights;
uniform vec4 lightPosn[10];
uniform vec4 lightColor[10];

vec4 ComputeLambert (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec4 mydiffuse) {

        float nDotL = dot(normal, direction)  ;         
        vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  
 
        return lambert ;            
}


void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0 ; 
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex ; 
    color = gl_Color ; 
    mynormal = gl_Normal ; 
    myvertex = gl_Vertex ; 
	
	lambertVert = vec4(0,0,0,0);
	if (vertexShading!=0){
		vec4 _mypos = gl_ModelViewMatrix * gl_Vertex ; 
        vec3 mypos = _mypos.xyz / _mypos.w ; // Dehomogenize current location

		vec3 position, direction;

		for(int i=0; i<numLights ;i++) {	
			if (lightPosn[i].w==0) {
				direction = normalize(lightPosn[i].xyz);
			} else {
	        	position = lightPosn[i].xyz / lightPosn[i].w ; 
				direction = normalize (position-mypos) ;
			}
	
	        lambertVert += ComputeLambert(direction,lightColor[i],gl_Normal,diffuse);
		}
	}
}


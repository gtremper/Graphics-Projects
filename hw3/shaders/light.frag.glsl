# version 120 

// Mine is an old machine.  For version 130 or higher, do 
// in vec4 color ;  
// in vec4 mynormal ; 
// in vec4 myvertex ;
// That is certainly more modern

varying vec4 color ;
varying vec3 mynormal ; 
varying vec4 myvertex ; 

uniform sampler2D tex ; 

uniform int istex ;
uniform int islight ; // are we lighting. 
uniform int numLights;

/* Color and Position for lights */
uniform vec4 lightPosn[10];
uniform vec4 lightColor[10];

// Now, set the material parameters.  These could be varying and/or bound to 
// a buffer.  But for now, I'll just make them uniform.  
// I use ambient, diffuse, specular, shininess as in OpenGL.  
// But, the ambient is just additive and doesn't multiply the lights.  

uniform vec4 ambient ;
uniform vec4 emission ; 
uniform vec4 diffuse ; 
uniform vec4 specular ; 
uniform float shininess ; 

vec4 ComputeLight (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in float myshininess) {

        float nDotL = dot(normal, direction)  ;         
        vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

        float nDotH = dot(normal, halfvec) ; 
        vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

        vec4 retval = lambert + phong ; 
        return retval ;            
}       

void main (void) 
{   
    if (islight == 0) gl_FragColor = color ; 
    else { 
        /* They eye is always at (0,0,0) looking down -z axis 
           Also compute current fragment position and direction to eye */ 
        const vec3 eyepos = vec3(0,0,0) ; 
        vec4 _mypos = gl_ModelViewMatrix * myvertex ; 
        vec3 mypos = _mypos.xyz / _mypos.w ; // Dehomogenize current location 
        vec3 eyedirn = normalize(eyepos - mypos) ; 

        /* Compute normal, needed for shading. */   
         vec3 normal = normalize(gl_NormalMatrix * mynormal) ; 

		/* Initialize variables */   
		vec3 position, direction, halfAngle;
		vec4 totalCol = vec4(0,0,0,0);
		
		/* Sum over all lights */  
		for(int i=0; i<numLights ;i++) {	
			if (lightPosn[i].w==0) {
				direction = normalize(lightPosn[i].xyz);
			} else {
	        	position = lightPosn[i].xyz / lightPosn[i].w ; 
				direction = normalize (position-mypos) ;
			}
	        halfAngle = normalize (direction + eyedirn) ;  
	        totalCol += ComputeLight(direction, lightColor[i], normal, halfAngle, diffuse, specular, shininess);
		}
		if (istex != 0) {
			gl_FragColor = totalCol * texture2D(tex, gl_TexCoord[0].st) ; 
		} else {
			gl_FragColor = totalCol + ambient + emission;
		}		
	}
	
}
		

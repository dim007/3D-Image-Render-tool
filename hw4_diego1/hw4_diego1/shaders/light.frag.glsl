# version 330 core
// Do not use any version older than 330!

/* This is the fragment shader for reading in a scene description, including 
   lighting.  Uniform lights are specified from the main program, and used in 
   the shader.  As well as the material parameters of the object.  */

// Inputs to the fragment shader are the outputs of the same name of the vertex shader.
// Note that the default output, gl_Position, is inaccessible!
in vec3 mynormal; 
in vec4 myvertex; 

// You will certainly need this matrix for your lighting calculations
uniform mat4 modelview;

// This first defined output of type vec4 will be the fragment color
out vec4 fragColor;

uniform vec3 color;

const int numLights = 10; 
uniform bool enablelighting; // are we lighting at all (global).
uniform vec4 lightposn[numLights]; // positions of lights 
uniform vec4 lightcolor[numLights]; // colors of lights
uniform int numused;               // number of lights used

// Now, set the material parameters.
// I use ambient, diffuse, specular, shininess. 
// But, the ambient is just additive and doesn't multiply the lights.  

uniform vec4 ambient; 
uniform vec4 diffuse; 
uniform vec4 specular; 
uniform vec4 emission; 
uniform float shininess; 

//FROM HW1
vec4 ComputeLight (vec3 direction, vec4 lightcolor, vec3 normal, vec3 halfvec, vec4 mydiffuse, vec4 myspecular, float myshininess) {

        float nDotL = dot(normal, direction)  ;         
        vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

        float nDotH = dot(normal, halfvec) ; 
        vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

        vec4 retval = lambert + phong ; 
        return retval ;
}       

void main (void) 
{       
    if (enablelighting) {       
        vec4 finalcolor = vec4(0.0f, 0.0f, 0.0f, 1.0f); 

        // YOUR CODE FOR HW 2 HERE
        // A key part is implementation of the fragment shader
		
		//ADAPTED FROM HW1
		const vec3 eyepos = vec3(0,0,0);
		vec4 mv = modelview * myvertex;	//need to combine with modelview //must be in this order
		vec3 mypos = mv.xyz / mv.w; //Dehomogenize vector
									//THIS IS CALLED "SWIZZLING" vec3.xyz or vec3.xxyy, up to 4 variables
		vec3 eyedirn = normalize(eyepos - mypos);
		vec4 norm4 = vec4(mynormal,0.0f);
		vec4 mvnorm = transpose(inverse(modelview)) * norm4; //BugFix:need invere transpose to maintain the normal property to surface when scaling
		vec3 normal = normalize(mvnorm.xyz); //Fixed Bug: Lights in wrong positions, stationary

		for(int idx = 0; idx < numused; idx++){

			//w=0 means directional lights
			vec3 lightpos3v = (lightposn[idx].w == 0) ? lightposn[idx].xyz : lightposn[idx].xyz / lightposn[idx].w;
			//if w=1 no attenuation (lightpos3v - mypos)
			vec3 lightdirn = (lightposn[idx].w == 0) ? normalize(lightpos3v) : normalize(lightpos3v - mypos);

			vec3 half0 = normalize(lightdirn + eyedirn);
			
			vec4 lightcol = lightcolor[idx];
			vec4 col = ComputeLight(lightdirn, lightcol, normal, half0, diffuse, specular, shininess);

			finalcolor = finalcolor + col;
		}
		finalcolor = finalcolor + ambient; //add ambient or else just black teapot


        // Color all pixels black for now, remove this in your implementation!
        //finalcolor = vec4(0.0f, 0.0f, 0.0f, 1.0f); 

        fragColor = finalcolor; 
    } else {
        fragColor = vec4(color, 1.0f); 
    }
}

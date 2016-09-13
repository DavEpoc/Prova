#version 330
// It was expressed that some drivers required this next line to function properly
precision highp float;
 
in vec4 ex_Color;
in vec2 Texcoord;

out vec4 fragColor;

//default value 0 set only if multiple textures
uniform sampler2D tex; 
 
void main(void) {
	//fragColor = ex_Color;
	fragColor = texture(tex,Texcoord) * ex_Color;
	//fragColor = texture(tex,Texcoord) + ex_Color;
	//fragColor = texture(tex,Texcoord);
}
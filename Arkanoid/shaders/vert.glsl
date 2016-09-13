#version 330
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec4 in_Color;
layout(location = 2) in vec2 texcoord;

uniform mat4 mvp;
 
// We output the ex_Color variable to the next shader in the chain
out vec4 ex_Color;

out vec2 Texcoord;

void main(void) {
	// Set the position to the one defined in our vertex array
	vec4 pos = vec4(in_Position, 1.0f);

	gl_Position = mvp * pos;

    // Pass the color on to the fragment shader
	ex_Color = in_Color;
    // Pass the texcoord on to the fragment shader
        Texcoord = texcoord;
}
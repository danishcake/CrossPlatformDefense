/*struct V2Textured
{
	vec2 pos;
	vec2 texc;
};

attribute V2Textured vert_in;*/

attribute vec2 pos;
attribute vec2 texc;


varying vec2 texCoord;

void main(void) {
/*
    gl_Position.x = vert_in.pos.x * 2.0 - 1.0;
    gl_Position.y = vert_in.pos.y * 2.0 - 1.0;
    gl_Position.x *= 0.9;
    gl_Position.y *= 0.9;    
    texCoord.xy = vert_in.texc.xy;
	*/

	gl_Position.x = pos.x * 2.0 - 1.0;
    gl_Position.y = pos.y * 2.0 - 1.0;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;

    gl_Position.x *= 0.9;
    gl_Position.y *= 0.9;    
    texCoord.xy = texc.xy;
}
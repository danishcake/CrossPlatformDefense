varying lowp vec2 texCoord;
uniform sampler2D texture;

void main(void) {
    //gl_FragColor = vec4(1, 1, 1, 1);
	gl_FragColor = texture2D(texture, texCoord);
}
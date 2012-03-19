varying lowp vec2 texCoord;
uniform sampler2D texture;

void main(void) {
	lowp vec4 tx = texture2D(texture, texCoord);
    gl_FragColor = vec4(tx.rgb, 1); //Flat unshaded
}
varying lowp vec4 VertexColor;
varying lowp vec2 VertexCoord;
uniform sampler2D texture;

void main(void) {
    lowp vec4 tx = texture2D(texture, VertexCoord);
    lowp vec4 col = vec4(tx.r * VertexColor.r, tx.r * VertexColor.g, tx.r * VertexColor.b, tx.r);
    gl_FragColor = col;
}
/*
 * Transform matrix for vertices
 */
uniform mat4 Transform;

/*
 * Input stream of positions
 */
attribute vec4 InputPosition;

/*
 * Input stream of texture coords
 */
attribute vec2 InputTextureCoord;

/*
 * Output stream of texture coords
 */
varying lowp vec2 VertexTextureCoord;

void main(void) {
    gl_Position = Transform * InputPosition;
    VertexTextureCoord = InputTextureCoord;
}
/*
 * Transform matrix for vertices
 */
uniform mat4 Transform;

/*
 * Input stream of positions
 */
attribute vec4 InputPosition;
/*
 * Input stream of texture coordinates
 */
attribute vec2 InputTextureCoord;

/*
 * Output stream of texture coordinates
 */
varying lowp vec2 texCoord;

void main(void) {
    gl_Position = Transform * InputPosition;
    texCoord = InputTextureCoord;
}
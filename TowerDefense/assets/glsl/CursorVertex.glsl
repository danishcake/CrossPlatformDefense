/*
 * Transform matrix for vertices
 */
uniform mat4 Transform;

/*
 * Input stream of positions
 */
attribute vec4 InputPosition;

void main(void) {
    gl_Position = Transform * InputPosition;
    texCoord = InputTextureCoord;
}
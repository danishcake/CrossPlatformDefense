/*
 * Transform matrix for vertices
 */
uniform mat4 Transform;

/*
 * Input stream of positions
 */
attribute vec4 InputPosition;

/*
 * Input stream of colorss
 */
attribute vec4 InputColor;

/*
 * Output stream of colors
 */
varying lowp vec4 VertexColor;

void main(void) {
    gl_Position = Transform * InputPosition;
    VertexColor = InputColor;
}
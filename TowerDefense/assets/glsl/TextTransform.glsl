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
 * Input stream of texture coordinate
 */
attribute vec2 InputTextureCoords;

/*
 * Output stream of texure coordinates
 */
varying lowp vec2 VertexCoord;

/*
 * Output stream of colors
 */
varying lowp vec4 VertexColor;

void main(void) {
    gl_Position = Transform * InputPosition;
    VertexColor = InputColor;
    VertexCoord = InputTextureCoords;
}
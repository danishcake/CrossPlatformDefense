/*
 * Transform matrix for vertices
 */
uniform mat4 Transform;

/* 
 * Transparency uniform
 */
uniform lowp float VertexAlpha;

/* 
 * Progress uniform
 */
uniform mediump float ProgressWidth;

/* 
 * Color LUT
 */
uniform lowp vec4 VertexColorLUT[2];

/*
 * Input stream of positions
 */
attribute vec4 InputPosition;

/*
 * Input stream of colorss
 */
attribute float InputVertexColorIndex;

/*
 * Output stream of colors
 */
varying lowp vec4 VertexColor;

void main(void) {
    gl_Position = Transform * vec4(InputPosition.x + (ProgressWidth * InputPosition.z), InputPosition.y, 0, InputPosition.w);
    VertexColor.rgb = VertexColorLUT[int(InputVertexColorIndex)].rgb;
	VertexColor.a = VertexAlpha;
}
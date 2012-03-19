//Assume one input texture of 256x256
//Assume three textures for each block type
//Eg top, side and bottom. Each texture is 32x32 and packed into the texture
//So inputting a single byte will be enough to calculate the texture coordinates
//Allows 16 block types

//Input index
//xbase = (index * 32) % 256; //Or & 0xFF
//ybase = ((index * 32) / 256) * 96; //Or (((index * 32) & 0xFF00) >> 8) * 96
//Then normalise TODO

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
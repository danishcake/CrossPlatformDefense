attribute vec2 Position;

void main(void) {
    gl_Position.x = Position.x * 2.0 - 1.0;
    gl_Position.y = Position.y * 2.0 - 1.0;
    gl_Position.z = 0.0;
    gl_Position.w = 1.0;
}
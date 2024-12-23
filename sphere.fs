#version 330 core
out vec4 FragColor;

uniform vec3 color; // 小球顏色

void main() {
    FragColor = vec4(color, 1.0); // 設置輸出顏色
}

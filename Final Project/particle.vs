#version 330 core
layout(location = 0) in vec3 aPos; // 位置
layout(location = 1) in vec4 aColor; // 顏色
layout(location = 2) in float aSize; // 粒子大小

out vec2 TexCoords; // 傳遞到片段著色器的紋理坐標

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 計算最終位置
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_PointSize = aSize;

    // 設置 TexCoords
    TexCoords = (aPos.xy + 1.0) * 0.5; // 假設粒子的範圍在 [-1, 1]
}

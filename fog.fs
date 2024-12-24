#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 fogColor;
uniform float fogStart; // 霧開始距離
uniform float fogEnd;   // 霧結束距離
uniform vec3 fogCenter; // 霧的中心位置

void main() {
    // 計算物體顏色
    vec4 texColor = texture(texture1, TexCoords);

    // 計算霧的濃度
    float distance = length(FragPos - fogCenter);
    float fogFactor = clamp((fogEnd - distance) / (fogEnd - fogStart), 0.0, 1.0);

    // 混合霧的顏色和物體顏色
    vec3 finalColor = mix(fogColor, texColor.rgb, fogFactor);
    FragColor = vec4(finalColor, texColor.a);
}

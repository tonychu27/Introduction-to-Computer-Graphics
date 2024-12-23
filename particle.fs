#version 330 core
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D particleTexture;

void main()
{
    // 從紋理中取樣顏色
    vec4 sampled = texture(particleTexture, TexCoords);

    // 設置最終顏色
    FragColor = sampled;
}

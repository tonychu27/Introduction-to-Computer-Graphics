#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform sampler2D ourTexture;

void main()
{
    vec3 ambient = vec3(1.0);

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    vec3 lighting = (ambient + diffuse);
    vec4 texColor = texture(ourTexture, texCoord);
    FragColor = vec4(lighting * texColor.rgb, texColor.a);
}

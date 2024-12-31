#version 430

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D ourTexture;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirectionLight
{
    int enable;
    vec3 direction;
    vec3 lightColor;
};

struct PointLight {
    int enable;
    vec3 position;  
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;
};

struct Spotlight {
    int enable;
    vec3 position;
    vec3 direction;
    vec3 lightColor;
    float cutOff;

    // Paramters for attenuation formula
    float constant;
    float linear;
    float quadratic;      
}; 

uniform Material material;
uniform DirectionLight dl;
uniform PointLight pl;
uniform Spotlight sl;

/*** Function to calculate directional light contribution ***/
vec4 dirLight(){

    /*** Normalize the normal vector and calculate view and light directions ***/
    vec3 norm = normalize(Normal);
    vec3 view = normalize(viewPos - FragPos);
    vec3 light = normalize(-dl.direction);
    vec3 reflect = reflect(-light, norm);

    /*** Calculate diffuse and specular lighting components ***/
    float diff = max(dot(norm, light), 0.0);
    float spec = pow(max(dot(view, reflect), 0.0), material.shininess);

    /*** Calculate ambient, diffuse, and specular components ***/
    vec4 ambient  = vec4(dl.lightColor * material.ambient, 1.0)  * texture(ourTexture, TexCoord);
    vec4 diffuse  = vec4(dl.lightColor * material.diffuse * diff, 1.0) * texture(ourTexture, TexCoord);
    vec4 specular = vec4(dl.lightColor * material.specular * spec, 1.0) * texture(ourTexture, TexCoord);

    return (ambient + diffuse + specular);
}  

/*** Function to calculate spotlight contribution ***/
vec4 spotLight(){

    /*** Normalize the normal vector and calculate view and light directions ***/
    vec3 norm = normalize(Normal);
    vec3 light = normalize(sl.position - FragPos);
    vec3 view = normalize(viewPos - FragPos);
    vec3 reflect = reflect(-light, norm);

    /*** Calculate the spotlight cutoff condition and lighting components ***/
    float theta = dot(light, normalize(-sl.direction)); 
    float spec = pow(max(dot(view, reflect), 0.0), material.shininess);
    float diff = max(dot(norm, light), 0.0);

    /*** Calculate attenuation based on distance ***/
    float dist = length(sl.position - FragPos);
    float attenuation = 1.0 / (sl.constant + sl.linear * dist + sl.quadratic * (dist * dist));    

    /*** Calculate ambient, diffuse, and specular components ***/
    vec4 ambient = vec4(sl.lightColor * material.ambient, 1.0) * texture(ourTexture, TexCoord);
    vec4 diffuse = vec4(sl.lightColor * material.diffuse * diff, 1.0) * texture(ourTexture, TexCoord);
    vec4 specular = vec4(sl.lightColor * material.specular * spec, 1.0) * texture(ourTexture, TexCoord);

    /*** Apply attenuation to diffuse and specular components ***/
    diffuse *= attenuation;
    specular *= attenuation;

    /*** Return lighting if within the cutoff, otherwise return ambient lighting ***/
    if(theta > sl.cutOff) return (ambient + diffuse + specular);
    return vec4(sl.lightColor * material.ambient, 1.0) * texture(ourTexture, TexCoord);
}

/*** Function to calculate point light contribution ***/
vec4 pointLight(){

    /*** Normalize the normal vector and calculate view and light directions ***/
    vec3 norm = normalize(Normal);
    vec3 light = normalize(pl.position - FragPos);
    vec3 view = normalize(viewPos - FragPos);
    vec3 reflect = reflect(-light, norm);

    /*** Calculate diffuse and specular lighting components ***/
    float spec = pow(max(dot(view, reflect), 0.0), material.shininess);
    float diff = max(dot(norm, light), 0.0);

    /*** Calculate attenuation based on distance ***/
    float dist = length(pl.position - FragPos);
    float attenuation = 1.0 / (pl.constant + pl.linear * dist + pl.quadratic * (dist * dist));  

    /*** Calculate ambient, diffuse, and specular components ***/   
    vec4 ambient  = vec4(pl.lightColor * material.ambient, 1.0)  * texture(ourTexture, TexCoord);
    vec4 diffuse  = vec4(pl.lightColor * material.diffuse * diff, 1.0) * texture(ourTexture, TexCoord);
    vec4 specular = vec4(pl.lightColor * material.specular * spec, 1.0) * texture(ourTexture, TexCoord);

    /*** Apply attenuation to diffuse and specular components ***/
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main() {
    /*** Initialize color to a default value ***/
    color = vec4(0.0, 0.0, 0.0, 1.0);
    
    /*** Add directional light contribution if enabled ***/
    if(dl.enable == 1) color += dirLight();

    /*** Add point light contribution if enabled ***/
    if(pl.enable == 1) color += pointLight();
    
    /*** Add spotlight contribution if enabled ***/
    if(sl.enable == 1) color += spotLight();
    
     /*** Default color when no lights are enabled ***/
    if(dl.enable == 0 && pl.enable == 0 && sl.enable == 0) color = vec4(material.ambient, 1.0) * texture(ourTexture, TexCoord);
}
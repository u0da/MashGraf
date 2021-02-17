#version 330 core

in vec3 normal_in_view;
in vec3 pos_in_view;
in vec3 lightPos_in_view;
in vec2 TexCoords;

out vec4 color;
  
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D Texture;

void main()
{
    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
      
    // Diffuse
    vec3 norm = normalize(normal_in_view);
    vec3 light_pos = normalize(lightPos_in_view - pos_in_view);
    float diff = max(dot(norm, light_pos), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5f;
    float shininess = 32;
    vec3 view_pos = normalize(-pos_in_view);
    vec3 reflect_light_pos = reflect(-light_pos, norm);
    float spec = pow(max(dot(view_pos, reflect_light_pos), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = texture(Texture, TexCoords) * vec4(result, 1.0f);
} 

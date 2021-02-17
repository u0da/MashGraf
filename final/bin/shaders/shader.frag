#version 330 core

in vec3 normal_in_view;
in vec3 pos_in_view;
in vec3 lightPos_in_view;

out vec4 color;
  
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D Texture;

uniform float constant;
uniform float linear;
uniform float quadratic;

uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;


void main()
{
    // Ambient
    vec3 ambient = ambientStrength * lightColor;
      
    // Diffuse
    vec3 norm = normalize(normal_in_view);
    vec3 light_pos = normalize(lightPos_in_view - pos_in_view);
    float diff = max(dot(norm, light_pos), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    vec3 view_pos = normalize(-pos_in_view);
    vec3 reflect_light_pos = reflect(-light_pos, norm);
    float spec = pow(max(dot(view_pos, reflect_light_pos), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    float dist = length(lightPos_in_view - pos_in_view);
    float attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0f);
} 

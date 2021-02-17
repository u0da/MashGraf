#version 330 core
out vec4 color;

in vec3 pos_in_model;
in vec3 norm_in_model;
  
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;

float CookTorrance(vec3 normal, vec3 light, vec3 view, float m) {
    if (m <= 0.0) return 0.0;
    vec3  half_vec = normalize(view + light);
    float N_L = max(dot(normal, light), 0.0);
    float N_V = max(dot(normal, view), 0.0);
    float N_H = max(dot(normal, half_vec), 0.0);
    if(N_H == 0.0)
        N_H = 1.0e-7;
    
    float V_H = max(dot(view, half_vec), 0.0);
    if(V_H == 0.0)
        V_H = 1.0e-7;
    
    float G = min(1.0, 2 * N_H * min(N_V, N_L) / V_H);

    float tmp = (N_H * N_H - 1.0) / (N_H * N_H  * m * m);
    float R = exp(tmp) / (4.0 * N_H * N_H * N_H * N_H * m * m);
    
    float F = 1.0 / (1.0 + N_V);

    return min(1.0, (F * G * R) / (N_V * N_L + 1.0e-7));
}

void main()
{

    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * vec3(1.0f);
      
    vec3 norm = normalize(norm_in_model);
    vec3 lightDir = normalize(lightPos - pos_in_model);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0f);
    
    float specularStrength = 0.5f;
    float roughness_val = 0.5f;
    vec3 viewDir = normalize(viewPos - pos_in_model);
    float spec = CookTorrance(norm, lightDir, viewDir, roughness_val);
    vec3 specular = specularStrength * spec * diff * vec3(1.0f);
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0f);
} 

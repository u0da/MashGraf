#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 norm_in_model;
out vec3 pos_in_model;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view *  model * vec4(position, 1.0f);
    pos_in_model = vec3(model * vec4(position, 1.0f));
    norm_in_model = mat3(transpose(inverse(model))) * normal;
} 

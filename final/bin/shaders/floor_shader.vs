#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 normal_in_view;
out vec3 pos_in_view;
out vec3 lightPos_in_view;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;

void main()
{
    gl_Position = projection * view *  model * vec4(position, 1.0f);
    pos_in_view = vec3(view * model * vec4(position, 1.0f));
    normal_in_view = mat3(transpose(inverse(view * model))) * normal;
    lightPos_in_view = vec3(view * vec4(lightPos, 1.0));
    TexCoords = texCoords;
}

#version 330 core
out vec4 FragColor;

in vec3 cubeMapCoord;

uniform samplerCube skybox;

void main()
{    
    FragColor = textureLod(skybox, cubeMapCoord, 0);
}
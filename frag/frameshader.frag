#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int screenmode;

void main()
{ 
     if(screenmode==0)
        FragColor = vec4(vec3(texture(screenTexture, TexCoords)), 1.0);
     else
        FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}
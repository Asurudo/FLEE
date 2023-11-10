#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

//uniform vec3 velocity;
//uniform vec3 acceleration;
//uniform float age;

void main()
{
      // gl_Position = projection * view  * vec4(position + velocity*age + 0.5 * acceleration *age*age, 1.0f);
      gl_Position = projection * view  * model * vec4(aPos, 1.0f);
}
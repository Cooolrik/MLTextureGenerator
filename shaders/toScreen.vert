#version 330 core
layout (location = 0) in vec3 aCoord;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 Transform;

void main()
	{
    gl_Position = Transform * vec4(aCoord,1.0);
    TexCoord = aTexCoord;
	}
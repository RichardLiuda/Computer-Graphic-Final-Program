#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    //TexCoords = aPos;
    TexCoords = vec3(aPos.x, -aPos.y, aPos.z);
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // 确保深度值为1.0
}
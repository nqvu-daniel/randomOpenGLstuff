#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 a_Color;
out vec2 v_TexCoord;
out vec4 v_Color;
uniform mat4 u_MVP; // model view projection matrix


void main()
{
   gl_Position = u_MVP * position ;
   v_TexCoord = texCoord;
   v_Color = a_Color;
}

#shader fragment
#version 330 core

in vec2 v_TexCoord;
in vec4 v_Color;

out vec4 FragColor;
uniform vec4 ourColor;
uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, v_TexCoord) * v_Color;
    FragColor = texColor;// * ourColor;
}
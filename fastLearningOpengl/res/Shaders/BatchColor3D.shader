#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 color;
layout(location = 3) in float texidx;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out     vec4 v_Color;
out     vec2 v_TexCoord;
out     float v_TexIndex;

void main()
{
	gl_Position = projection * view * model * position;
	v_Color = color;
	v_TexCoord = texcoord;
	v_TexIndex = texidx;
}


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D u_Textures[2];
in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;

void main()
{
	int index = int(v_TexIndex);
	// non-constant expressions are forbidden in GLSL 1.30 (GLSL 4.0 supports)
	//color = texture(u_Textures[index], v_TexCoord);
	switch (index) {
	case 0: color = texture(u_Textures[0], v_TexCoord); break;
	case 1: color = texture(u_Textures[1], v_TexCoord); break;
	}
}
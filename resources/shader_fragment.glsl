#version 330 core
out vec4 color;
in vec3 pos;
void main()
{
	color.rgb = normalize(pos);
	color.a = 1;
}
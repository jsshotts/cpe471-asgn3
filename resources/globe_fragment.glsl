#version 330 core
out vec4 color;
in vec3 fragNor;
uniform vec3 camPos;
void main()
{
	vec3 normal = normalize(fragNor);
	vec3 cam = normalize(camPos);
	color.r = 0.73;
	color.g = 0.90;
	color.b = 0.97;
	color.a=1-clamp(dot(normal, cam), 0, 1);
}
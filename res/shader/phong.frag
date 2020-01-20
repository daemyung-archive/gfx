precision mediump float;

layout(location = 0) in vec3 light_intensity;

layout(location = 0) out vec4 frag_color0;

void main()
{
	frag_color0 = vec4(light_intensity, 1.0);
}
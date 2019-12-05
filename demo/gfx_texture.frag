precision mediump float;

layout(location = 0) in vec2 tex_coord;

layout(location = 0) out vec4 frag_color0;

layout(binding = 0) uniform sampler2D diffuse_texture;

void main()
{
	frag_color0 = texture(diffuse_texture, tex_coord);
}
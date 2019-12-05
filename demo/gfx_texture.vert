layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_tex_coord;

layout(location = 0) out vec2 tex_coord;

//----------------------------------------------------------------------------------------------------------------------

void main()
{
	gl_Position = vec4(vertex_position, 1.0);
	tex_coord = vertex_tex_coord;
}

//----------------------------------------------------------------------------------------------------------------------
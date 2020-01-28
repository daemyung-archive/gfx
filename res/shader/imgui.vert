//----------------------------------------------------------------------------------------------------------------------

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) in vec2 vertex_uv;

//----------------------------------------------------------------------------------------------------------------------

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 uv;

//----------------------------------------------------------------------------------------------------------------------

layout(binding = 0) uniform Imgui_info {
	vec2 translation;
	vec2 scale;
};

//----------------------------------------------------------------------------------------------------------------------

void main()
{
	gl_Position = vec4(vertex_position * scale + translation, 0.0, 1.0);
	color = vertex_color / 255.0;
	uv = vertex_uv;
}
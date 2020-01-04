precision mediump float;

layout(location = 0) out vec3 color;

//----------------------------------------------------------------------------------------------------------------------

void main()
{
	vec2 vertex_position[3] = vec2[3](vec2(-0.7, -0.7), vec2(0.7, -0.7), vec2(0.0, 0.7));
	vec3 vertex_color[3] = vec3[3](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));

	gl_Position = vec4(vertex_position[gl_VertexIndex], 0.0, 1.0);
	color = vertex_color[gl_VertexIndex];
}

//----------------------------------------------------------------------------------------------------------------------
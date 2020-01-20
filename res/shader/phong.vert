layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_tex_coord;


layout(location = 0) out vec3 light_intensity;

layout(binding = 0) uniform Matrix {
    mat4 model;
    mat4 normal;
    mat4 view;
    mat4 projection;
    mat4 mv;
    mat4 mvp;
} matrix;

layout(binding = 1) uniform Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} light;

layout(binding = 2) uniform Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

void main()
{
    gl_Position = matrix.mvp * vec4(vertex_position, 1.0);

    vec3 ambient = light.ambient * material.ambient;

    vec4 cam_coords = matrix.mv * vec4(vertex_position, 1.0);
    mat4 nv = matrix.view * matrix.normal;
    vec3 n = normalize(nv * vec4(vertex_normal, 1.0)).xyz;
    vec3 s = normalize(nv * vec4(light.position, 1.0) - cam_coords).xyz;
    float s_dot_n = max(dot(s, n), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * s_dot_n;

    vec3 specular = vec3(0.0);

    if (s_dot_n > 0.0) {
        vec3 v = normalize(-cam_coords.xyz);
        vec3 r = reflect(-s, n);
        specular = light.specular * material.specular * pow(max(dot(r, v), 0.0), material.shininess);
    }

    light_intensity = ambient + diffuse + specular;
}
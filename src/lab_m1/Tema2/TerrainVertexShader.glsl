#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float frequency;

out vec3 frag_normal;
out vec3 frag_position;
out vec2 frag_texcoord;

void main()
{
    vec3 pos = a_position;

    // Apply procedural noise for terrain deformation
    float amplitude = 1.0;
    float noise = sin(pos.x * frequency) * cos(pos.z * frequency) * amplitude;
    pos.y += noise;

    // Generate texture coordinates based on position
    frag_texcoord = vec2(pos.x * 0.1, pos.z * 0.1);

    // Transform position to world space
    frag_position = vec3(Model * vec4(pos, 1.0));
    
    // Transform and normalize normal vector
    frag_normal = normalize(mat3(transpose(inverse(Model))) * a_normal);

    gl_Position = Projection * View * vec4(frag_position, 1.0);
}

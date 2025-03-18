#version 330 core

in vec3 frag_normal;
in vec3 frag_position;
in vec2 frag_texcoord;

uniform vec3 terrain_color_low;
uniform vec3 terrain_color_high;
uniform vec3 light_position;
uniform float max_altitude;
uniform float drone_altitude;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(frag_normal);

    vec3 lightDir = normalize(light_position - frag_position);

    // Calculate diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);

    // Ambient lighting component
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * terrain_color_low;

    // Default height-based color interpolation
    float minHeight = -5.0;
    float maxHeight = 5.0;
    float heightFactor = clamp((frag_position.y - minHeight) / (maxHeight - minHeight), 0.0, 1.0);
    vec3 terrainColor = mix(terrain_color_low, terrain_color_high, heightFactor);

    // Smooth transition based on drone altitude
    float transitionStart = 10.0;
    float transitionEnd = 15.0;
    float altitudeFactor = smoothstep(transitionStart, transitionEnd, drone_altitude);

    vec3 altitudeColor = mix(terrain_color_low, terrain_color_high, altitudeFactor);

    // Blend altitude-based color with height-based color
    terrainColor = mix(terrainColor, altitudeColor, altitudeFactor);

    // Simulate texture pattern for additional detail
    float texturePattern = sin(frag_texcoord.x * 10.0) * cos(frag_texcoord.y * 10.0);
    terrainColor *= (0.9 + 0.1 * texturePattern);

    vec3 finalColor = ambient + diff * terrainColor;
    FragColor = vec4(finalColor, 1.0);
}

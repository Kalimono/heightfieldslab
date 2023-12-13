#version 420
///////////////////////////////////////////////////////////////////////////////
// Input vertex attributes
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoordIn;

///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////
uniform mat4 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform float displaceNormal;
uniform sampler2D hf;

///////////////////////////////////////////////////////////////////////////////
// Output to fragment shader
///////////////////////////////////////////////////////////////////////////////
out vec2 texCoord;
out vec3 viewSpacePosition;
out vec3 viewSpaceNormal;

vec4 computeNormal()
{
    float off = 0.01f;
    float hX = texture(hf, texCoordIn + vec2(off, 0)).r;
    float hZ = texture(hf, texCoordIn + vec2(0, off)).r;
    float currentHeight = 3.0 * texture(hf, texCoordIn).r;

    vec3 slopeX = vec3(position.x + off, hX, position.z) - vec3(position.x, currentHeight, position.z);
    vec3 slopeZ = vec3(position.x, hZ, position.z + off) - vec3(position.x, currentHeight, position.z);

    vec3 normal = cross(normalize(slopeX), normalize(slopeZ));
    return -normalize(vec4(normal, 0));
}

void main() 
{
    viewSpaceNormal = (normalMatrix * computeNormal()).xyz;
    viewSpacePosition = (modelViewMatrix * vec4(position, 1.0)).xyz;
    
    float height = texture(hf, texCoordIn).r * 3.0;
    gl_Position = modelViewProjectionMatrix * vec4(position.x, height, position.z, 1.0) + normalize(vec4(viewSpaceNormal, 0.0)) * displaceNormal;
    
    texCoord = texCoordIn;
}

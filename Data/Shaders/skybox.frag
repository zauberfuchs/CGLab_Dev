#version 330 core

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

///////////////////////////////////////////////////////////////////////////////
// Input varyings from vertex shader
///////////////////////////////////////////////////////////////////////////////
in vec3 TexCoords;
in vec3 WorldPos;
///////////////////////////////////////////////////////////////////////////////
// Output color
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec4 FragColor;

///////////////////////////////////////////////////////////////////////////////
// Skybox Texture
///////////////////////////////////////////////////////////////////////////////
uniform samplerCube skybox;



void main()
{    
//    FragColor = texture(skybox, vec4(TexCoords, 1)); // cubemap Array call not used
//    FragColor = texture(skybox, TexCoords);

    vec3 envColor = textureLod(skybox, WorldPos, 0.0).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColor, 1.0);
}
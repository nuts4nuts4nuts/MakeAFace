#version 440
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D trueTexture;
uniform sampler2D referenceTexture;
uniform sampler2D testTexture;
uniform vec2 iResolution;

out vec4 fragColor;

void main()
{	
	vec2 fragCoord = gl_FragCoord.xy;
    vec2 imageUV  = fragCoord.xy / iResolution.xy;
    
    vec4 trueColor = texture( trueTexture, imageUV );
    vec4 refColor = texture( referenceTexture, imageUV );
	vec4 testColor = texture( testTexture, imageUV );

    if(abs(length(trueColor - testColor)) < abs(length(trueColor - refColor)))
    {
        fragColor = testColor;
    }
	else
    {
        fragColor = refColor;
    }
}
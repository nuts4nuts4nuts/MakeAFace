#version 400

uniform vec2 iResolution;
uniform sampler2D renderedTexture;

out vec4 fragColor;

void main()
{
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 uv = fragCoord.xy / iResolution.xy;
	
    //Flip the u so that the final image is mirrored
    uv = vec2(1.0 - uv.x, uv.y);
    
	fragColor = texture( renderedTexture, uv );
}
#version 400

uniform sampler2D trueTexture;
uniform sampler2D previousTexture;
uniform vec2 iResolution;
uniform float iTime;

uniform bool useTriangles;
uniform bool everyPixelSameColor;
uniform bool sourceColors;

out vec4 fragColor;

//Randomness code from Martin, here: https://www.shadertoy.com/view/XlfGDS
float Random_Final(vec2 uv, float seed)
{
    float fixedSeed = abs(seed) + 1.0;
	float x = dot(uv, vec2(12.9898,78.233) * fixedSeed);
	return fract(sin(x) * 43758.5453);
}

//Test if a point is in a triangle
bool pointInTriangle(vec2 triPoint1, vec2 triPoint2, vec2 triPoint3, vec2 testPoint)
{
	float denominator = ((triPoint2.y - triPoint3.y)*(triPoint1.x - triPoint3.x) + (triPoint3.x - triPoint2.x)*(triPoint1.y - triPoint3.y));
	float a = ((triPoint2.y - triPoint3.y)*(testPoint.x - triPoint3.x) + (triPoint3.x - triPoint2.x)*(testPoint.y - triPoint3.y)) / denominator;
	float b = ((triPoint3.y - triPoint1.y)*(testPoint.x - triPoint3.x) + (triPoint1.x - triPoint3.x)*(testPoint.y - triPoint3.y)) / denominator;
	float c = 1.0 - a - b;
 
	return 0.0 <= a && a <= 1.0 && 0.0 <= b && b <= 1.0 && 0.0 <= c && c <= 1.0;
}

void main()
{	
	vec2 fragCoord = gl_FragCoord.xy;
    vec2 imageUV  = fragCoord.xy / iResolution.xy;
    vec2 testUV = imageUV;

	if(everyPixelSameColor)
	{
	    testUV = vec2(1.0, 1.0); 
	}

    vec2 triPoint1 = vec2(Random_Final(testUV, iTime), Random_Final(testUV, iTime * 2.0));
    vec2 triPoint2 = vec2(Random_Final(testUV, iTime * 3.0), Random_Final(testUV, iTime * 4.0));
    vec2 triPoint3 = vec2(Random_Final(testUV, iTime * 5.0), Random_Final(testUV, iTime * 6.0));

    vec4 testColor = vec4(Random_Final(testUV, iTime * 10.0),
                          Random_Final(testUV, iTime * 11.0),
                          Random_Final(testUV, iTime * 12.0),
                          1.0);

	if(sourceColors)
	{
		vec2 colorUV = vec2(Random_Final(testUV, iTime * 10.0),
							Random_Final(testUV, iTime * 11.0));

		testColor = texture( trueTexture, colorUV );
	}
    
    vec4 trueColor = texture( trueTexture, imageUV );
    vec4 prevColor = texture( previousTexture, imageUV );

    fragColor = prevColor;

    bool isInTriangle = true;

	if(useTriangles)
	{
		isInTriangle = pointInTriangle(triPoint1, triPoint2, triPoint3, imageUV);
	}

    if(isInTriangle && abs(length(trueColor - testColor)) < abs(length(trueColor - prevColor)))
    {
        fragColor = testColor;
    }
}
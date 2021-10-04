#version 330 core
out vec4 FragColor;

in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;
in vec2 TexCoord;

// texture samplers
uniform sampler2D ColorTexture;
uniform sampler2D NormalTexture;
uniform sampler2D AOTexture;

// blend settings
uniform vec2 BlendCenter;
uniform float BlendInnerRadius;
uniform float BlendOuterRadius;

// shader values
vec3 FragBaseColor;
vec3 FragNormal;
float FragAO;

void ShaderMain();

// common functions

// Converts a color from linear light gamma to sRGB gamma
vec3 linear_to_srgb(vec3 linearRGB)
	{
    bvec3 cutoff = lessThan(linearRGB, vec3(0.0031308));
    vec3 higher = vec3(1.055)*pow(linearRGB, vec3(1.0/2.4)) - vec3(0.055);
    vec3 lower = linearRGB * vec3(12.92);
    return mix(higher, lower, cutoff);
	}

// Converts a color from sRGB gamma to linear light gamma
vec3 srgb_to_linear(vec3 sRGB)
	{
    bvec3 cutoff = lessThan(sRGB, vec3(0.04045));
    vec3 higher = pow((sRGB + vec3(0.055))/vec3(1.055), vec3(2.4));
    vec3 lower = sRGB/vec3(12.92);
    return mix(higher, lower, cutoff);
	}

float srgb_to_linear(float sRGB)
	{
    bool cutoff = sRGB < float(0.04045);
    float higher = pow((sRGB + float(0.055))/float(1.055), float(2.4));
    float lower = sRGB/float(12.92);
    return mix(higher, lower, cutoff);
	}

float GetBlendValue()
	{
	return 1.0 - smoothstep( BlendInnerRadius , BlendOuterRadius , length(TexCoord - BlendCenter) );
	}

void main()
	{
	FragBaseColor = srgb_to_linear(vec3(texture(ColorTexture, TexCoord)));
	
	vec3 Normal = normalize(vNormal);
	vec3 Tangent = normalize(vTangent);
	vec3 Bitangent = normalize(vBitangent);

	vec3 ntex = (vec3(texture(NormalTexture,TexCoord)) * 2.0) - 1.0;

	FragNormal = (ntex.x * Tangent) +  
				 (ntex.y * Bitangent) +
				 (ntex.z * Normal); 
	
	FragAO = srgb_to_linear(texture(AOTexture,TexCoord).r);

	ShaderMain();
	
	FragColor.a = GetBlendValue();	
	}

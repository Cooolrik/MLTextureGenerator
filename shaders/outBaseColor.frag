void ShaderMain()
	{
	FragColor.rgb = linear_to_srgb(vec3( FragBaseColor ));
	}
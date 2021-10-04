void ShaderMain()
	{
	float light = dot(FragNormal,vec3(0.707,0.707,-0.207)) + 0.5;
	if( light > 1.0 )
		light = 1.0;

	FragColor.rgb = linear_to_srgb(FragBaseColor * light) * ((FragAO * 0.7) + 0.3);
	}
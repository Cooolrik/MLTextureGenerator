void ShaderMain()
	{
	// output the transformed normal xy in rg
	vec3 n;
	n.x = dot(FragNormal,vec3(1,0,0)); 
	n.y = dot(FragNormal,vec3(0,1,0)); 
	n.z = dot(FragNormal,vec3(0,0,-1)); 
	n = normalize(n);
	FragColor.rg = vec3( ((n+1)/2) ).rg;

	// output the AO in b
	FragColor.b = FragAO;
	}
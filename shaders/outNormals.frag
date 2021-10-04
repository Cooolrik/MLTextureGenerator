void ShaderMain()
	{
	vec3 n;
	n.x = dot(FragNormal,vec3(1,0,0)); 
	n.y = dot(FragNormal,vec3(0,1,0)); 
	n.z = dot(FragNormal,vec3(0,0,-1)); 
	FragColor.rgb = vec3( ((n+1)/2) );
	}
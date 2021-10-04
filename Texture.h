#pragma once

class Texture
	{
	public:
		unsigned int ID;
		unsigned int Width;
		unsigned int Height; 

		static Texture* Load(const char* path);
		~Texture();
	};


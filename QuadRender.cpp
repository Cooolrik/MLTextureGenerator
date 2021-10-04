
#include <glad/gl.h>
#include <glm/glm.hpp>

#include "QuadRender.h"

struct Vertex
	{
	glm::vec3 Coord;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	glm::vec2 TexCoord;
	};

static Vertex vertices[] =
	{
	// Coord			  // Normal         // Tangent        // Bitangent      // TexCoord
	{ glm::vec3(2,2,0),   glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec2(2,2)   }, // top right
	{ glm::vec3(2,-2,0),  glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec2(2,-2)  }, // bottom right
	{ glm::vec3(-2,-2,0), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec2(-2,-2) }, // bottom left
	{ glm::vec3(-2,2,0),  glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec2(-2,2)  }  // top left 
	};

static unsigned int indices[] = 
	{
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
	};

QuadRender* QuadRender::Create()
	{
	QuadRender* qr = new QuadRender();

	glGenVertexArrays(1, &qr->VAO);
	glGenBuffers(1, &qr->VBO);
	glGenBuffers(1, &qr->EBO);

	glBindVertexArray(qr->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, qr->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qr->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Coords attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Tangent attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Bitangent attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// TexCoord attribute
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(12 * sizeof(float)));
	glEnableVertexAttribArray(4);

	return qr;
	}

void QuadRender::Render()
	{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

QuadRender::~QuadRender()
	{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
	}

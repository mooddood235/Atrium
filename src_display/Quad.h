#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Quad{
public:
	Quad();
	void Draw();
private:
	unsigned int vao;
	unsigned int vbo;
    unsigned int ebo;
private:
    float vertices[12] = {
     1.0f,  1.0f, 0.0f,  // top right
     1.0f, -1.0f, 0.0f,  // bottom right
    -1.0f, -1.0f, 0.0f,  // bottom left
    -1.0f,  1.0f, 0.0f   // top left 
    };
    unsigned int indices[6] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

};


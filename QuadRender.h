#pragma once

class QuadRender
    {
    private:
        QuadRender() {};

    public:
        unsigned int VBO = 0;
        unsigned int VAO = 0;
        unsigned int EBO = 0;

        static QuadRender* Create();

        void Render();
                
        ~QuadRender();
    };


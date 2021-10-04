#pragma once
class RenderFramebuffer
    {
    private:

    public:
        unsigned int Width = 0;
        unsigned int Height = 0;
        unsigned int FramebufferID = 0;
        unsigned int ColorTexturebufferID = 0;
        unsigned int DepthRenderbufferID = 0;

        static RenderFramebuffer* Create(unsigned int width, unsigned int height);

        void BindAsDestination();

        ~RenderFramebuffer();
    };


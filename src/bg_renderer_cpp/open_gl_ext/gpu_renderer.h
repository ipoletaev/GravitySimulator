#pragma once

#include "renderer.h"

namespace background
{
    class GPURenderer : public Renderer
    {
        
    protected:
        
        bg_uint gl_window;
        float aspect_ratio = 1.0;
        
        bg_uint buffer;
        bg_uint texture;
        
        bool is_texture_initialized;
        
    public:
        
        GPURenderer() {};
        
        virtual ~GPURenderer();
                    
        void generte_texture();
        
        void init_open_gl();
        
        void transform_point_coords(float&, float&);
                
    };


    class FrameBuffer : public GPURenderer
    {
        
    public:
        
        FrameBuffer() {};
        
        virtual ~FrameBuffer();
        
        int init_background();
        
        void draw_a_star(bg_cuint, bg_cuint, const uint8_t);
        
        bg_cint8_ptr get_background() const;
        
        std::string get_name() const;
    };

    
    class PixelBuffer : public GPURenderer
    {
        
    public:
        
        PixelBuffer() {};
        
        virtual ~PixelBuffer();
        
        int init_background();
        
        void draw_a_star(bg_cuint, bg_cuint, const uint8_t);
        
        bg_cint8_ptr get_background() const;

        std::string get_name() const;
    };
}



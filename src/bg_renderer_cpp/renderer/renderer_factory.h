#pragma once

#include "cpu_renderer.h"
#include "gpu_renderer.h"
#include "gl_info.h"

namespace background
{
    class RendererFactory
    {

    private:
        
        
        bool pbo_support = false;
        bool fbo_support = false;

    public:
        
        RendererFactory()
        {
            GLInfo open_gl_info;
            open_gl_info.get_info();
            
            // use pixel buffer object
            if(open_gl_info.is_extension_supported("GL_ARB_pixel_buffer_object")) pbo_support = true;
            
            // use frame buffer object
            if(open_gl_info.is_extension_supported("GL_ARB_framebuffer_object")) fbo_support = true;

            show_info();
        };
        
        RendererFactory(bool is_pbo_supported, bool is_fbo_supported)
        {
            // use pixel buffer object
            pbo_support = is_pbo_supported;
            
            // use frame buffer object
            fbo_support = is_fbo_supported;
            
            show_info();
        };
        
        ~RendererFactory() {};
        
        void show_info()
        {
            if(!pbo_support && !fbo_support)
            {
                // use CPU renderer
                Renderer::print_msg("Your video card doesn't support FBO/PBO. Use CPU renderer");
            }
            else
            {
                Renderer::print_msg("Your video card supports FBO/PBO. Use GPU renderer");
            }
        }
        
        Renderer* create_empty_renderer() const
        {
            if (pbo_support && fbo_support)
            {
                return new PixelBuffer();
            }
            else if (pbo_support && !fbo_support)
            {
                return new PixelBuffer();
            }
            else if (!pbo_support && fbo_support)
            {
                return new FrameBuffer();
            }
            else
            {
                return new CPURenderer();
            }
        }
        
        Renderer* create_renderer(bg_cuint img_w, bg_cuint img_h) const
        {
            Renderer* renderer = create_empty_renderer();
            renderer->set_paramaters(img_w, img_h);
            return renderer;
        }
    };

}

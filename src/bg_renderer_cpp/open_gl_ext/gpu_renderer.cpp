#include "gpu_renderer.h"

#include <GLUT/glut.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <string>

const GLenum PIXEL_FORMAT = GL_RGBA;

void empty_display_for_gpu_renderer(void) {};  // A little trick to be able to use multiple windows in different threads

namespace background
{
    bg_uint DEFAULT_SECTORS_AMOUNT = 50;
    
    // -*- common methods -*- //
    GPURenderer::~GPURenderer()
    {
        if (is_texture_initialized)
        {
            glDeleteTextures(1, &texture);
        }
        
        glutDestroyWindow(gl_window);
    };
    
    
    void GPURenderer::generte_texture()
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexImage2D(GL_TEXTURE_2D, 0, PIXEL_FORMAT, img_w, img_h, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, 0);
        is_texture_initialized = true;
    }
    
    
    void GPURenderer::init_open_gl()
    {
        // -*- Open GL initialization -*- //
        glutInitWindowSize(img_w, img_h);
        glutCreateWindow("BackgroundRenderer");
        gl_window = glutGetWindow();
        glutDisplayFunc(empty_display_for_gpu_renderer);
        
        // clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        aspect_ratio = (float) img_w / img_h;
        gluOrtho2D(-1 * aspect_ratio, aspect_ratio, -1, 1);
        
        //glShadeModel(GL_SMOOTH);          // shading mathod: GL_SMOOTH or GL_FLAT
        GLfloat brightness_f = bg_default_value / (float)UINT8_MAX;
        glClearColor(brightness_f, brightness_f, brightness_f, 1.0);
    };
    
    
    void GPURenderer::transform_point_coords(float& y, float& x)
    {
        y = (2.0 * y - img_h) / (float)(img_h);
        x = aspect_ratio * (2.0 * x - img_w) / (float)(img_w);
    }
    // -*-*-*-*-*-*-*-*-*-*- //
    
    
    // -*- frame buffer -*- //
    FrameBuffer::~FrameBuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &buffer);
    }
    
    
    int FrameBuffer::init_background()
    {
        init_open_gl();
        allocate_memory_for_image();     // buffer on RAM
        
        // allocate GPU memory for background
        try
        {
            generte_texture();  //generate empty texture
            
            //Generate a framebuffer object(FBO) and bind it to the pipeline
            glGenFramebuffers(1, &buffer);
            glBindFramebuffer(GL_FRAMEBUFFER, buffer);
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
            
            //Check for FBO completeness
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                throw background::BGexception("FrameBuffer is not complete!");
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << " <FrameBuffer initialization>" << std::endl;
            return -1;
        }
        
        return 0;
    }
    
    
    void FrameBuffer::draw_a_star(bg_cuint y, bg_cuint x, const uint8_t brightness)
    {
        GLfloat brightness_f = brightness / (float)UINT8_MAX;
        GLfloat angle(0), y_0(y), x_0(x), r = (2.0 * star_size) / (float)(img_w);
        
        transform_point_coords(y_0, x_0);
        
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(brightness_f, brightness_f, brightness_f);
        glVertex2f(x_0, y_0);
        glColor3f(0, 0, 0);
        for (int i = 0; i <= DEFAULT_SECTORS_AMOUNT; i++)
        {
            angle = i * 2.0f * M_PI / DEFAULT_SECTORS_AMOUNT;
            glVertex2f(x_0 + cos(angle) * r, y_0 + sin(angle) * r);
        }
        
        glEnd();
    };
    
    
    bg_cint8_ptr FrameBuffer::get_background() const
    {
        glReadPixels(0, 0, img_w, img_h, PIXEL_FORMAT, GL_UNSIGNED_BYTE, bg_img.data);
        return bg_img.data;
    };
    
    
    std::string FrameBuffer::get_name() const
    {
        return "GPU-Renderer-Frame-Buffer-Object";
    }
    // -*-*-*-*-*-*-*-*-*-*- //
    
    
    // -*- pixel buffer -*- //
    PixelBuffer::~PixelBuffer()
    {
        glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
        glDeleteBuffersARB(1, &buffer);
    }
    
    
    int PixelBuffer::init_background()
    {
        init_open_gl();
        bg_img.set_sizes(img_w, img_h);

        // allocate GPU memory for background and create the DMA to the GPU
        try
        {
            generte_texture();
            glGenBuffersARB(1, &buffer);
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, buffer);
            glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, bg_img.data_size, 0, GL_DYNAMIC_DRAW_ARB);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img_w, img_h, PIXEL_FORMAT, GL_UNSIGNED_BYTE, 0);
            
            //example: http://www.songho.ca/opengl/gl_pbo.html
            glBufferData(GL_PIXEL_UNPACK_BUFFER, bg_img.data_size, 0, GL_DYNAMIC_DRAW_ARB);
            bg_img.data = (uint8_t*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_WRITE);
            if (!bg_img.data)
            {
                throw BGexception("Error while getting the direct access to the GPU memory!");
            }
            else
            {
                for (unsigned long int i = 0; i < bg_img.data_size; i++) bg_img.data[i] = bg_default_value;
            }
            
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << " <PixelBuffer initialization>" << std::endl;
            return -1;
        }
        
        return 0;
    }
    
    
    void PixelBuffer::draw_a_star(bg_cuint y, bg_cuint x, const uint8_t brightness)
    {
        // draw a star in shape of circle with symmetric brightness decreasing from its center to the border
        float dist;
        for (bg_uint row = y - star_size; row < y + star_size; row++)
        {
            for (bg_uint col = x - star_size; col < x + star_size; col++)
            {
                dist = sqrtf((row - y) * (row - y) + (col - x) * (col - x));
                if (dist < star_size)
                {
                    bg_img.at(row, col) = brightness * (1. - dist / star_size); // b(r) = max_b * (1 - r / max_r)
                }
            }
        }
    };
    
    
    bg_cint8_ptr PixelBuffer::get_background() const
    {
        return bg_img.data;
    };
    
    std::string PixelBuffer::get_name() const
    {
        return "GPU-Renderer-Pixel-Buffer-Object";
    }
    // -*-*-*-*-*-*-*-*-*-*- //
}

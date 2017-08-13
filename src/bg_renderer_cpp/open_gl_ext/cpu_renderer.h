#pragma once

#include "renderer.h"

namespace background
{
    class CPURenderer : public Renderer
    {
        
    // implementation of abstract interface methods
        
    public:
        
        CPURenderer() {};
        
        virtual ~CPURenderer() {};
        
        // -*- renderer methods -*- //
        
        int init_background();
        
        bg_cint8_ptr get_background() const;
        
        void draw_a_star(bg_cuint, bg_cuint, const uint8_t);
        
        std::string get_name() const;
    };
}

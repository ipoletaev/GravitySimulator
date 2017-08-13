#include "cpu_renderer.h"

namespace background
{
    int CPURenderer::init_background()
    {
        return allocate_memory_for_image();
    }
    
    
    bg_cint8_ptr CPURenderer::get_background() const
    {
        return bg_img.data;
    }
    
    std::string CPURenderer::get_name() const
    {
        return "CPU-Renderer";
    }
    
    void CPURenderer::draw_a_star(bg_cuint y, bg_cuint x, const uint8_t brightness)
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
    }
}

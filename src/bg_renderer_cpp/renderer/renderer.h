#pragma once

#include "utilities.h"

namespace background
{
    class Renderer
    {
        
    protected:
        
        bg_uint img_w, img_h, star_size;
        uint8_t bg_default_value;
        
        RGB_2DImage bg_img;
        bool use_random_star_choice = false;
    
    private:
        
        bg_uint stars_number;
        float period_of_flickering;
        std::vector<std::tuple<bg_uint, bg_uint, bg_uint, float>> stars;
        
    public:
        
        // -*- con- & destructors -*- //
        
        Renderer() {};
                
        virtual ~Renderer() {};
 
        void set_paramaters(bg_cuint, bg_cuint);
        
        void initialize();
        
        int allocate_memory_for_image();
        
        void save_img(std::string);
        
        static void print_msg(std::string);
                
        // -*- star drawing methods -*- //
        
        inline float time_function(const float);
        
        void generate_stars_centers();
        
        // -*- get methods -*- //
        
        int get_height() const { return img_h; };
        
        int get_width() const { return img_w; };

        // -*- function for usage in .py code -*- //
        
        void update_background(const float);
        
        // -*- pure virtual methods -*- //
        
        virtual int init_background() = 0;
        
        virtual bg_cint8_ptr get_background() const = 0; // read only
        
        virtual void draw_a_star(bg_cuint, bg_cuint, const uint8_t) = 0;
        
        virtual std::string get_name() const = 0;
    };
    
}

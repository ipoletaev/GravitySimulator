#include "renderer.h"

namespace background
{
    
    uint8_t DEFAULT_BG_VAL = 0;
    bg_uint DEAFAULT_STAR_SIZE = 4;
    float DEAFAULT_P_OF_FLIK = 7.0;
    
    
    void Renderer::set_paramaters(bg_cuint w, bg_cuint h)
    {
        img_w = w;
        img_h = h;
        initialize();
    };
    
    int Renderer::allocate_memory_for_image()
    {
        try
        {
            bg_img.initialize(img_w, img_h, bg_default_value);
        }
        catch (const std::bad_alloc& ba)
        {
            std::cerr << ba.what() << "<Memory allocation error>" << std::endl;
            return -1;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << "<Initialization exception>" << std::endl;
            return -1;
        }
        
        return 0;
    }
    
    void Renderer::print_msg(std::string msg)
    {
        std::cout << "Renderer: \033[1;33m" + msg + "\033[0m" << std::endl;
    }
    
    void Renderer::initialize()
    {
        
        if (bg_img.is_initialized) throw BGexception("Generator has been already initialized!");
        
        
        // mostly constant parameters
        
        stars_number = (bg_uint)(img_w * img_h / 350.);
        star_size = DEAFAULT_STAR_SIZE;
        bg_default_value = DEFAULT_BG_VAL;
        period_of_flickering = DEAFAULT_P_OF_FLIK;
        
        
        // check
        
        if (bg_default_value > UINT8_MAX)                               throw BGexception("Max bg. brightness value is 255!");
        if (period_of_flickering == 0)                                  throw BGexception("Flickering period must be greater than 0!");
        if (star_size == 0)                                             throw BGexception("Star size must be greater than 0!");
        if ((star_size * 2 + 1 > img_w) || (star_size * 2 + 1 > img_h)) throw BGexception("Too large star size!");
        if ((img_w < star_size) || (img_w > INT_MAX))                   throw BGexception("Bad image width value!");
        if ((img_h < star_size) || (img_h > INT_MAX))                   throw BGexception("Bad image height value!");
        
        // end check
        
        if (init_background() == -1)                                    throw BGexception("Memory allocation error!");
        generate_stars_centers();                                       // init stars positions and brighnesses
        update_background(0);                                           // draw initial background
        
        
        print_msg("Sky generator has been successfully initialized");
        print_msg("Image size = (" + std::to_string(img_w) + "," + std::to_string(img_h)+ ")");
        print_msg("Flickering time = " + std::to_string(period_of_flickering) + " second(s)");
    }
    
    
    inline float Renderer::time_function(const float time)
    {
        return (0.5 + sin(time) * sin(time)) / 1.5;
    }
    
    
    void Renderer::generate_stars_centers()
    {
        std::vector<std::tuple<bg_uint, bg_uint, bg_uint, float>> initial_array_of_stars;
        initial_array_of_stars.resize(stars_number); //allocate memory in advance
        bg_uint y, x;
        uint8_t start_brightness;
        float start_phase_shift;
        for (int i = 0; i < stars_number; i++)
        {
            y =                 random_T<bg_uint>(star_size + 1, img_h - star_size - 1);
            x =                 random_T<bg_uint>(star_size + 1, img_w - star_size - 1);
            start_brightness  = random_T<bg_uint>(64, UINT8_MAX);
            start_phase_shift = random_T<float>(0, M_PI);
            
            initial_array_of_stars[i] = std::make_tuple(y, x, start_brightness, start_phase_shift);
        }
        
        // delete too close stars
        bg_uint y_0, x_0, y_1, x_1;
        std::vector<std::tuple<bg_uint, bg_uint, bg_uint, float>> filtered_stars;
        for (int i = 0; i < stars_number; i++)
        {
            bool appropriate_star = true;
            for (int j = 0; (j < stars_number) && (j != i); j++)
            {
                y_0 = std::get<0>(initial_array_of_stars[i]);
                x_0 = std::get<1>(initial_array_of_stars[i]);
                y_1 = std::get<0>(initial_array_of_stars[j]);
                x_1 = std::get<1>(initial_array_of_stars[j]);
                if (sqrtf((y_0 - y_1) * (y_0 - y_1) + (x_0 - x_1) * (x_0 - x_1)) < (star_size + 2))
                {
                    appropriate_star = false; // too close points
                }
            }
            
            if (appropriate_star)
            {
                stars.push_back(initial_array_of_stars[i]);
            }
        }
        
        stars_number = (bg_uint)stars.size();
    }
    
    
    void Renderer::update_background(const float time)
    {
        // every star's brightness is oscillating between respective min/max
        // brightness values during the 'period of flickering'
        if (time > MAXFLOAT) throw BGexception("Too large time argument!");
        
        uint8_t brightness;
        for (int i = 0; i < stars_number; i++)
        {
            if ((!use_random_star_choice) || (rand() % 2)) // flickering is enabled only for some random set of stars
            {
                auto star = stars[i];
                brightness = std::get<2>(star) * time_function(std::get<3>(star) + time * M_PI / period_of_flickering);
                draw_a_star(std::get<0>(star), std::get<1>(star), (brightness > UINT8_MAX ? UINT8_MAX : brightness));
            }
        }
    }
    
    
    void Renderer::save_img(std::string filename)
    {
        get_background();
        std::string path = filename + ".ppm";
        FILE *f = fopen(path.c_str(), "wb");
        fprintf(f, "P6\n%i %i 255\n", img_w, img_h);

        int val = 0;
        for (bg_uint row = 0; row < img_h; row++)
        {
            for (bg_uint col = 0; col < img_w; col++)
            {
                val = (int)bg_img.at(row, col);
                fputc(val, f);
                fputc(val, f);
                fputc(val, f);
            }
        }
        fclose(f);
    }

}

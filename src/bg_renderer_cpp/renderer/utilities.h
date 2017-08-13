#pragma once

#include <vector>
#include <math.h>
#include <string>
#include <iostream>

namespace background
{
    
    typedef unsigned int bg_uint;
    typedef const unsigned int bg_cuint;
    typedef const uint8_t* const bg_cint8_ptr;
    
    struct BGexception : public std::exception
    {
        
    private:
        
        std::string msg;
        
    public:
        
        BGexception() : msg("") {};
        
        BGexception(std::string str) : msg("Renderer: \033[0;31mBackground exception\033[0m: " + str) {};
        
        BGexception (const BGexception& another_exception)
        {
            msg = ""; *this = another_exception;
        }
        
        BGexception& operator = (const BGexception& another_exception)
        {
            if (this == &another_exception) return *this;
            this->msg = another_exception.msg;
            return *this;
        }
        
        ~BGexception() throw () {}
        
        const char* what() const throw() { return msg.c_str(); }
    };
    
    
    class RGB_2DImage
    {
    public:
        
        uint8_t* data;
        bool is_initialized = false;
        bg_uint img_w, img_h;
        unsigned long int data_size;

        // -*- -*- //
        
        RGB_2DImage() : data(NULL), img_w(0), img_h(0) {};
        
        RGB_2DImage(bg_uint w, bg_uint h, const uint8_t bg_default_value)
        {
            initialize(w, h, bg_default_value);
        }
        
        void initialize(bg_uint w, bg_uint h, const uint8_t bg_default_value)
        {
            set_sizes(w, h);
            data = new uint8_t[data_size];
            for (int i = 0; i < data_size; i++) data[i] = bg_default_value;
            is_initialized = true;
        }
        
        void set_sizes(bg_uint w, bg_uint h) { img_w = w; img_h = h; data_size = 4 * img_w * img_h; };
        
        ~RGB_2DImage()
        {
            std::cout << "Free the renderer memory..." << std::endl;
            destroy();
        }
        
        void destroy()
        {
            if (is_initialized)
            {
                delete [] data;
            }
        }
        
        uint8_t& at(bg_uint row, bg_uint col) { return data[(row * img_w + col) * 4]; };
        
    };
    
    
    template<typename T>
    T random_T(T min, T max)
    {
        return min + static_cast<T>(rand()) / (static_cast<T>(RAND_MAX/(max-min)));
    }

}

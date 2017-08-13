#pragma once

#include "renderer_factory.h"

using namespace std::chrono;

background::bg_cuint TEST_WINDOW_SIZE = 300;
background::bg_cuint RECT_SHIFT = 150;
background::bg_uint width = TEST_WINDOW_SIZE + RECT_SHIFT;
background::bg_uint height = TEST_WINDOW_SIZE - RECT_SHIFT; // 640x480 for test


float timer(void (*func)(void))
{
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    func();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    
    return (float)(duration_cast<microseconds>(t2 - t1).count()) / CLOCKS_PER_SEC;
}


void test_of_image_generation_with_pbo_renderer(void)
{
    background::RendererFactory factory(true, false);
    background::Renderer* generator = factory.create_renderer(width, height);
    
    for (float t = 0.01; t < 1.0; t += 0.05) generator->update_background(t);

    //generator->save_img("test_img_pbo");
    delete generator;
}


void test_of_image_generation_with_fbo_renderer(void)
{
    background::RendererFactory factory(false, true);
    background::Renderer* generator = factory.create_renderer(width, height);
    
    for (float t = 0.01; t < 1.0; t += 0.05) generator->update_background(t);

    //generator->save_img("test_img_fbo");
    delete generator;
}


void test_of_image_generation_with_cpu_renderer(void)
{
    background::RendererFactory factory(false, false);
    background::Renderer* generator = factory.create_renderer(width, height);
    
    for (float t = 0.01; t < 1.0; t += 0.05) generator->update_background(t);
    
    //generator->save_img("test_img_cpu");
    delete generator;
}


std::vector<std::pair<background::BGexception, std::string>> test(void)
{
    float pbo_test_time(0), fbo_test_time(0), cpu_test_time(0);
    std::vector<std::pair<background::BGexception, std::string>>  errors;
    
    //
    std::cout << "-*-*-*-*-*-* PBO test *-*-*-*-*-*-" << std::endl;
    try
    {
        pbo_test_time = timer(test_of_image_generation_with_pbo_renderer);
        std::cout << "PBO rendering time: " << pbo_test_time << " sec." << std::endl;
    }
    catch (const background::BGexception& e)
    {
        errors.push_back(std::make_pair(e, "Pixel buffer traceback. "));
    }
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-" << std::endl << std::endl;
    
    //
    std::cout << "-*-*-*-*-*-* FBO test *-*-*-*-*-*-" << std::endl;
    try
    {
        fbo_test_time = timer(test_of_image_generation_with_fbo_renderer);
        std::cout << "FBO rendering time: " << fbo_test_time << " sec." << std::endl;
    }
    catch (const background::BGexception& e)
    {
        errors.push_back(std::make_pair(e, "Frame buffer traceback. "));
    }
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-" << std::endl << std::endl;

    //
    std::cout << "-*-*-* CPU renderer test *-*-*-*-" << std::endl;
    try
    {
        cpu_test_time = timer(test_of_image_generation_with_cpu_renderer);
        std::cout << "CPU rendering time: " << cpu_test_time << " sec." << std::endl;
    }
    catch (const background::BGexception& e)
    {
        errors.push_back(std::make_pair(e, "CPU renderer traceback. "));
    }
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-" << std::endl << std::endl;
    
    return errors;
}

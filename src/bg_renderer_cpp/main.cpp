#include "main.h"
#include "gpu_renderer.h"
#include <unistd.h>

// Test background generation


int main(int argc, char *argv[])
{
    
    srand(time(NULL));
    auto errors = test();

    if (errors.size())
    {
        for (const auto& err : errors)
        {
            std::cout << err.first.what() << err.second << std::endl;
        }
        
        background::Renderer::print_msg("TEST: --- Failed");
        return EXIT_FAILURE;
    }
    else
    {
        background::Renderer::print_msg("TEST: --- Success");
        return EXIT_SUCCESS;
    }
}

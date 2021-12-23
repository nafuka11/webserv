#include <iostream>
#include "Webserv.hpp"

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            throw std::invalid_argument("Invalid argument.");
        }
        Webserv webserv(argv[1]);
        webserv.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

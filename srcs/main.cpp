#include <iostream>
#include "HTTPServer.hpp"

int main(void)
{
    try
    {
        HTTPServer server(4242);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

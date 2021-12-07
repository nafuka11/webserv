#include <iostream>
#include "Config.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Failed arguments. Please enter the following... " << std::endl
                  << "$ ./file [CONFIG_FILE_PATH]" << std::endl;
        return (0);
    }
    try
    {
        Config config = Config();
        config.readFile(argv[1]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}

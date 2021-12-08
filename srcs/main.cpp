#include <iostream>
#include "Webserv.hpp"

int main(void)
{
    try
    {
        // TODO: Configのファイル読み込み処理実装後、コマンドライン引数からセットすること
        Webserv webserv("dummy.conf");
        webserv.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

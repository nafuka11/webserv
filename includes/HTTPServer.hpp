#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

class HTTPServer
{
public:
    HTTPServer(int port);
    ~HTTPServer();
    void run();

private:
    int port_;
};

#endif /* HTTPSERVER_HPP */

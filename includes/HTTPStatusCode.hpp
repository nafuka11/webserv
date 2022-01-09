#ifndef HTTPSTATUSCODE_HPP
#define HTTPSTATUSCODE_HPP

enum HTTPStatusCode
{
    CODE_200 = 200,   // OK
    CODE_400 = 400,   // Bad Request
    CODE_404 = 404,   // Not Found
    CODE_405 = 405,   // Method Not Allowed
    CODE_413 = 413,   // Payload Too Large
    CODE_501 = 501    // Not Implemented
};

#endif /* HTTPSTATUSCODE_HPP */

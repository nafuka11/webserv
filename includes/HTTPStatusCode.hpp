#ifndef HTTPSTATUSCODE_HPP
#define HTTPSTATUSCODE_HPP

enum HTTPStatusCode
{
    CODE_100 = 100,   // Continue
    CODE_101 = 101,   // Switching Protocols
    CODE_200 = 200,   // OK
    CODE_201 = 201,   // Created
    CODE_202 = 202,   // Accepted
    CODE_203 = 203,   // Non-Authoritative Information
    CODE_204 = 204,   // No Content
    CODE_205 = 205,   // Reset Content
    CODE_206 = 206,   // Partial Content
    CODE_300 = 300,   // Multiple Choices
    CODE_301 = 301,   // Moved Permanently
    CODE_302 = 302,   // Found
    CODE_303 = 303,   // See Other
    CODE_304 = 304,   // Not Modified
    CODE_305 = 305,   // Use Proxy
    CODE_307 = 307,   // Temporary Redirect
    CODE_400 = 400,   // Bad Request
    CODE_401 = 401,   // Unauthorized
    CODE_402 = 402,   // Payment Required
    CODE_403 = 403,   // Forbidden
    CODE_404 = 404,   // Not Found
    CODE_405 = 405,   // Method Not Allowed
    CODE_406 = 406,   // Not Acceptable
    CODE_407 = 407,   // Proxy Authentication Required
    CODE_408 = 408,   // Request Timeout
    CODE_409 = 409,   // Conflict
    CODE_410 = 410,   // Gone
    CODE_411 = 411,   // Length Required
    CODE_412 = 412,   // Precondition Failed
    CODE_413 = 413,   // Payload Too Large
    CODE_414 = 414,   // URI Too Long
    CODE_415 = 415,   // Unsupported Media Type
    CODE_416 = 416,   // Range Not Satisfiable
    CODE_417 = 417,   // Expectation Failed
    CODE_426 = 426,   // Upgrade Required
    CODE_500 = 500,   // Internal Server Error
    CODE_501 = 501,   // Not Implemented
    CODE_502 = 502,   // Bad Gateway
    CODE_503 = 503,   // Service Unavailable
    CODE_504 = 504,   // Gateway Timeout
    CODE_505 = 505,   // HTTP Version Not Supported
};

#endif /* HTTPSTATUSCODE_HPP */

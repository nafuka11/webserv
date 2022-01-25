#ifndef HTTPSTATUSCODE_HPP
#define HTTPSTATUSCODE_HPP

enum HTTPStatusCode
{
    CODE_100 = 100,   // Continue
    CODE_101 = 101,   // Switching Protocols
    CODE_102 = 102,   // Processing
    CODE_103 = 103,   // Early Hints
    CODE_200 = 200,   // OK
    CODE_201 = 201,   // Created
    CODE_202 = 202,   // Accepted
    CODE_203 = 203,   // Non-Authoritative Information
    CODE_204 = 204,   // No Content
    CODE_205 = 205,   // Reset Content
    CODE_206 = 206,   // Partial Content
    CODE_207 = 207,   // Multi-Status
    CODE_208 = 208,   // Already Reported
    CODE_226 = 226,   // IM Used
    CODE_300 = 300,   // Multiple Choice
    CODE_301 = 301,   // Moved Permanently
    CODE_302 = 302,   // Found
    CODE_303 = 303,   // See Other
    CODE_304 = 304,   // Not Modified
    CODE_307 = 307,   // Temporary Redirect
    CODE_308 = 308,   // Permanent Redirect
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
    CODE_418 = 418,   // I'm a teapot
    CODE_421 = 421,   // Misdirected Request
    CODE_422 = 422,   // Unprocessable Entity
    CODE_423 = 423,   // Locked
    CODE_424 = 424,   // Failed Dependency
    CODE_425 = 425,   // Too Early
    CODE_426 = 426,   // Upgrade Required
    CODE_428 = 428,   // Precondition Required
    CODE_429 = 429,   // Too Many Requests
    CODE_431 = 431,   // Request Header Fields Too Large
    CODE_451 = 451,   // Unavailable For Legal Reasons
    CODE_500 = 500,   // Internal Server Error
    CODE_501 = 501,   // Not Implemented
    CODE_502 = 502,   // Bad Gateway
    CODE_503 = 503,   // Service Unavailable
    CODE_504 = 504,   // Gateway Timeout
    CODE_505 = 505,   // HTTP Version Not Supported
    CODE_506 = 506,   // Variant Also Negotiates
    CODE_507 = 507,   // Insufficient Storage
    CODE_508 = 508,   // Loop Detected
    CODE_510 = 510,   // Not Extended
    CODE_511 = 511    // Network Authentication Required
};

#endif /* HTTPSTATUSCODE_HPP */

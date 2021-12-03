#ifndef HTTPPARSEEXCEPTION_HPP
#define HTTPPARSEEXCEPTION_HPP

#include <stdexcept>
#include "HTTPStatusCode.hpp"

class HTTPParseException : public std::exception
{
public:
    HTTPParseException(HTTPStatusCode status_code);
    HTTPStatusCode getStatusCode() const;

private:
    HTTPStatusCode status_code_;
};

#endif /* HTTPPARSEEXCEPTION_HPP */

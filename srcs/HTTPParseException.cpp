#include "HTTPParseException.hpp"

HTTPParseException::HTTPParseException(HTTPStatusCode status_code)
    : status_code_(status_code)
{
}

HTTPStatusCode HTTPParseException::getStatusCode() const
{
    return status_code_;
}

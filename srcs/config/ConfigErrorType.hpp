#ifndef CONFIGERRORTYPE_HPP
# define CONFIGERRORTYPE_HPP

enum ConfigErrorType
{
    DUPLICATE_DIRECTIVE,
    DUPLICATE_LOCATION,
    DUPLICATE_VALUE,
    INVALID_NUM_OF_ARGS,
    INVALID_VALUE,
    NO_END_SEMICOLON,
    NO_OPEN_DIRECTIVE,
    NOT_ALLOWED_DIRECTIVE,
    UNEXPECTED,
    UNEXPECTED_END,
    UNKOWN_DIRECTIVE
};

#endif

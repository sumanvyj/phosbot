#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>

/** Simple Exception class, derived from std::runtime_error */
class Exception : public std::runtime_error
{
public:
  Exception() : std::runtime_error("Undocumented Exception.") {}
  Exception(String except) : std::runtime_error(except){}
};

/** Thrown when a given name or ID should be unique, but isn't */
class NonUniqueNameException : public std::runtime_error
{
public:
  NonUniqueNameException() : std::runtime_error("Non-Unique Name Exeption!") {}
  NonUniqueNameException(String except) : std::runtime_error("Non-Unique Name Exception: "+except){}
};

#endif


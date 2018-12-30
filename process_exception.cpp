#include "process_exception.h"
#include <map>
#include <string>
#include <stdexcept>

enum ERROR_CODE
{

};


class process_error : public std::runtime_error
{
	unsigned error_code;
	process_error(int error_code, std::string err_msg) :std::runtime_error(err_msg), error_code(error_code) {}
};

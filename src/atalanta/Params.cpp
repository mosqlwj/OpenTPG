#include "Params.hpp"


#ifdef _WIN32
extern const char * realpath(const char *restrict file_name, char *restrict resolved_name)
{
    return file_name;
}
#endif


namespace hiatpg {

}

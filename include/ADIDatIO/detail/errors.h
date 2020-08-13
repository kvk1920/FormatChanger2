#include <ctime>

namespace ADIDatIO::detail
{

#include <ADIDatCAPI_mex.h>

[[noreturn]]
void throw_exception(detail::ADIResultCode code);

void check_exception(detail::ADIResultCode code);

}

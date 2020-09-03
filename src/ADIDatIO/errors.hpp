#pragma once

#include <ctime>

#include <extern/ADIDatIO/ADIDatCAPI_mex.h>

namespace ADIDatIO::detail
{

[[noreturn]]
void throwException(ADIResultCode code);

void checkException(ADIResultCode code);

}

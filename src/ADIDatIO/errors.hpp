#pragma once

#include <ctime>

#include <ADIDatCAPI_mex.h>

namespace ADIDatIO::detail
{

[[noreturn]]
void throwException(ADIResultCode code);

void checkException(ADIResultCode code);

}

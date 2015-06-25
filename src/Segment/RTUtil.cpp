#include "RTCore.h"

using namespace arma;
namespace Nf
{
  void NTrace(const char *fmt, ...)
  {
    char str[1024] = {0};
    va_list Argp;
    va_start (Argp, fmt);
    vsprintf(str, fmt, Argp);
    va_end(Argp);
    OutputDebugString(str);
  }
}
#include "FrontendError.h"

#include <stdarg.h>
#define MAX_LOG_LENGTH 1024

void Err::ReportError(const FrontendLog& err)
{
    switch (err.Severity)
    {
    case FrontendErrorSeverity::Error:
        printf("\u001b[31m[FRONTEND ERROR]\u001b[0m ");
        break;
    case FrontendErrorSeverity::Info:
        printf("\u001b[32m[FRONTEND INFO]\u001b[0m ");
        break;
    case FrontendErrorSeverity::Warning:
        printf("\u001b[33m[FRONTEND WARNING]\u001b[0m ");
        break;
    case FrontendErrorSeverity::Unknown:
        printf("\u001b[96m[FRONTEND UNKNOWN]\u001b[0m ");
        break;
    default:
        assert(false);
    }
    printf("%s\n", err.Msg.c_str());
}

namespace Err
{
    static void VReportError(va_list args, FrontendErrorSeverity es, const char* fmt)
    {
        char buf[MAX_LOG_LENGTH];
        vsprintf_s(buf, fmt, args);
        ReportError({ buf, es });
    }
}


void Err::ReportError(FrontendErrorSeverity es, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VReportError(args, es, fmt);
    va_end(args);
}

void Err::LogInfo(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VReportError(args, FrontendErrorSeverity::Info, fmt);
    va_end(args);
}

void Err::LogWarning(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VReportError(args, FrontendErrorSeverity::Warning, fmt);
    va_end(args);
}

void Err::LogError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VReportError(args, FrontendErrorSeverity::Error, fmt);
    va_end(args);
}

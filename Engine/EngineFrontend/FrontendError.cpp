#include "FrontendError.h"



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

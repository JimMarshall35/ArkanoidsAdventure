#include "FrontendError.h"
#include "EditorServer.h"
#include "EditorServerMsg.h"
#include <stdarg.h>
#include "EngineLib.h"

#define MAX_LOG_LENGTH 1024
namespace Err
{
    void ReportError(const FrontendLog& err, bool local)
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

        if (!local && Editor::IsEditorConnected())
        {
            EditorServer::Msg msg;
            msg.Data = EditorServer::EngineLogMsg();
            msg.Type = EditorServer::MsgType::EngineLog;
            std::get<EditorServer::EngineLogMsg>(msg.Data).src = EditorServer::LogSource::Frontend;
            std::get<EditorServer::EngineLogMsg>(msg.Data).msg = err.Msg;

            switch (err.Severity)
            {
            case FrontendErrorSeverity::Error:
                std::get<EditorServer::EngineLogMsg>(msg.Data).severity = EditorServer::LogSeverity::Error;
                break;
            case FrontendErrorSeverity::Info:
                std::get<EditorServer::EngineLogMsg>(msg.Data).severity = EditorServer::LogSeverity::Info;
                break;
            case FrontendErrorSeverity::Warning:
                std::get<EditorServer::EngineLogMsg>(msg.Data).severity = EditorServer::LogSeverity::Warning;
                break;
            case FrontendErrorSeverity::Unknown:
                std::get<EditorServer::EngineLogMsg>(msg.Data).severity = EditorServer::LogSeverity::Unknown;
                break;
            }
            EVerify(!Editor::EnqueueMsg(msg));
        }
    }

    static void VReportError(va_list args, FrontendErrorSeverity es, const char* fmt, bool local = false)
    {
        char buf[MAX_LOG_LENGTH];
        vsprintf_s(buf, fmt, args);
        ReportError({ buf, es }, local);
    }

    void ReportError(FrontendErrorSeverity es, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        VReportError(args, es, fmt);
        va_end(args);
    }

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

void Err::LogInfoLocal(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VReportError(args, FrontendErrorSeverity::Info, fmt, true);
    va_end(args);
}

void Err::LogWarningLocal(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VReportError(args, FrontendErrorSeverity::Warning, fmt, true);
    va_end(args);
}

void Err::LogErrorLocal(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VReportError(args, FrontendErrorSeverity::Error, fmt, true);
    va_end(args);
}

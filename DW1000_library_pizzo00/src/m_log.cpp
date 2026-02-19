#include "m_log.h"

#include <cstdarg>
#include <unordered_map>
#include <HardwareSerial.h>

namespace m_log
{
    #define SAVE_LOGS false

    #if SAVE_LOGS
        ushort logs_idx = 0;
        ushort LOGS_QTY = 100;
        ushort LOGS_LEN = 251;
        char** logs;
    #endif

    enum class LOG_LEVEL
    {
        m_VERBOSE = 0,
        m_DEBUG = 1,
        m_INFO = 2,
        m_WARNING = 3,
        m_ERROR = 4,
    };

    static LOG_LEVEL gloablLogLevels;
    static std::unordered_map<std::string, LOG_LEVEL> logLevels;

    void setup()
    {
#if SAVE_LOGS
        logs = (char**)malloc(sizeof(char*) * LOGS_QTY);
        for(int i = 0; i < LOGS_QTY; i++)
        {
            logs[i] = (char*)malloc(sizeof(char) * LOGS_LEN);
            strcpy(logs[i], "");
        }
#endif

        gloablLogLevels = LOG_LEVEL::m_INFO;
        
        logLevels[LOG_DW1000] = LOG_LEVEL::m_VERBOSE;
        logLevels[LOG_DW1000_MSG] = LOG_LEVEL::m_VERBOSE;
    }

    void log(std::string const& tag, LOG_LEVEL level, const char* msg, std::va_list args)
    {
        LOG_LEVEL minLevel = logLevels.count(tag) ? logLevels[tag] : gloablLogLevels;
        if(level >= minLevel)
        {
#if SAVE_LOGS
            vsnprintf (logs[logs_idx], LOGS_LEN-1, msg, args);
            Serial.println(logs[logs_idx]);
            logs_idx = (logs_idx + 1) % LOGS_QTY;
#else
            // https://arduino.stackexchange.com/a/72456
            for(const char* i=msg; *i!=0; ++i) {
                if(*i!='%') { Serial.print(*i); continue; }
                switch(*(++i)) {
                case '%': Serial.print('%'); break;
                case 's': Serial.print(va_arg(args, char*)); break;
                case 'd': Serial.print(va_arg(args, int), DEC); break;
                case 'b': Serial.print(va_arg(args, int), BIN); break;
                case 'o': Serial.print(va_arg(args, int), OCT); break;
                case 'x': Serial.print(va_arg(args, int), HEX); break;
                case 'f': Serial.print(va_arg(args, double), 2); break;
                }
            }
            Serial.println();
#endif
            va_end(args);
        }
    }

    void log_err(std::string const& tag, const char* msg, ...) { std::va_list args; va_start(args, msg); log(tag, LOG_LEVEL::m_ERROR, msg, args); }
    void log_war(std::string const& tag, const char* msg, ...) { std::va_list args; va_start(args, msg); log(tag, LOG_LEVEL::m_WARNING, msg, args); }
    void log_inf(std::string const& tag, const char* msg, ...) { std::va_list args; va_start(args, msg); log(tag, LOG_LEVEL::m_INFO, msg, args); }
    void log_dbg(std::string const& tag, const char* msg, ...) { std::va_list args; va_start(args, msg); log(tag, LOG_LEVEL::m_DEBUG, msg, args); }
    void log_vrb(std::string const& tag, const char* msg, ...) { std::va_list args; va_start(args, msg); log(tag, LOG_LEVEL::m_VERBOSE, msg, args); }

    int getLogQty()
    {
#if SAVE_LOGS
        return LOGS_QTY;
#else
        return 0;
#endif
    }

    int getCurrentLogIdx()
    {
#if SAVE_LOGS
        return logs_idx;
#else
        return 0;
#endif
    }

    char** getLogs()
    {
#if SAVE_LOGS
        return logs;
#else
        return null;
#endif
    }
}
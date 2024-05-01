#pragma once

#include <cstdlib>
#include <unordered_map>

enum ErrorCode {
    ERROR_PARSING_CONFIG_FILE = 0,
};

enum SeverityError {
    MINOR = 0,  // see ThrowErrorFunction;
    MEDIUM = 1,
    MAJOR = 2,
};

inline std::unordered_map<ErrorCode, const char*> mapErrorMessage = {
    {ERROR_PARSING_CONFIG_FILE, "ERROR: Could not parse config file:"},
};

struct Error {
   public:
    Error(ErrorCode errorCode_, char* errorInfo_, SeverityError severity_);
    ~Error();

    ErrorCode GetErrorCode();
    SeverityError GetSeverity();
    static ErrorCode GetLastErrorCode();
    static Error GetLastError();

    void SetErrorInfo(char*);
    static void SetLastError(Error);

    void ThrowError();

   private:
    const char* defaultMessage;
    char* info;

    ErrorCode code;
    SeverityError severity;

    static Error lastError;  // Gets updated with ThrowError function
};

void ThrowError(ErrorCode, char* errorInfo_, SeverityError severity_);

//------------------------------------------------------------------------------

inline Error::Error(ErrorCode errorCode_, char* errorInfo_,
                    SeverityError severity_)
    : code(errorCode_), info(errorInfo_), severity(severity_) {
    this->defaultMessage = "ERROR: Could not initialize error!";

    if (mapErrorMessage.find(errorCode_) != mapErrorMessage.end()) {
        this->defaultMessage = mapErrorMessage[code];
    }
}

inline ErrorCode Error::GetErrorCode() { return this->code; }
inline SeverityError Error::GetSeverity() { return this->severity; }

inline ErrorCode Error::GetLastErrorCode() { return Error::lastError.code; }
inline Error Error::GetLastError() { return Error::lastError; }

inline void Error::SetErrorInfo(char* info_) { this->info = info_; };
inline void Error::SetLastError(Error error_) { Error::lastError = error_; };

inline void Error::ThrowError() {
    lastError = *this;
    switch ((*this).severity) {
        case MINOR: {
            // should just print
            break;
        }
        case MEDIUM: {
            // should be drawn to screen
            break;
        }
        case MAJOR: {
            exit(1);
        }
    }
}

inline void ThrowError(ErrorCode codeError_, char* errorInfo_,
                       SeverityError severity_) {
    Error::SetLastError(Error(codeError_, errorInfo_, severity_));
    switch (severity_) {
        case MINOR: {
            // should just print
            break;
        }
        case MEDIUM: {
            // should be drawn to screen
            break;
        }
        case MAJOR: {
            exit(1);
        }
    }
}

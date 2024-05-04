#pragma once

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include <unordered_map>

enum ErrorCode {
    NO_CODE = -1,

    // File input/output
    FAILED_TO_OPEN_FILE = 20,

    // Config items
    COULDNT_PARSE_CONFIG_FILE = 100,
    INVALID_TOKEN = 101,

    // SDL Error
    SDL_FUNCTION_ERROR = 200,
};

enum Severity {
    MINOR = 0,  // see ThrowErrorFunction;
    MEDIUM = 1,
    MAJOR = 2,
};

inline static std::unordered_map<ErrorCode, std::string> mapErrorMessage = {
    {FAILED_TO_OPEN_FILE, "ERROR: Failed to open file: "},
    {COULDNT_PARSE_CONFIG_FILE, "ERROR: Could not parse config file: "},
    {INVALID_TOKEN, "ERROR: Invalid token: "},
};

struct Error {
   public:
    Error(ErrorCode errorCode_, std::string errorInfo_,
          Severity severity_);
    ~Error();

    ErrorCode GetErrorCode();
    Severity GetSeverity();
    std::string GetInfo();
    static ErrorCode GetLastErrorCode();
    static Error GetLastError();

    void SetErrorInfo(std::string);
    static void SetLastError(Error);

    void ThrowError(std::string path);

   private:
    void CreateLog(std::string path);

   private:
    ErrorCode code = NO_CODE;
    std::string info = "";
    Severity severity = MINOR;
    std::string defaultMessage = "";

    static Error lastError;  // Gets updated with ThrowError function

   private:
    Error();
};

void ThrowError(ErrorCode, std::string errorInfo_, Severity severity_, std::string path);

void CreateLog(Error, std::string path);

//------------------------------------------------------------------------------

inline Error::~Error() {};
inline Error::Error() {};
inline Error::Error(ErrorCode errorCode_, std::string errorInfo_, Severity severity_) : code(errorCode_), info(errorInfo_), severity(severity_) {
    this->defaultMessage = "ERROR: Could not initialize error!";

    if (mapErrorMessage.find(errorCode_) != mapErrorMessage.end()) {
        this->defaultMessage = mapErrorMessage[code];
    }
}

inline Error Error::lastError = Error{};

inline ErrorCode Error::GetErrorCode() { return this->code; }
inline Severity Error::GetSeverity() { return this->severity; }
inline std::string Error::GetInfo() { return this->info; }

inline ErrorCode Error::GetLastErrorCode() { return Error::lastError.code; }
inline Error Error::GetLastError() { return Error::lastError; }

inline void Error::SetErrorInfo(std::string info_) { this->info = info_; };
inline void Error::SetLastError(Error error_) { Error::lastError = error_; };

inline void Error::ThrowError(std::string logPath) {
    lastError = *this;
    printf("%s%s", mapErrorMessage[this->code].c_str(), this->defaultMessage.c_str());
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
            CreateLog(logPath);
            exit(1);
        }
    }
}

inline void ThrowError(ErrorCode codeError_, std::string errorInfo_, Severity severity_, std::string logPath) {
    Error err(codeError_, errorInfo_, severity_);
    Error::SetLastError(err);

    if (mapErrorMessage.find(codeError_) != mapErrorMessage.end()) {
        printf("%s%s", mapErrorMessage[codeError_].c_str(), errorInfo_.c_str());
    }

    switch (severity_) {
        case MINOR: {
            break;
        }
        case MEDIUM: {
            // should be drawn to screen
            break;
        }
        case MAJOR: {
            CreateLog(err, logPath);
            exit(1);
        }
    }
}

inline void Error::CreateLog(std::string path) {
    if (std::filesystem::create_directories("logs/")) {
        printf("INFO: Created logs directory\n");
    }
    std::ofstream logFile(path, std::ios::app);

    if (!logFile.is_open()) {
        ::ThrowError(ErrorCode::FAILED_TO_OPEN_FILE, path + "\n", Severity::MINOR, path);  // minor so it doesnt enter in a loop of not being able to open logFile
        return;
    }
    {
        std::stringstream logMessage;
        std::time_t timeNow = std::time(nullptr);
        logMessage << "\n"
                   << std::asctime(std::localtime(&timeNow)) << "{\n"
                   << "Severity: " << this->GetSeverity() << "\n"
                   << "ErrorCode: " << this->GetErrorCode() << "\n"
                   << "ErrorMessage: " << mapErrorMessage[(*this).GetErrorCode()] << "\n"
                   << "Info: " << this->GetInfo()
                   << "}\n";

        logFile << logMessage.str();
    }
    logFile.close();
}

inline void CreateLog(Error err, std::string path) {
    if (std::filesystem::create_directories("logs/")) {
        printf("INFO: Created logs directory\n");
    }
    std::ofstream logFile(path, std::ios::app);

    if (!logFile.is_open()) {
        ThrowError(ErrorCode::FAILED_TO_OPEN_FILE, path + "\n", Severity::MINOR, path);  // minor so it doesnt enter in a loop of not being able to open logFile
        return;
    }

    {
        std::stringstream logMessage;
        std::time_t timeNow = std::time(nullptr);
        logMessage << "\n"
                   << std::asctime(std::localtime(&timeNow)) << "{\n"
                   << "Severity: " << err.GetSeverity() << "\n"
                   << "ErrorCode: " << err.GetErrorCode() << "\n"
                   << "ErrorMessage: " << mapErrorMessage[err.GetErrorCode()] << "\n"
                   << "Info: " << err.GetInfo()
                   << "}\n";

        logFile << logMessage.str();
    }
    logFile.close();
}

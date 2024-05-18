#pragma once

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

enum ErrorCode {
    NON = 0,

    // Wrong code
    BAD_PARAMS = 10,

    // File input/output
    FAILED_TO_OPEN_FILE = 20,

    // Config items
    COULDNT_PARSE_CONFIG_FILE = 100,
    INVALID_TOKEN = 101,

    // SDL Error
    SDL_FUNCTION_ERROR = 200,
    COULDNT_LOAD_TEXTURE = 201,
    TEXTURE_IS_NULL = 202,
    TEXTURE_ALREADY_SET = 203,
};

enum Severity {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2,
};

inline static std::unordered_map<ErrorCode, std::string> mapErrorMessage = {
    {FAILED_TO_OPEN_FILE, "ERROR: Failed to open file: "},
    {COULDNT_PARSE_CONFIG_FILE, "ERROR: Could not parse config file: "},
    {INVALID_TOKEN, "ERROR: Invalid token: "},
};

class Error {
   public:
    operator bool() const { return code != NON; }

   public:
    Error(ErrorCode errorCode_, std::string errorInfo_,
          Severity severity_);
    Error();
    ~Error();

    ErrorCode GetErrorCode();
    Severity GetSeverity();
    std::string GetInfo();
    static ErrorCode GetLastErrorCode();
    static Error GetLastError();

    void SetErrorInfo(std::string);
    static void SetLastError(Error);

   private:
    void CreateLog(std::string path);

   private:
    ErrorCode code = NON;
    std::string info = "";
    Severity severity = LOW;
    std::string defaultMessage = "";

    static Error lastError;  // NOTE: Not used
};

void CreateLog(Error, std::string path);

void Crash(Error);

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

inline void Error::CreateLog(std::string path) {
    if (std::filesystem::create_directories("logs/")) {
        printf("INFO: Created logs directory\n");
    }
    std::ofstream logFile(path, std::ios::app);

    if (!logFile.is_open()) {
        Crash(Error{COULDNT_PARSE_CONFIG_FILE, path, MEDIUM});
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
        Crash(Error{ErrorCode::FAILED_TO_OPEN_FILE, path + "\n", Severity::LOW});
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

inline void Crash(Error error_) {
    std::cerr << error_.GetInfo();
    exit(error_.GetErrorCode());
    // TODO: to be implemented
}

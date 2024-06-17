#pragma once

#include <SDL2/SDL_render.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

class [[nodiscard]] Error {
   public:
    inline bool IsEmpty() const { return code == ErrorCode::NONE; }  // TODO: Use this instead

   public:
    enum class ErrorCode {
        NONE = 0,

        // Wrong code
        BAD_PARAMS = 10,

        // File input/output
        FAILED_TO_OPEN_FILE = 20,

        // Config items
        COULDNT_PARSE_CONFIG_FILE = 100,
        INVALID_CONFIG_TOKEN = 101,

        // SDL Error
        SDL_FUNCTION_ERROR = 200,
        COULDNT_LOAD_TEXTURE = 201,
        TEXTURE_IS_NULL = 202,
        TEXTURE_ALREADY_SET = 203,
    };
    enum class Severity {
        LOW = 0,
        MEDIUM = 1,
        HIGH = 2,
    };

    inline Error() = default;
    inline ~Error() {
        if (!isHandled && !this->IsEmpty()) {
            std::terminate();
        }
    };
    inline Error(ErrorCode errorCode_, std::string errorInfo_, Severity severity_) : code(errorCode_), info(errorInfo_), severity(severity_) {
        this->defaultMessage = "ERROR: Could not initialize error!";
        if (mapErrorMessage.find(errorCode_) != mapErrorMessage.end()) {
            this->defaultMessage = mapErrorMessage[code];
        }

        if (!this->IsEmpty()) {
            lastError = *this;
        }
    }

    inline ErrorCode GetErrorCode() const { return code; }
    inline Severity GetSeverity() const { return severity; }
    inline std::string GetInfo() const { return info; }
    inline std::string GetDefaultMessage() const { return defaultMessage; }

    static ErrorCode GetLastErrorCode() {
        if (lastError.IsEmpty()) {
            return ErrorCode::NONE;
        }
        return lastError.code;
    }
    static const Error GetLastError() {
        if (lastError.IsEmpty()) {
            return Error();
        }
        return lastError;
    }

    inline void SetErrorInfo(std::string info_) { this->info = info_; };

    inline void Handle() const {
        if (this->IsEmpty()) return;
        int severity = static_cast<int>(this->severity);
        if (severity >= static_cast<int>(Error::Severity::LOW)) PrintError(*this);
        if (severity == static_cast<int>(Error::Severity::HIGH)) Crash(*this);
        this->isHandled = true;
    }

   private:
    ErrorCode code = ErrorCode::NONE;
    std::string info = "";
    Severity severity = Severity::LOW;
    std::string defaultMessage = "";
    mutable bool isHandled = false;

    inline static std::unordered_map<ErrorCode, std::string> mapErrorMessage = {
        {ErrorCode::FAILED_TO_OPEN_FILE, "ERROR: Failed to open file: "},
        {ErrorCode::COULDNT_PARSE_CONFIG_FILE, "ERROR: Could not parse config file: "},
        {ErrorCode::INVALID_CONFIG_TOKEN, "ERROR: Invalid token: "},
    };

    static Error lastError;

   private:
    static void PrintError(const Error& err) {
        std::cerr << "CODE: " << static_cast<int>(err.GetErrorCode()) << " " << err.GetDefaultMessage() << err.GetInfo() << std::endl;
    };
    static void Crash(const Error& err) { (void)err; };
};
inline Error Error::lastError = Error();

template <typename Type>
class [[nodiscard]] Result {
   public:
    Error error = Error();
    Type result = Type();

   public:
    Result<Type> Handle() {
        error.Handle();
        return *this;
    }
};

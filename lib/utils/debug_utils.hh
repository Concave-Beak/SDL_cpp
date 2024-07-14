#pragma once

#include <SDL2/SDL_render.h>

#include <cassert>
#include <exception>
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
        if (!isHandled && !IsEmpty()) {
            std::terminate();
        }
    };
    inline Error(ErrorCode errorCode_, std::string errorInfo_, Severity severity_) : code(errorCode_), info(errorInfo_), severity(severity_) {
        defaultMessage = "ERROR: Could not initialize error messsage!";
        if (mapErrorMessage.find(errorCode_) != mapErrorMessage.end()) {
            defaultMessage = mapErrorMessage[code];
        }

        if (!IsEmpty()) {
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

    inline void SetErrorInfo(std::string info_) { info = info_; };

    inline void Handle() const {
        if (IsEmpty()) return;
        int severity_ = static_cast<int>(severity);
        if (severity_ >= static_cast<int>(Error::Severity::LOW)) PrintError(*this);
        if (severity_ == static_cast<int>(Error::Severity::HIGH)) Crash();
        isHandled = true;
    }

   private:
    ErrorCode code = ErrorCode::NONE;
    std::string info = "";
    Severity severity = Severity::LOW;
    std::string defaultMessage = "";
    mutable bool isHandled = false;

    inline static std::unordered_map<ErrorCode, std::string> mapErrorMessage = {
        {ErrorCode::NONE, ""},

        {ErrorCode::BAD_PARAMS, "ERROR: Bad parameters: "},

        {ErrorCode::FAILED_TO_OPEN_FILE, "ERROR: Failed to open file: "},

        {ErrorCode::COULDNT_PARSE_CONFIG_FILE, "ERROR: Could not parse config file: "},
        {ErrorCode::INVALID_CONFIG_TOKEN, "ERROR: Invalid token: "},

        {ErrorCode::SDL_FUNCTION_ERROR, "ERROR: SDL function error: "},
        {ErrorCode::COULDNT_LOAD_TEXTURE, "ERROR: Couldn't load texture: "},
        {ErrorCode::TEXTURE_IS_NULL, "ERROR: Texture is NULL: "},
        {ErrorCode::TEXTURE_ALREADY_SET, "ERROR: Texture field is already set: "},
    };

    static Error lastError;

   private:
    static void PrintError(const Error& err) {
        std::cerr << "CODE: " << static_cast<int>(err.GetErrorCode()) << '\n'
                  << err.GetDefaultMessage() << ' ' << err.GetInfo() << std::endl;
    };
    static void Crash() { std::terminate(); };
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

enum class Info {
    SDL_INITIALIZED_SUCESSFULY = 0x01,
    SDL_WINDOW_INITIALIZED = 0x02,
    SDL_RENDERER_INITIALIZED = 0x03,

    STARTING_GAME_LOOP = 0x100,
    GAME_CLOSED = 0x101,

    FONT_LOADED = 0x200,

    CONFIG_READ_SUCESSFULLY = 0x300,

    PLAYER_INITIALIZED_SUCESSFULLY = 0x400,
};
inline void PrintInfo(Info info_, std::string msg) {
    switch (info_) {
        case Info::SDL_INITIALIZED_SUCESSFULY:
            std::cout << "INFO: SDL_Init initialized sucessfully\n";
            break;
        case Info::SDL_WINDOW_INITIALIZED:
            std::cout << "INFO: Window initialized successfully\n";
            break;
        case Info::SDL_RENDERER_INITIALIZED:
            std::cout << "INFO: Renderer initialized successfully\n";
            break;
        case Info::STARTING_GAME_LOOP:
            std::cout << "INFO: Game loop started\n";
            break;
        case Info::GAME_CLOSED:
            std::cout << "INFO: Game closed";
            break;
        case Info::FONT_LOADED:
            std::cout << "INFO: Font loaded :" << msg << '\n';
            break;
        case Info::CONFIG_READ_SUCESSFULLY:
            std::cout << "INFO: Config read sucessfully\n";
            break;
        case Info::PLAYER_INITIALIZED_SUCESSFULLY: {
            std::cout << "INFO: Player initialized sucessfully\n";
            break;
        }
    }
}

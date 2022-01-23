#include <cstdio>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sys/ioctl.h>
#include <chrono>
#include <exception>
#include <thread>
#include <filesystem>
#include <span>
#include <fmt/format.h>

struct FileRaiiGuard {
    FileRaiiGuard(std::string_view filePath):m_fileHandle{}
    {
        if (!std::filesystem::exists(filePath)) {
            throw std::runtime_error(
                        fmt::format("Bad file path for the FileRaiiGuard, path:{}", filePath));
        }
        m_fileHandle = open(filePath.data(), O_WRONLY);

        if (m_fileHandle == kInvalidFileHandle) {
            throw std::runtime_error(
                        fmt::format("Failed to open the given file, path: {}", filePath));
        }
    }
    FileRaiiGuard &operator=(const FileRaiiGuard &) = delete;
    FileRaiiGuard(const FileRaiiGuard &) = delete;

    ~FileRaiiGuard()
    {
        if (m_fileHandle != kInvalidFileHandle) {
            close(m_fileHandle);
        }
    }

public:
    template<typename TIOCTLCall, typename  IOCtlData>
    void passIoctl(TIOCTLCall&& ioctlCall, IOCtlData&& data)
    {
        auto returnCode = ioctl(m_fileHandle, ioctlCall,&data);
        if(returnCode == kInvalidIoctlCall)
            throw std::runtime_error(fmt::format("IOCL to device finished with the error:{}",returnCode));
    }

    std::int64_t readFrom(std::span<char> readTo)
    {
        auto retCode = read(m_fileHandle,readTo.data(),readTo.size());
        if(retCode == kDefErrorCode)
            throw std::runtime_error(fmt::format("READ from file failed with code:{}", retCode));
        
        return retCode;
    }

private:
    static constexpr inline std::int32_t kInvalidIoctlCall = -1;
    static constexpr inline std::int32_t kInvalidFileHandle = -1;
    static constexpr inline std::int32_t kDefErrorCode = -1;
private:
    int m_fileHandle;
};
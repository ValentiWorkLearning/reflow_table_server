#pragma once
#include <filesystem>
#include <string_view>

namespace Utils::Executable
{
inline std::filesystem::path GetExecutableLocation(std::string_view basePath)
{
    std::filesystem::path executable{basePath};
    return executable.remove_filename();
}
}

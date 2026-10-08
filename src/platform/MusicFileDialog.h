#pragma once

#include <filesystem>

namespace platform {

std::filesystem::path ChooseMusicFile();
std::filesystem::path ChooseBackgroundFile();
std::string PathToUtf8(const std::filesystem::path& path);
bool CopyFileTo(const std::filesystem::path& source, const std::filesystem::path& destination);
bool ReplaceFileWith(const std::filesystem::path& source, const std::filesystem::path& destination);

}  // namespace platform

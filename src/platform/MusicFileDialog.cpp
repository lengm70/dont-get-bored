#include "platform/MusicFileDialog.h"
#include <iterator>

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif

namespace platform {

std::string PathToUtf8(const std::filesystem::path& path) {
#ifdef _WIN32
    const std::wstring wide = path.wstring();
    if (wide.empty()) return {};
    const int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()),
                                         nullptr, 0, nullptr, nullptr);
    std::string utf8(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()),
                        utf8.data(), size, nullptr, nullptr);
    return utf8;
#else
    return path.string();
#endif
}

bool CopyFileTo(const std::filesystem::path& source, const std::filesystem::path& destination) {
#ifdef _WIN32
    return CopyFileW(source.c_str(), destination.c_str(), FALSE) != 0;
#else
    std::error_code error;
    std::filesystem::copy_file(source, destination,
                               std::filesystem::copy_options::overwrite_existing, error);
    return !error;
#endif
}

std::filesystem::path ChooseMusicFile() {
#ifdef _WIN32
    wchar_t path[MAX_PATH]{};
    OPENFILENAMEW dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.lpstrFile = path;
    dialog.nMaxFile = static_cast<DWORD>(std::size(path));
    dialog.lpstrFilter = L"Music files\0*.mp3;*.wav;*.ogg;*.flac;*.mod;*.xm\0All files\0*.*\0";
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    return GetOpenFileNameW(&dialog) ? std::filesystem::path(path) : std::filesystem::path{};
#else
    return {};
#endif
}

bool ReplaceFileWith(const std::filesystem::path& source, const std::filesystem::path& destination) {
#ifdef _WIN32
    return MoveFileExW(source.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
#else
    std::error_code error;
    std::filesystem::rename(source, destination, error);
    return !error;
#endif
}

std::filesystem::path ChooseBackgroundFile() {
#ifdef _WIN32
    wchar_t path[MAX_PATH]{};
    OPENFILENAMEW dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.lpstrFile = path;
    dialog.nMaxFile = static_cast<DWORD>(std::size(path));
    dialog.lpstrFilter = L"Image files\0*.png;*.jpg;*.jpeg;*.bmp\0All files\0*.*\0";
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    return GetOpenFileNameW(&dialog) ? std::filesystem::path(path) : std::filesystem::path{};
#else
    return {};
#endif
}

}  // namespace platform

#include "config/ProgressRecords.h"
#include <algorithm>
#include <charconv>
#include <fstream>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
namespace app {
namespace {
bool Valid(const std::string &key) {
    return !key.empty() && key.size() < 80 &&
           std::all_of(
               key.begin(), key.end(),
               [](unsigned char c) {
                   return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '_';
               });
}
} // namespace
bool ProgressRecords::Set(const std::string &key, int value) {
    if (!Valid(key) || value <= 0 || Get(key) == value)
        return false;
    values_[key] = value;
    dirty_ = true;
    return true;
}
int ProgressRecords::Get(const std::string &key) const {
    auto i = values_.find(key);
    return i == values_.end() ? 0 : i->second;
}
bool ProgressRecords::Best(const std::string &key, int value, bool lower) {
    if (!Valid(key) || value <= 0)
        return false;
    int old = Get(key);
    if (old && (lower ? old <= value : old >= value))
        return false;
    values_[key] = value;
    dirty_ = true;
    return true;
}
bool ProgressRecords::Load(const std::filesystem::path &path) {
    std::ifstream in(path);
    if (!in)
        return false;
    std::map<std::string, int> loaded;
    std::string line;
    while (std::getline(in, line)) {
        auto split = line.find('=');
        if (split == std::string::npos)
            continue;
        auto key = line.substr(0, split), text = line.substr(split + 1);
        if (!text.empty() && text.back() == '\r')
            text.pop_back();
        int value = 0;
        auto parsed = std::from_chars(text.data(), text.data() + text.size(), value);
        if (Valid(key) && parsed.ec == std::errc{} && parsed.ptr == text.data() + text.size() &&
            value > 0)
            loaded[key] = value;
    }
    if (in.bad())
        return false;
    values_ = std::move(loaded);
    dirty_ = false;
    return true;
}
bool ProgressRecords::Save(const std::filesystem::path &path) {
    std::error_code ec;
    if (!path.parent_path().empty())
        std::filesystem::create_directories(path.parent_path(), ec);
    if (ec)
        return false;
    auto temp = path;
    temp += ".tmp";
    std::ofstream out(temp);
    if (!out)
        return false;
    out << "# Local game progress; zero/missing means no record\n";
    for (const auto &row : values_)
        out << row.first << '=' << row.second << '\n';
    out.close();
    if (!out.good())
        return false;
#ifdef _WIN32
    if (!MoveFileExW(temp.c_str(), path.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        return false;
#else
    std::filesystem::rename(temp, path, ec);
    if (ec)
        return false;
#endif
    dirty_ = false;
    return true;
}
} // namespace app

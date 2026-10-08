#include "media/MediaLibrary.h"
#include <algorithm>
#include <cstdint>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#endif

namespace media {
namespace {
constexpr const char* folders[]{"backgrounds", "player-backgrounds", "music"};
bool ValidKind(int kind) { return kind >= 0 && kind < 3; }
}
bool MediaLibrary::Load() {
    std::ifstream input(root_ / "library.txt");
    if (!input) return false;
    std::vector<Entry> loaded;
    std::string line;
    while (std::getline(input, line)) {
        std::istringstream row(line);
        int kind = -1;
        Entry entry{};
        if (!(row >> kind >> std::quoted(entry.name) >> std::quoted(entry.path)) ||
            !ValidKind(kind) || entry.path.empty()) continue;
        entry.kind = static_cast<Kind>(kind);
        if (std::none_of(loaded.begin(), loaded.end(), [&](const Entry& existing) {
            return existing.kind == entry.kind && existing.path == entry.path;
        })) loaded.push_back(std::move(entry));
    }
    if (input.bad()) return false;
    entries_ = std::move(loaded);
    return true;
}
bool MediaLibrary::Save() const {
    std::error_code error;
    std::filesystem::create_directories(root_, error);
    if (error) return false;
    const auto staged = root_ / "library.tmp";
    const auto destination = root_ / "library.txt";
    std::ofstream output(staged, std::ios::trunc);
    for (const auto& entry : entries_) output << static_cast<int>(entry.kind) << ' '
        << std::quoted(entry.name) << ' ' << std::quoted(entry.path) << '\n';
    output.close();
    if (!output.good()) return false;
#ifdef _WIN32
    const bool replaced = MoveFileExW(staged.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
#else
    std::filesystem::rename(staged, destination, error);
    const bool replaced = !error;
#endif
    if (!replaced) std::filesystem::remove(staged, error);
    return replaced;
}
std::vector<Entry> MediaLibrary::List(Kind kind) const {
    std::vector<Entry> result;
    for (const auto& entry : entries_) if (entry.kind == kind) result.push_back(entry);
    return result;
}
void MediaLibrary::Remember(Kind kind, const std::string& name, const std::string& path) {
    if (path.empty()) return;
    if (std::none_of(entries_.begin(), entries_.end(), [&](const Entry& entry) {
        return entry.kind == kind && entry.path == path;
    })) entries_.push_back({kind, name, path});
}
std::optional<Entry> MediaLibrary::Import(Kind kind, const std::filesystem::path& source,
    const std::function<bool(const std::filesystem::path&)>& use) {
    if (!ValidKind(static_cast<int>(kind))) return std::nullopt;
    std::ifstream input(source, std::ios::binary);
    if (!input) return std::nullopt;
    // Content-based names preserve past imports and avoid duplicate copies.
    std::uint64_t hash = 14695981039346656037ull;
    char buffer[8192];
    while (input.read(buffer, sizeof(buffer)) || input.gcount()) {
        for (std::streamsize index = 0; index < input.gcount(); ++index) {
            hash ^= static_cast<unsigned char>(buffer[index]); hash *= 1099511628211ull;
        }
    }
    if (input.bad()) return std::nullopt;
    auto extension = source.extension().u8string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (extension.find_first_not_of(".abcdefghijklmnopqrstuvwxyz0123456789") != std::string::npos) return std::nullopt;
    std::ostringstream id; id << std::hex << hash;
    const auto destination = root_ / folders[static_cast<int>(kind)] / (id.str() + extension);
    std::error_code error;
    std::filesystem::create_directories(destination.parent_path(), error);
    if (error) return std::nullopt;
    const bool existed = std::filesystem::exists(destination, error);
    if (error) return std::nullopt;
    if (!existed && !std::filesystem::copy_file(source, destination, error)) {
        std::filesystem::remove(destination, error); return std::nullopt;
    }
    if (!use(destination)) {
        if (!existed) std::filesystem::remove(destination, error);
        return std::nullopt;
    }
    Entry entry{kind, source.filename().u8string(), destination.generic_u8string()};
    Remember(kind, entry.name, entry.path);
    return entry;
}
}  // namespace media

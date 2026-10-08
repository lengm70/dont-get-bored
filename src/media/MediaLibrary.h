#pragma once
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace media {
enum class Kind { Background, PlayerBackground, Music };
struct Entry { Kind kind; std::string name; std::string path; };
class MediaLibrary {
public:
    explicit MediaLibrary(std::filesystem::path root = "config/media") : root_(std::move(root)) {}
    bool Load();
    bool Save() const;
    std::vector<Entry> List(Kind kind) const;
    void Remember(Kind kind, const std::string& name, const std::string& path);
    // Validate/use the copied file before adding it to history. Failed imports
    // leave the history intact and remove only a newly created candidate file.
    std::optional<Entry> Import(Kind kind, const std::filesystem::path& source,
        const std::function<bool(const std::filesystem::path&)>& use);
private:
    std::filesystem::path root_;
    std::vector<Entry> entries_;
};
}  // namespace media

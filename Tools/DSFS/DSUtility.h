#pragma once
#include <unordered_map>

namespace DSFS {
class DSUtility {
public:
    static std::unordered_multimap<std::string, std::string> _fileExtensions;
    static bool TryGetFileExtension(const std::string& signature, std::string& extension);
};
}

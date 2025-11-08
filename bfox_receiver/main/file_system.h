#ifndef BFOX_RECEIVER_MAIN_FILE_SYSTEM_H_
#define BFOX_RECEIVER_MAIN_FILE_SYSTEM_H_
// ESP32 B-Fox Beacon
// (C)2025 bekki.jp
// FileSystem

// Include ----------------------
#include <string>

namespace bfox_receiver_system {
namespace file_system {

/// Mount File system
bool Mount();

/// Unmount File System
void Unmount();

/// Write
bool Write(const std::string& file_path, const std::string& body);

/// Read
bool Read(const std::string& file_path, std::string& body);

/// Delete
bool Delete(const std::string& file_path);

}  // namespace file_system
}  // namespace bfox_receiver_system

#endif  // BFOX_RECEIVER_MAIN_FILE_SYSTEM_H_
// EOF

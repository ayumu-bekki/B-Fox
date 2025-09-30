#ifndef BRDF_RECEIVER_MAIN_FILE_SYSTEM_H_
#define BRDF_RECEIVER_MAIN_FILE_SYSTEM_H_
// ESP32 BRDF Beacon
// (C)2025 bekki.jp
// FileSystem

// Include ----------------------
#include <string>

namespace brdf_receiver_system {
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
}  // namespace brdf_receiver_system

#endif  // BRDF_RECEIVER_MAIN_FILE_SYSTEM_H_
// EOF

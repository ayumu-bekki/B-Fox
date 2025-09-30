#ifndef BleRDF_BEACON_MAIN_FILE_SYSTEM_H_
#define BleRDF_BEACON_MAIN_FILE_SYSTEM_H_
// ESP32 BleRDF Beacon
// (C)2025 bekki.jp
// FileSystem

#include <string>

namespace blerdf_beacon_system {
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
}  // namespace blerdf_beacon_system

#endif  // BleRDF_BEACON_MAIN_FILE_SYSTEM_H_

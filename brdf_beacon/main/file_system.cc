// ESP32 BRDF Beacon
// (C)2025 bekki.jp
// FileSystem

// Include ----------------------
#include "file_system.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "logger.h"

namespace BrdfBeaconSystem::FileSystem {

static constexpr char* const base_path = (char*)"/storage";

static wl_handle_t s_wl_handle =
    WL_INVALID_HANDLE;  // Handle of the wear levelling library instance

/// Mount File system
bool Mount() {
  // Mount File System
  ESP_LOGI(TAG, "Mounting FAT filesystem");
  const esp_vfs_fat_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files = 4,
      .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
      .disk_status_check_enable = false,
      .use_one_fat = false,
  };
  esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage",
                                                   &mount_config, &s_wl_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
    return false;
  }
  return true;
}


/// Unmount File System
void Unmount() {
  ESP_LOGI(TAG, "Unmounting FAT filesystem");
  esp_vfs_fat_spiflash_unmount_rw_wl(base_path, s_wl_handle);
}

/// Write
bool Write(const std::string& file_path, const std::string& body) {
  std::fstream file_open_stream;
  file_open_stream.open(base_path + std::string("/") + file_path,
                        std::ios::out);
  if (!file_open_stream.is_open()) {
    ESP_LOGE(TAG, "Failed to open file for writing");
    return false;
  }
  file_open_stream << body << std::flush;
  return true;
}

/// Read
bool Read(const std::string& file_path, std::string& body) {
  std::fstream file_read_stream;
  file_read_stream.open(base_path + std::string("/") + file_path, std::ios::in);
  if (!file_read_stream.is_open()) {
    ESP_LOGE(TAG, "Failed to open file for read");
    return false;
  }
  body = static_cast<std::stringstream const&>(std::stringstream()
                                               << file_read_stream.rdbuf())
             .str();
  return true;
}

/// Delete
bool Delete(const std::string& file_path) {
  return std::remove((base_path + std::string("/") + file_path).c_str()) == 0;
}

}  // namespace BrdfBeaconSystem::FileSystem

// EOF

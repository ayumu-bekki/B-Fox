idf_component_register(SRCS "main.cc"
                            "logger.cc"
                            "util.cc"
                            "bfox_beacon.cc"
                            "gpio_control.cc"
                            "task.cc"
                            "ble_device.cc"
                            "ble_services.cc"
                            "voltage_check_task.cc"
                            "ibeacon.cc"
                            "beacon_setting.cc"
                    INCLUDE_DIRS "")

component_compile_options(-Wno-error=format= -Wno-format)


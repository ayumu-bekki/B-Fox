idf_component_register(SRCS "main.cc"
                            "logger.cc"
                            "util.cc"
                            "bfox_receiver.cc"
                            "gpio_control.cc"
                            "task.cc"
                            "i2c_util.cc"
                            "st7032.cc"
                            "beacon_receive_task.cc"
                            "file_system.cc"
                            "receiver_setting.cc"
                    INCLUDE_DIRS "")

component_compile_options(-Wno-error=format= -Wno-format)


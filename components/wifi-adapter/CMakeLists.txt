idf_component_register(SRCS "keyboard.c" "keyboard_A7100.c" "keyboard_PC1715.c" "cmdline.c" "wlan.c" "nvs.c" "webserver.c" "globalvars.c" "osd.c" "vga.c" "capture.c" "main.c" "highint5.S"
                    REQUIRES "driver" "esp_lcd" "soc" "nvs_flash" "esp_wifi" "esp_http_server" "app_update" "esp_partition" "wpa_supplicant"
                    INCLUDE_DIRS "include"
                    EMBED_TXTFILES "html/index.html")
target_link_libraries(${COMPONENT_TARGET} "-u ld_include_xt_highint5")

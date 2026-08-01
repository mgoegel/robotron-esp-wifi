#pragma once
#include <stdint.h>
#include <stdbool.h>

void setup_keyboard_PC1715();
void send_key_PC1715(uint8_t count, char* ch, bool keyShift, bool keyAlt, bool keyCtrl);

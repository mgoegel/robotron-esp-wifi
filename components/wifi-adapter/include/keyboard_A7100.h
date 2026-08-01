#pragma once
#include <stdint.h>
#include <stdbool.h>

void setup_keyboard_A7100();
void send_key_A7100(uint8_t count, char* ch, bool keyShift, bool keyAlt, bool keyCtrl);

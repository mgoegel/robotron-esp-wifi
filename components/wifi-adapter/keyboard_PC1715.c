#include "keyboard_PC1715.h"
#include "pins.h"
#include <stdint.h>
#include <string.h>


void setup_keyboard_PC1715()
{
    // TODO: setup SPI3 (weil der den Takt mit sendet)
}

void send_key_PC1715(uint8_t count, char* ch, bool keyShift, bool keyAlt, bool keyCtrl)
{
    // TODO: taste decodieren und an SPI3 senden
}
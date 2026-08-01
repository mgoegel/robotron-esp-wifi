#include "keyboard.h"
#include "keyboard_A7100.h"
#include "keyboard_PC1715.h"
#include "globalvars.h"
#include <stdint.h>
#include <stdbool.h>

void setup_keyboard()
{
    switch(ACTIVESYS)
    {
        case 0: setup_keyboard_A7100(); break;
        case 1: setup_keyboard_PC1715(); break;
    }
}

bool keyShift = false;
bool keyCtrl = false;
bool keyAlt = false;

void send_key(uint8_t bytecount, char* character)
{
    if (!strcmp(&character[1],"Control"))
    {
        keyCtrl = character[0]=='+';
        return;
    }

    if (!strcmp(&character[1],"Shift"))
    {
        keyShift = character[0]=='+';
        return;
    }

    if (!strcmp(&character[1],"Alt"))
    {
        keyAlt = character[0]=='+';
        return;
    }

    if (character[0]=='-') return;

    switch(ACTIVESYS)
    {
        case 0: send_key_A7100(bytecount-1, &character[1], keyShift, keyAlt, keyCtrl); break;
        case 1: send_key_PC1715(bytecount-1, &character[1], keyShift, keyAlt, keyCtrl); break;
    }
}

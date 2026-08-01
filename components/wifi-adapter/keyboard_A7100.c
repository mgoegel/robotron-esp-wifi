#include "keyboard_A7100.h"
#include "pins.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <string.h>

static QueueHandle_t uart_rcv_queue;

void uart_task(void*) 
{
    uart_event_t event;
    uint8_t inp;
    uint8_t cmd[4];
    uint8_t cnt = 0;
    uint8_t reply[4];
    
    while (1)
    {
        if (xQueueReceive(uart_rcv_queue, (void *)&event, portMAX_DELAY)) 
        {
            if (event.type==UART_DATA)
            {
                while (uart_read_bytes(UART_NUM_1, &inp, 1, portMAX_DELAY)>0)
                {
                    //printf("uart-in=%02x\n",inp);
                    cmd[cnt++] = inp;
                    if (inp==0)
                    {
                        cnt = 0;
                    }
                    switch (cnt)
                    {
                        case 1:
                            switch (cmd[0]) 
                            {
                                case 0x20:
                                    //XON
                                    cnt = 0;
                                    break;
                                case 0x44:
                                    //XOFF
                                    cnt = 0;
                                    break;
                                case 0x52:
                                    //Status
                                    cnt = 0;
                                    reply[0]=0x1B;
                                    reply[1]=0x5B;
                                    reply[2]=0x30;
                                    reply[3]=0x6E;
                                    uart_write_bytes(UART_NUM_1, reply, 4);
                                    break;
                            }
                            break;
                        case 2:
                            switch (cmd[1]) 
                            {
                                case 0x20:
                                    // ALT-LED ein
                                    cnt = 0;
                                    break;
                                case 0x44:
                                    // MOD2-LED ein
                                    cnt = 0;
                                    break;
                                case 0x52:
                                    // INS-MODE-LED ein
                                    cnt = 0;
                                    break;
                                case 0x63:
                                    // Reset
                                    if (cmd[0] == 0x1b) 
                                    {
                                        cnt = 0;
                                        reply[0]=0x11;
                                        uart_write_bytes(UART_NUM_1, reply, 1);
                                    }
                                    break;
                            }
                            break;
                        case 3:
                            switch (cmd[2]) 
                            {
                                case 0x20:
                                    // ALT-LED aus
                                    cnt = 0;
                                    break;
                                case 0x44:
                                    // MOD2-LED aus
                                    cnt = 0;
                                    break;
                                case 0x52:
                                    // INS-MODE-LED aus
                                    cnt = 0;
                                    break;
                                case 0x55:
                                    // RESET
                                    cnt = 0;
                                    reply[0]=0x11;
                                    uart_write_bytes(UART_NUM_1, reply, 1);
                                    break;
                            }
                            break;
                        case 4:
                            if (cmd[3]==0x6E && cmd[0]==0x1B) 
                            {
                                reply[0]=0x1B;
                                reply[1]=0x5B;
                                reply[2]=0x30;
                                reply[3]=0x6E;
                                uart_write_bytes(UART_NUM_1, reply, 4);
                            }
                            cnt = 0;
                            break;
                    }
                }
            }
        }
    }
    vTaskDelete(NULL);
}

void setup_keyboard_A7100()
{
    const uart_config_t uart_config = 
    {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, PIN_NUM_KEYBOARD1, PIN_NUM_KEYBOARD2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_rx_timeout(UART_NUM_1, 1));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 256, 256, 20, &uart_rcv_queue, 0));
    xTaskCreate(uart_task, "keyboard_task", 4096, NULL, 10, NULL);
}

void sendByte(uint8_t byte)
{
    uart_write_bytes(UART_NUM_1, &byte, 1);
}

void sendBytes(const uint8_t bytes[], uint16_t size)
{
    uart_write_bytes(UART_NUM_1, bytes, size);
}

void send_key_A7100(uint8_t count, char* ch, bool keyShift, bool keyAlt, bool keyCtrl)
{
/*    printf("Taste \"%s\" (",ch);
    for (int a=0;a<count;a++) printf(" %02x ",ch[a]);
    printf(")\n");
*/

    if (count==1)
    {
        switch (ch[0]) 
        {
            case '0':
                sendByte(keyCtrl ? 0x8A : keyShift ? 0x3D : 0x30);
                return;
            case '1':
                sendByte(keyCtrl ? 0x97 : keyShift ? 0x21 : 0x31);
                return;
            case '2':
                sendByte(keyCtrl ? 0x96 : keyShift ? 0x22 : 0x32);
                return;
            case '3':
                sendByte(keyShift ? (keyAlt ? 0xC0 : 0x40) : 0x33);
                return;
            case '4':
                sendByte(keyCtrl ? 0x84 : keyShift ? 0x24 : 0x34);
                return;
            case '5':
                sendByte(keyCtrl ? 0x82 : keyShift ? 0x25 : 0x35);
                return;
            case '6':
                sendByte(keyCtrl ? 0x80 : keyShift ? 0x26 : 0x36);
                return;
            case '7':
                sendByte(keyCtrl ? 0x81 : keyShift ? 0x2F : 0x37);
                return;
            case '8':
                sendByte(keyCtrl ? 0x83 : keyShift ? 0x28 : 0x38);
                return;
            case '9':
                sendByte(keyCtrl ? 0x85 : keyShift ? 0x29 : 0x39);
                return;
            case 'a' ... 'z':
            case 'A' ... 'Z':
                sendByte(keyCtrl ? ch[0]-'a'+1 : keyAlt ? ch[0]+0x80 : ch[0]);
                return;
            default: 
                sendByte(ch[0]);
                return;
        }
    }

    if (count==2 && ch[0]=='F' && ch[1]>='1' && ch[1]<='9')
    {
        sendBytes((const uint8_t[]){0x1b,0x4f,ch[1] + (ch[1]>=0x35 ? 0x3b : 0x1f)},3);
        return;
    }

    if (count==3 && ch[0]=='F' && ch[1]=='1' && ch[2]>='0' && ch[2]<='2')
    {
        sendBytes((const uint8_t[]){0x1b,0x4f,ch[2]+0x45},3);
        return;
    }

    if (!strcmp(ch,"ArrowUp"))
    {
        if (keyCtrl) 
            sendByte(0x9b);
        else
            sendBytes((const uint8_t[]){0x1b,0x58,0x41},3);
        return;
    }

    if (!strcmp(ch,"ArrowDown"))
    {
        if (keyCtrl) 
            sendByte(0x9f);
        else
            sendBytes((const uint8_t[]){0x1b,0x58,0x42},3);
        return;
    }

    if (!strcmp(ch,"ArrowRight"))
    {
        if (keyCtrl) 
            sendByte(0x9d);
        else
            sendBytes((const uint8_t[]){0x1b,0x58,0x43},3);
        return;
    }

    if (!strcmp(ch,"ArrowLeft"))
    {
        if (keyCtrl) 
            sendByte(0x8d);
        else
            sendBytes((const uint8_t[]){0x1b,0x58,0x44},3);
        return;
    }

    if (!strcmp(ch,"Home"))
    {
        if (keyShift) 
            sendBytes((const uint8_t[]){0x1b,0x4f,0x79},3);
        else
            sendBytes((const uint8_t[]){0x1b,0x4f,0x78},3);
        return;
    }

    if (!strcmp(ch,"End"))
    {
        if (keyShift) 
            sendBytes((const uint8_t[]){0x1b,0x5b,0x48},3);
        else
            sendBytes((const uint8_t[]){0x1b,0x4f,0x7a},3);
        return;
    }

    if (!strcmp(ch,"Escape"))
    {
        if (keyShift) 
            sendBytes((const uint8_t[]){0x1b,0x63},2);
        else
            sendByte(0x1b);
        return;
    }

    if (!strcmp(ch,"Enter"))
    {
        if (keyCtrl) 
            sendBytes((const uint8_t[]){0x1b,0x4f,0x4d},3);
        else
            sendByte(0x0d);
        return;
    }

    if (!strcmp(ch,"Tab"))
    {
        if (keyShift) 
            sendBytes((const uint8_t[]){0x1b,0x5b,0x5a},3);
        else
            sendByte(0x09);
        return;
    }

    if (!strcmp(ch,"Backspace"))
    {
        if (keyCtrl) 
            sendByte(0x88);
        else
        {
            if (keyShift) 
                sendByte(0x18);
            else
                sendByte(0x08);
        }
        return;
    }

    if (!strcmp(ch,"Delete"))
    {
        if (keyShift) 
            sendBytes((const uint8_t[]){0x1b,0x5b,0x4d},3);
        else
            sendByte(0x7f);
        return;
    }

    if (!strcmp(ch,"PageUp"))
    {
        if (keyShift) 
            sendBytes((const uint8_t[]){0x1b,0x5b,0x4f},3);
        else
            sendBytes((const uint8_t[]){0x1b,0x5b,0x4e},3);
        return;
    }

    if (!strcmp(ch,"PageDown"))
    {
        if (keyShift) 
            sendBytes((const uint8_t[]){0x1b,0x4f,0x4f},3);
        else
            sendBytes((const uint8_t[]){0x1b,0x4f,0x4e},3);
        return;
    }

    if (count==2 && ch[0]==0xc3)
    {
        switch (ch[1])
        {
            case 0xbc: // ü
                sendByte(keyAlt ? 0xFD : 0x5D);
                return;
            case 0x9c: // Ü
                sendByte(keyAlt ? 0xDD : 0x7D);
                return;
            case 0xb6: // ö
                sendByte(keyAlt ? 0xFC : 0x5C);
                return;
            case 0x96: // Ö
                sendByte(keyAlt ? 0xDC : 0x7C);
                return;
            case 0xa4: // ä
                sendByte(keyAlt ? 0xFB : 0x5B);
                return;
            case 0x84: // Ä
                sendByte(keyAlt ? 0xDB : 0x7B);
                return;
        }
    }

    printf("Unbehandelte Taste \"%s\" (",ch);
    for (int a=0;a<count;a++) printf(" %02x ",ch[a]);
    printf(")\n");
}
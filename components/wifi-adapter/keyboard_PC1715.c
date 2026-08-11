// #include "pins.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/queue.h"
// #include "driver/gpio.h"
// #include "esp_rom/rom.h"

#include "keyboard_PC1715.h"
#include "pins.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include <driver/gpio.h>
#include "soc/spi_struct.h"
#include "soc/system_struct.h"
#include "soc/gpio_reg.h"
#include "esp_private/gpio.h"
#include "pthread.h"


void setup_keyboard_PC1715()
{
    gpio_config_t pincfg =
    {
        .pin_bit_mask = 1ULL<<PIN_NUM_KEYBOARD1 | 1ULL<<PIN_NUM_KEYBOARD2,
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&pincfg));

    // SPI3 initialisieren
    SYSTEM.perip_clk_en0.spi3_clk_en = 1;
    SYSTEM.perip_rst_en0.spi3_rst = 0;
    GPSPI3.user.val = 0;
    GPSPI3.user1.val = 0;
    GPSPI3.user2.val = 0;
    GPSPI3.ctrl.val = 0;
    GPSPI3.clk_gate.val = 0;
    GPSPI3.misc.val = 0;

    GPSPI3.user.doutdin = 1; // einfaches SPI
    GPSPI3.clk_gate.clk_en = 1; // Takt aktivieren
    GPSPI3.clk_gate.mst_clk_active = 1;
    GPSPI3.clk_gate.mst_clk_sel = 0;

    GPSPI3.misc.ck_idle_edge = 1; // Datenänderung bei steigender Takt-Flanke und Negation
    GPSPI3.ctrl.wr_bit_order = 1; // Bit-Reihenfolge wie bei RS232

    GPSPI3.clock.clkcnt_l = 63; // 39KBit/s: weiter runter lässt sich das mit unserem Systemtakt nicht runterbremsen.
    GPSPI3.clock.clkcnt_h = 31; // Für den Z80 SIO sollte das aber kein Problem sein
    GPSPI3.clock.clkcnt_n = 63;
    GPSPI3.clock.clkdiv_pre = 15;
    GPSPI3.clock.clk_equ_sysclk = 0;

    gpio_func_sel(PIN_NUM_KEYBOARD1,1); // Pin-Konfi: IO-Matrix
    gpio_func_sel(PIN_NUM_KEYBOARD2,1);

    esp_rom_gpio_connect_out_signal(PIN_NUM_KEYBOARD1,66,0,0); // Pinkonfi: SPI3-CLK  = Keyboard Clock
    esp_rom_gpio_connect_out_signal(PIN_NUM_KEYBOARD2,68,1,0); // Pinkonfi: SPI3-MOSI = Keyboard Data (und Negation)

    GPSPI3.ms_dlen.ms_data_bitlen = 21; // 22 Bits
    GPSPI3.cmd.update = 1; // Einstellungen in den SPI-Kontroller übernehmen
}

void send_2_bytes(uint8_t status, uint8_t keycode)
{
    // Wenn der SPI3 noch mit der letzen Übertragung kämpft: warten
    while (GPSPI3.cmd.usr) usleep(10000);
    // Daten in das Sendepuffer-Register schreiben
    GPSPI3.data_buf[0] = 0x801 | (keycode^0xff)<<12 | (status^0xff)<<1;
    // und los gehts!
    GPSPI3.cmd.usr = 1; 
}

void send_key_PC1715(uint8_t count, char* ch, bool keyShift, bool keyAlt, bool keyCtrl)
{
    uint8_t status = 0xe0;
    if (keyShift) status |= 2;
    if (keyCtrl) status |= 1;

    if (count==1)
    {
        switch (ch[0]) 
        {
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '0' ... '9':
                send_2_bytes(status, ch[0]);
                return;
            // TODO: alle Tasten testen, eventuell hier Umwandlung einfügen
        }
    }

    if (!strcmp(ch,"Enter"))
    {
        send_2_bytes(status, keyShift ? 0x9d : 0x9e);
        return;
    }

    if (!strcmp(ch,"F1"))
    {
        send_2_bytes(status, 0xd1);
        return;
    }

    if (!strcmp(ch,"F2"))
    {
        send_2_bytes(status, 0xd2);
        return;
    }

    if (!strcmp(ch,"F3"))
    {
        send_2_bytes(status, 0xd3);
        return;
    }

    if (!strcmp(ch,"F4"))
    {
        send_2_bytes(status, 0xd4);
        return;
    }

    if (!strcmp(ch,"F5"))
    {
        send_2_bytes(status, 0xcf);
        return;
    }

    if (!strcmp(ch,"F6"))
    {
        send_2_bytes(status, 0xa0);
        return;
    }

    if (!strcmp(ch,"F7"))
    {
        send_2_bytes(status, 0xa1);
        return;
    }

    if (!strcmp(ch,"F8"))
    {
        send_2_bytes(status, 0xa2);
        return;
    }

    if (!strcmp(ch,"F9"))
    {
        send_2_bytes(status, 0xa3);
        return;
    }

    if (!strcmp(ch,"F10"))
    {
        send_2_bytes(status, 0x83);
        return;
    }

    if (!strcmp(ch,"F11"))
    {
        send_2_bytes(status, 0xc1);
        return;
    }

    if (!strcmp(ch,"F12"))
    {
        send_2_bytes(status, 0xc2);
        return;
    }

    if (!strcmp(ch,"ArrowUp"))
    {
        send_2_bytes(status, 0x8b);
        return;
    }

    if (!strcmp(ch,"ArrowDown"))
    {
        send_2_bytes(status, 0x83);
        return;
    }

    if (!strcmp(ch,"ArrowRight"))
    {
        send_2_bytes(status, 0x86);
        return;
    }

    if (!strcmp(ch,"ArrowLeft"))
    {
        send_2_bytes(status, 0x88);
        return;
    }

    if (!strcmp(ch,"Home"))
    {
        send_2_bytes(status, 0x8c);
        return;
    }

    if (!strcmp(ch,"End"))
    {
        send_2_bytes(status, 0xdd);
        return;
    }

/*  TODO: fehlende Tasten implementieren
    if (!strcmp(ch,"Escape"))
    {
        send_2_bytes(status, );
        return;
    }

    if (!strcmp(ch,"Tab"))
    {
        send_2_bytes(status, );
        return;
    }

    if (!strcmp(ch,"Backspace"))
    {
        send_2_bytes(status, );
        return;
    }

    if (!strcmp(ch,"Delete"))
    {
        send_2_bytes(status, );
        return;
    }
*/

    printf("Unbehandelte Taste \"%s\" (",ch);
    for (int a=0;a<count;a++) printf(" %02x ",ch[a]);
    printf(")\n");
}




// // Queue for keyboard transmission
// QueueHandle_t key_queue = NULL;

// #define KEY_QUEUE_SIZE 2*5

// // State machine for bit transmission
// typedef enum {
//     STATE_IDLE,
//     STATE_START_BIT,
//     STATE_DATA_BITS,
//     STATE_STOP_BIT1,
//     STATE_STOP_BIT2
// } transmission_state_t;

// typedef struct {
//     uint8_t data;
//     uint8_t bit_index;
//     transmission_state_t state;
// } transmission_context_t;

// static transmission_context_t tx_ctx;

// static void send_bit(bool bit) {
//     gpio_set_level(PIN_NUM_KEYBOARD2, bit);
//     gpio_set_level(PIN_NUM_KEYBOARD1, 1);
//     vTaskDelay(20 / portTICK_PERIOD_MS);  // 20us
//     gpio_set_level(PIN_NUM_KEYBOARD1, 0);
//     vTaskDelay(10 / portTICK_PERIOD_MS);  // 10us
// }

// void send_key_task(void *pvParameters) {
//     uint8_t key_code;
//     while (1) {
//         // Wait for key code on queue
//         if (xQueueReceive(key_queue, &key_code, portMAX_DELAY)) {
//             // Send status byte (0x00)
//             tx_ctx.data = 0x00;
//             tx_ctx.bit_index = 0;
//             tx_ctx.state = STATE_START_BIT;
            
//             // Send key byte
//             tx_ctx.data = key_code;
//             tx_ctx.bit_index = 0;
//             tx_ctx.state = STATE_START_BIT;
            
//             // Process until transmission complete
//             while (tx_ctx.state != STATE_IDLE) {
//                 switch (tx_ctx.state) {
//                     case STATE_IDLE:
//                         break;
//                     case STATE_START_BIT:
//                         send_bit(1);  // Start bit (high)
//                         tx_ctx.state = STATE_DATA_BITS;
//                         break;
                        
//                     case STATE_DATA_BITS:
//                         send_bit((tx_ctx.data >> tx_ctx.bit_index) & 1);
//                         tx_ctx.bit_index++;
//                         if (tx_ctx.bit_index >= 8) {
//                             tx_ctx.state = STATE_STOP_BIT1;
//                         }
//                         break;
                        
//                     case STATE_STOP_BIT1:
//                         send_bit(0);  // Stop bit 1 (low)
//                         tx_ctx.state = STATE_STOP_BIT2;
//                         break;
                        
//                     case STATE_STOP_BIT2:
//                         send_bit(0);  // Stop bit 2 (low)
//                         tx_ctx.state = STATE_IDLE;
//                         break;
//                 }
//                 vTaskDelay(1);  // Allow other tasks to run
//             }
//         }
//     }
// }

// void send_key_PC1715(uint8_t key_code) {
//     // Enqueue key code for transmission task
//     xQueueSend(key_queue, &key_code, portMAX_DELAY);
// }

// void setup_keyboard_PC1715(void) {
//     // Configure CLK and DATA pins as outputs (pad selection is handled by the driver on ESP32-S3)
//     ESP_ERROR_CHECK(gpio_set_direction(PIN_NUM_KEYBOARD1, GPIO_MODE_OUTPUT));
//     ESP_ERROR_CHECK(gpio_set_direction(PIN_NUM_KEYBOARD2, GPIO_MODE_OUTPUT));
    
//     gpio_set_level(PIN_NUM_KEYBOARD1, 0);
//     gpio_set_level(PIN_NUM_KEYBOARD2, 0);
    
//     // Initialize transmission context
//     tx_ctx.state = STATE_IDLE;
//     tx_ctx.bit_index = 0;
    
//     // Create queue for key transmission
//     key_queue = xQueueCreate(KEY_QUEUE_SIZE, sizeof(uint8_t));
//     configASSERT(key_queue);
    
//     // Create dedicated transmission task
//     xTaskCreate(send_key_task, "key_transmit", 2048, NULL, 5, NULL);
// }


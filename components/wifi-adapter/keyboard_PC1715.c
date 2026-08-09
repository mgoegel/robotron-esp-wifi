#include "pins.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
// #include "esp_rom/rom.h"


// Queue for keyboard transmission
QueueHandle_t key_queue = NULL;

#define KEY_QUEUE_SIZE 2*5

// State machine for bit transmission
typedef enum {
    STATE_IDLE,
    STATE_START_BIT,
    STATE_DATA_BITS,
    STATE_STOP_BIT1,
    STATE_STOP_BIT2
} transmission_state_t;

typedef struct {
    uint8_t data;
    uint8_t bit_index;
    transmission_state_t state;
} transmission_context_t;

static transmission_context_t tx_ctx;

static void send_bit(bool bit) {
    gpio_set_level(PIN_NUM_KEYBOARD2, bit);
    gpio_set_level(PIN_NUM_KEYBOARD1, 1);
    vTaskDelay(20 / portTICK_PERIOD_MS);  // 20us
    gpio_set_level(PIN_NUM_KEYBOARD1, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);  // 10us
}

void send_key_task(void *pvParameters) {
    uint8_t key_code;
    while (1) {
        // Wait for key code on queue
        if (xQueueReceive(key_queue, &key_code, portMAX_DELAY)) {
            // Send status byte (0x00)
            tx_ctx.data = 0x00;
            tx_ctx.bit_index = 0;
            tx_ctx.state = STATE_START_BIT;
            
            // Send key byte
            tx_ctx.data = key_code;
            tx_ctx.bit_index = 0;
            tx_ctx.state = STATE_START_BIT;
            
            // Process until transmission complete
            while (tx_ctx.state != STATE_IDLE) {
                switch (tx_ctx.state) {
                    case STATE_IDLE:
                        break;
                    case STATE_START_BIT:
                        send_bit(1);  // Start bit (high)
                        tx_ctx.state = STATE_DATA_BITS;
                        break;
                        
                    case STATE_DATA_BITS:
                        send_bit((tx_ctx.data >> tx_ctx.bit_index) & 1);
                        tx_ctx.bit_index++;
                        if (tx_ctx.bit_index >= 8) {
                            tx_ctx.state = STATE_STOP_BIT1;
                        }
                        break;
                        
                    case STATE_STOP_BIT1:
                        send_bit(0);  // Stop bit 1 (low)
                        tx_ctx.state = STATE_STOP_BIT2;
                        break;
                        
                    case STATE_STOP_BIT2:
                        send_bit(0);  // Stop bit 2 (low)
                        tx_ctx.state = STATE_IDLE;
                        break;
                }
                vTaskDelay(1);  // Allow other tasks to run
            }
        }
    }
}

void send_key_PC1715(uint8_t key_code) {
    // Enqueue key code for transmission task
    xQueueSend(key_queue, &key_code, portMAX_DELAY);
}

void setup_keyboard_PC1715(void) {
    // Configure CLK and DATA pins as outputs (pad selection is handled by the driver on ESP32-S3)
    ESP_ERROR_CHECK(gpio_set_direction(PIN_NUM_KEYBOARD1, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(PIN_NUM_KEYBOARD2, GPIO_MODE_OUTPUT));
    
    gpio_set_level(PIN_NUM_KEYBOARD1, 0);
    gpio_set_level(PIN_NUM_KEYBOARD2, 0);
    
    // Initialize transmission context
    tx_ctx.state = STATE_IDLE;
    tx_ctx.bit_index = 0;
    
    // Create queue for key transmission
    key_queue = xQueueCreate(KEY_QUEUE_SIZE, sizeof(uint8_t));
    configASSERT(key_queue);
    
    // Create dedicated transmission task
    xTaskCreate(send_key_task, "key_transmit", 2048, NULL, 5, NULL);
}


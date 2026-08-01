#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "globalvars.h"
#include "capture.h"
#include "main.h"
#include "pins.h"
#include "osd.h"
#include "keyboard.h"

#include "driver/gpio.h"
#include "esp_private/periph_ctrl.h"
#include <driver/spi_master.h>
#include <xtensa_context.h>
#include <soc/gpio_reg.h>
#include <soc/gdma_reg.h>
#include <soc/spi_reg.h>
#include "esp_intr_alloc.h"
#include <rom/ets_sys.h>
#include <esp_http_server.h>
#include "esp_wps.h"

static httpd_handle_t web_server = NULL;

void start_webserver();

extern const char index_html[] asm ("_binary_index_html_start"); 

uint8_t empty = 0;

// Webseiten-Handler BMP-Screenshot
static esp_err_t img_sock_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) 
    {
        return ESP_OK;
    }
    char buf[128];
    httpd_ws_frame_t pkt=
    {
        .payload=(uint8_t*)buf,
        .type=HTTPD_WS_TYPE_BINARY
    };

    httpd_ws_recv_frame(req, &pkt, 128);

    switch (buf[0])
    {
        case 1:    // Vorgabe-Bildrand: ganzes Bild. Werden beim Samplen nicht mehr geändert.
            img_x_min = 0;
            img_x_max = ABG_XRes-1;
            img_y_min = 0;
            img_y_max = ABG_YRes-1;
            break;
        case 2:   // Vorgabe-Bildrand: nichts. Werden beim Samplen gesetzt, sobald ein Pixel unterschiedlich ist
            img_x_min = ABG_XRes-1;
            img_x_max = 0;
            img_y_min = ABG_YRes-1;
            img_y_max = 0;
            break;
        case 3:  // Handler für Tastatur
            buf[pkt.len]=0;
            send_key(pkt.len-1, &buf[1]);
            break;
        default: 
            for (int a=0;a<8;a++) buf[a] = 0;
            pkt.len = 8;
            httpd_ws_send_frame(req, &pkt);
            return ESP_OK;
    }

	update_pixel_steplist();
    if (web_capture_image()==0)
    {
        if (empty==0)
        {
            img_x_min = 0;
            img_x_max = ABG_XRes-1;
            img_y_min = 0;
            img_y_max = ABG_YRes-1;
            memset(img_data,0,ABG_YRes * stride);
            empty = 1;
        }
    }
    else
    {
        empty = 0;
    }

    if (img_x_min>img_x_max)
    {
        memset(buf,0,8);
        pkt.len = 8;
        pkt.final = true;
        if (httpd_ws_send_frame(req, &pkt)) return ESP_OK;
        return ESP_OK;
    }
    uint16_t* a = (uint16_t*)&buf[0];
    a[0] = img_x_min & ~15;
    a[1] = img_y_min;
    a[2] = ((img_x_max-img_x_min) | 15)+1;
    a[3] = (img_y_max-img_y_min)+1;

//    printf("Frame x=%d, y=%d, width=%d, height=%d\n", a[0],a[1],a[2],a[3]);
    pkt.len = 8;
    pkt.final = 0;
    pkt.fragmented = 1;
    pkt.type = HTTPD_WS_TYPE_BINARY;
    if (httpd_ws_send_frame(req, &pkt)) return ESP_OK;
    pkt.type = HTTPD_WS_TYPE_CONTINUE;
    
    for (uint32_t i = img_y_min;i<=img_y_max;i++)
    {
        pkt.payload = (uint8_t*)(((uint32_t)img_data) + ((img_x_min>>2) & ~0x3) + i*stride);
        pkt.len = (((img_x_max - img_x_min) | 15)+1) / 4;
        if (i==img_y_max) pkt.final = 1;
        if (httpd_ws_send_frame(req, &pkt)!=ESP_OK) return ESP_OK;
    }
    return ESP_OK;
}

// Webseiten-Handler Hauptseite
static esp_err_t mainpage_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_sendstr_chunk(req, index_html);
    httpd_resp_sendstr_chunk(req, NULL);
    return ESP_OK;
}

static const httpd_uri_t mainpage = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = mainpage_get_handler,
};

static const httpd_uri_t img_sock = {
    .uri       = "/screen.soc",
    .method    = HTTP_GET,
    .handler   = img_sock_handler,
    .is_websocket = true,
};

// Webserver starten
void start_webserver()
{
	if (web_server != NULL)
	{
		httpd_stop(web_server);
		web_server = NULL;
	}
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.core_id = 0;

    if (httpd_start(&web_server, &config) != ESP_OK) 
    {
		web_server = NULL;
        return;
    }
    httpd_register_uri_handler(web_server, &mainpage);
    httpd_register_uri_handler(web_server, &img_sock);
}


void stop_webserver()
{
	if (web_server != NULL)
	{
		httpd_stop(web_server);
		web_server = NULL;
	}
}
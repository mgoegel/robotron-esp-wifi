
#include "globalvars.h"
#include "main.h"
#include "osd.h"
#include "vga.h"
#include "capture.h"
#include "nvs.h"
#include "wlan.h"
#include "keyboard.h"

// Hauptprogramm
void IRAM_ATTR app_main(void)
{
	setup_flash();
	restore_settings();

	alloc_vga_buffer();
	setup_vga_buffer();
	setup_vga_mode();

	stride = (ABG_XRes / 4) + ((ABG_XRes & 3)!=0 ? 1 : 0);
	img_data = heap_caps_malloc(ABG_YRes * stride, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);

	img_line_done = heap_caps_malloc(ABG_YRes, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);

    setup_abg();
	xTaskCreatePinnedToCore(osd_task,"osd_task",8000,NULL,0,NULL,0);
	setup_wlan(wlan_mode);
	#ifndef DEBUG
	setup_keyboard();
	#endif
}


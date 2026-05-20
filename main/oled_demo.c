#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h"

static const char *TAG = "oled";

#define I2C_SDA          GPIO_NUM_5
#define I2C_SCL          GPIO_NUM_6
#define OLED_ADDR        0x3C
#define OLED_WIDTH       72
#define OLED_HEIGHT      40
#define OLED_PAGES       5
#define OLED_X_OFFSET    30

// ── Bit-bang I2C ───────────────────────────────────
static void i2c_delay(void) { esp_rom_delay_us(5); }

static void i2c_start(void) {
    gpio_set_direction(I2C_SDA, GPIO_MODE_OUTPUT);
    gpio_set_level(I2C_SDA, 1); gpio_set_level(I2C_SCL, 1);
    i2c_delay();
    gpio_set_level(I2C_SDA, 0); i2c_delay();
    gpio_set_level(I2C_SCL, 0);
}
static void i2c_stop(void) {
    gpio_set_direction(I2C_SDA, GPIO_MODE_OUTPUT);
    gpio_set_level(I2C_SDA, 0); gpio_set_level(I2C_SCL, 1);
    i2c_delay();
    gpio_set_level(I2C_SDA, 1); i2c_delay();
}
static uint8_t i2c_write(uint8_t d) {
    gpio_set_direction(I2C_SDA, GPIO_MODE_OUTPUT);
    for (int i = 0; i < 8; i++, d <<= 1) {
        gpio_set_level(I2C_SDA, (d & 0x80) ? 1 : 0);
        i2c_delay(); gpio_set_level(I2C_SCL, 1);
        i2c_delay(); gpio_set_level(I2C_SCL, 0);
    }
    gpio_set_direction(I2C_SDA, GPIO_MODE_INPUT);
    gpio_set_level(I2C_SCL, 1); i2c_delay();
    uint8_t a = gpio_get_level(I2C_SDA);
    gpio_set_level(I2C_SCL, 0);
    return a;
}
static void oled_cmd(uint8_t c) {
    i2c_start(); i2c_write(OLED_ADDR<<1); i2c_write(0x00); i2c_write(c); i2c_stop();
}
static void oled_data(const uint8_t *b, int n) {
    i2c_start(); i2c_write(OLED_ADDR<<1); i2c_write(0x40);
    for (int i=0;i<n;i++) i2c_write(b[i]);
    i2c_stop();
}

// ── SH1106 Init ────────────────────────────────────
static void oled_init(void) {
    oled_cmd(0xAE); // display off
    oled_cmd(0xD5); oled_cmd(0x80); // clock
    oled_cmd(0xA8); oled_cmd(0x27); // mux 40-1
    oled_cmd(0xD3); oled_cmd(0x00); // offset = 0
    oled_cmd(0x40 | 12);             // start line = 12 — matches panel offset
    oled_cmd(0x8D); oled_cmd(0x14); // charge pump
    oled_cmd(0xA1);                   // seg remap
    oled_cmd(0xC8);                   // com scan reverse (original)
    oled_cmd(0xDA); oled_cmd(0x12); // com pins
    oled_cmd(0x81); oled_cmd(0x7F); // contrast
    oled_cmd(0xD9); oled_cmd(0x22); // precharge
    oled_cmd(0xDB); oled_cmd(0x20); // vcomh
    oled_cmd(0xA4);                   // resume
    oled_cmd(0xA6);                   // normal
    oled_cmd(0xAF);                   // display on
    ESP_LOGI(TAG, "init ok");
}

// ── Framebuffer ────────────────────────────────────
static uint8_t fb[OLED_WIDTH * OLED_PAGES];
static void clr(void) { memset(fb, 0, sizeof(fb)); }
static void px(int x, int y, int on) {
    if (x<0||x>=OLED_WIDTH||y<0||y>=OLED_HEIGHT) return;
    if(on) fb[x+(y>>3)*OLED_WIDTH]|=(1<<(y&7));
    else   fb[x+(y>>3)*OLED_WIDTH]&=~(1<<(y&7));
}
static void flush(void) {
    uint8_t cl=0x00|(OLED_X_OFFSET&0x0F), ch=0x10|(OLED_X_OFFSET>>4);
    for(int p=0;p<OLED_PAGES;p++) {
        oled_cmd(0xB0|p); oled_cmd(cl); oled_cmd(ch);
        oled_data(fb+p*OLED_WIDTH, OLED_WIDTH);
    }
}

// ── 5x7 Font (essential chars) ────────────────────
static const uint8_t FONT[96][5] = {
 [0]={0},[1]={0,0,0x5F},[14]={0,0x60,0x60},[15]={0x20,0x10,8,4,2},
 [16]={0x3E,0x51,0x49,0x45,0x3E},[17]={0,0x42,0x7F,0x40},
 [18]={0x42,0x61,0x51,0x49,0x46},[19]={0x21,0x41,0x45,0x4B,0x31},
 [20]={0x18,0x14,0x12,0x7F,0x10},[21]={0x27,0x45,0x45,0x45,0x39},
 [22]={0x3C,0x4A,0x49,0x49,0x30},[23]={1,0x71,9,5,3},
 [24]={0x36,0x49,0x49,0x49,0x36},[25]={6,0x49,0x49,0x29,0x1E},
 [26]={0,0x36,0x36},[33]={0x7E,0x11,0x11,0x11,0x7E},
 [34]={0x7F,0x49,0x49,0x49,0x36},[35]={0x3E,0x41,0x41,0x41,0x22},
 [36]={0x7F,0x41,0x41,0x22,0x1C},[37]={0x7F,0x49,0x49,0x49,0x41},
 [38]={0x7F,9,9,1,1},[40]={0x7F,8,8,8,0x7F},
 [41]={0,0x41,0x7F,0x41},[44]={0x7F,0x40,0x40,0x40,0x40},
 [47]={0x3E,0x41,0x41,0x41,0x3E},[48]={0x7F,9,9,9,6},
 [50]={0x7F,9,0x19,0x29,0x46},[51]={0x46,0x49,0x49,0x49,0x31},
 [52]={1,1,0x7F,1,1},[53]={0x3F,0x40,0x40,0x40,0x3F},
 [56]={0x63,0x14,8,0x14,0x63},[57]={3,4,0x78,4,3},
 [58]={0x61,0x51,0x49,0x45,0x43},[65]={0x20,0x54,0x54,0x54,0x78},
 [66]={0x7F,0x48,0x44,0x44,0x38},[67]={0x38,0x44,0x44,0x44,0x20},
 [69]={0x38,0x54,0x54,0x54,0x18},[72]={0x7F,8,4,4,0x78},
 [73]={0,0x44,0x7D,0x40},[76]={0,0x41,0x7F,0x40},
 [78]={0x7C,8,4,4,0x78},[79]={0x38,0x44,0x44,0x44,0x38},
 [82]={0x7C,8,4,4,8},[83]={0x48,0x54,0x54,0x54,0x20},
 [84]={4,0x3F,0x44,0x40,0x20},[85]={0x3C,0x40,0x40,0x20,0x7C},
 [88]={0x44,0x28,0x10,0x28,0x44},[90]={0x44,0x64,0x54,0x4C,0x44},
};
static void chr(int x, int y, char c) {
    if (c < ' ' || c > '~') return;
    const uint8_t *g = FONT[c - ' '];
    for (int i=0;i<5;i++) for (int j=0;j<7;j++) px(x+i,y+j,(g[i]>>j)&1);
}
static void txt(int x, int y, const char *s) { while(*s){chr(x,y,*s++);x+=6;} }
static void box(int x, int y, int w, int h) {
    for(int i=0;i<w;i++){px(x+i,y,1);px(x+i,y+h-1,1);}
    for(int i=0;i<h;i++){px(x,y+i,1);px(x+w-1,y+i,1);}
}

// ── Main ───────────────────────────────────────────
void app_main(void) {
    gpio_config_t g = {.pin_bit_mask=(1ULL<<I2C_SDA)|(1ULL<<I2C_SCL),
                       .mode=GPIO_MODE_OUTPUT_OD,
                       .pull_up_en=GPIO_PULLUP_ENABLE};
    gpio_config(&g);
    gpio_set_level(I2C_SDA,1); gpio_set_level(I2C_SCL,1);

    oled_init();
    clr();
    txt(1,1,"Hi!"); txt(1,16,"ABRobot"); txt(1,30,"OK!");
    box(0,0,OLED_WIDTH,OLED_HEIGHT);
    flush();
    ESP_LOGI(TAG,"done");

    int n=0;
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        clr();
        char b[16]; snprintf(b,sizeof(b),"%02d",n++);
        txt(4,2,b); txt(0,16,"ESP32C3"); txt(0,30,"BB");
        box(0,0,OLED_WIDTH,OLED_HEIGHT);
        flush();
    }
}

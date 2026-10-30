#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// ESP32-C3 只有一路 I2S，麦克风和功放共用时钟，采样率必须相同。
// 上行会在音频服务里重采样到 16 kHz。
#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// INMP441 L/R 接 GND，对应左声道。MAX98357 的 SD 接 3.3V 保持打开。
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_5
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_4
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_6
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_7

// SuperMini 板载 BOOT，按下为低电平。长按进入配网。GPIO8 是板载灯，不占用。
#define BOOT_BUTTON_GPIO GPIO_NUM_9

// 外接轻触开关：一端接 GPIO10，另一端接 GND。只负责开始聊天和回到待命。
#define CHAT_BUTTON_GPIO GPIO_NUM_10

#endif // _BOARD_CONFIG_H_

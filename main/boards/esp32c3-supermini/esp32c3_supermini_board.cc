#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "display/display.h"
#include "application.h"
#include "button.h"
#include "config.h"

#include <cstdio>
#include <string>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "Esp32c3Supermini"

static void SerialTextTask(void* /*arg*/) {
    std::string line;
    ESP_LOGI(TAG, "串口模拟：发送一行文字后回车，即可代替麦克风提问");
    while (true) {
        int ch = fgetc(stdin);
        if (ch == EOF) {
            vTaskDelay(pdMS_TO_TICKS(20));
            clearerr(stdin);
            continue;
        }
        if (ch == '\n' || ch == '\r') {
            if (!line.empty()) {
                Application::GetInstance().SubmitTextQuery(line);
                line.clear();
            }
            continue;
        }
        if (line.size() < 180) {
            line.push_back(static_cast<char>(ch));
        }
    }
}

class Esp32c3SuperminiBoard : public WifiBoard {
private:
    Button boot_button_;
    Button chat_button_;

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });
        boot_button_.OnLongPress([this]() {
            EnterWifiConfigMode();
        });

        // 松开即生效，按住也不会进入配网。
        chat_button_.OnPressUp([this]() {
            Application::GetInstance().ToggleStandby();
        });
    }

public:
    Esp32c3SuperminiBoard()
        : boot_button_(BOOT_BUTTON_GPIO, false, 2000), chat_button_(CHAT_BUTTON_GPIO, false, 0) {
        InitializeButtons();
        xTaskCreate(SerialTextTask, "serial_text", 4096, nullptr, 2, nullptr);
        ESP_LOGI(TAG, "ESP32-C3 SuperMini duplex I2S");
        ESP_LOGI(TAG, "聊天键 GPIO10：短按开始聊天，再按回到待命");
        ESP_LOGI(TAG, "串口打字提问已开启（短句，回车发送）");
    }

    virtual AudioCodec* GetAudioCodec() override {
        static NoAudioCodecDuplex audio_codec(
            AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN);
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        static NoDisplay display;
        return &display;
    }
};

DECLARE_BOARD(Esp32c3SuperminiBoard);

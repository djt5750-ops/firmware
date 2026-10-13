#include <Arduino.h>
#include <M5StickCPlus2.h>
#include <Adafruit_PN532.h>

// Definicja pinów I2C dla dolnego portu (Grove) w M5StickC Plus2
#define SDA_PIN 32
#define SCL_PIN 33

static Adafruit_PN532 fuzzer_nfc(SDA_PIN, SCL_PIN);

void run_rfid_fuzzer() {
    bool running = true;
    bool is_fuzzing = false;
    uint32_t current_uid = 0x00000000;

    // Inicjalizacja ekranu aplikacji
    StickCP2.Lcd.fillScreen(BLACK);
    StickCP2.Lcd.setTextColor(WHITE);
    StickCP2.Lcd.setTextSize(2);
    StickCP2.Lcd.setCursor(10, 10);
    StickCP2.Lcd.println("RFID Fuzzer PN532");

    // Inicjalizacja podłączonego modułu
    fuzzer_nfc.begin();
    if (!fuzzer_nfc.getFirmwareVersion()) {
        StickCP2.Lcd.setTextColor(RED);
        StickCP2.Lcd.println("\nBrak PN532! Sprawdz piny");
        delay(3000);
        return;
    }
    fuzzer_nfc.SAMConfig();

    StickCP2.Lcd.setCursor(10, 130);
    StickCP2.Lcd.setTextColor(GRAY);
    StickCP2.Lcd.print("M5: Start | B: Wyjdz");

    while (running) {
        StickCP2.update();

        // Przycisk główny M5 (BtnA) - Start / Pauza
        if (StickCP2.BtnA.wasPressed()) { 
            is_fuzzing = !is_fuzzing; 
            StickCP2.Lcd.fillRect(0, 50, 240, 50, BLACK);
        }
        // Przycisk boczny (BtnB) - Powrót do menu Bruce
        if (StickCP2.BtnB.wasPressed()) { 
            running = false; 
        }

        if (is_fuzzing) {
            uint8_t uid[4];
            uid[0] = (current_uid >> 24) & 0xFF;
            uid[1] = (current_uid >> 16) & 0xFF;
            uid[2] = (current_uid >> 8) & 0xFF;
            uid[3] = current_uid & 0xFF;

            StickCP2.Lcd.setCursor(10, 60);
            StickCP2.Lcd.setTextColor(GREEN);
            StickCP2.Lcd.printf("FUZZING: %02X%02X%02X%02X", uid[0], uid[1], uid[2], uid[3]);

            // Symulacja karty zbliżeniowej w locie
            fuzzer_nfc.AsTarget();
            delay(150); // Czas trwania jednego impulsu radiowego (150ms)
            current_uid++;
        } else {
            StickCP2.Lcd.setCursor(10, 60);
            StickCP2.Lcd.setTextColor(YELLOW);
            StickCP2.Lcd.print("Status: Oczekiwanie");
        }
    }
}

#include <Arduino.h>
#include "ControladorSemaforo.h"

ControladorSemaforo* controladorPtr = nullptr;

// Function to convert uint32_t to binary string
String uint32ToBinaryString(uint32_t value) {
    char binary[33];
    binary[32] = '\0';
    for (int i = 31; i >= 0; i--) {
        binary[i] = (value & 1) ? '1' : '0';
        value >>= 1;
    }
    return String(binary);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing Traffic Light Controller...");

    ControladorSemaforo::Config config = {
        .pinData = 12,
        .pinLatch = 33,
        .pinClock = 15,
        .pinOE = 27,
        .ledPin = LED_BUILTIN
    };
    
    controladorPtr = &ControladorSemaforo::getInstance(config);
    
    if (!controladorPtr->initRTC()) {
        Serial.println("Failed to initialize RTC!");
        return;
    }
    
    // Set the current time (you should do this only once, then comment it out)
    // Use the current date and time when uploading the code
    controladorPtr->setDateTime(DateTime(F(__DATE__), F(__TIME__)));
    
    // Add duration-based scenarios
    controladorPtr->agregarEscenario(614006784,  5000); // Ejemplo: primer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(1638400,    375); // Ejemplo: segundo LED encendido por 1 segundo
    controladorPtr->agregarEscenario(614006784,  375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(1638400,    375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(614006784,  375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(1638400,    375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(614006784,  375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(1638400,    375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(614006784,  375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(1226375168, 3000); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2451062784, 5000); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2449997824, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2451062784, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2449997824, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2451062784, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2449997824, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2451062784, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2449997824, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2451062784, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2452127744, 3000); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2428698624, 5000); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2428567552, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2428698624, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2428567552, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2428698624, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2428567552, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2428698624, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2428567552, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2428698624, 375); // Ejemplo: tercer LED encendido por 3 segundos
    controladorPtr->agregarEscenario(2428829696, 3000); // Ejemplo: tercer LED encendido por 3 segundos
    
    // Add time-based scenarios
    DateTime now = controladorPtr->getDateTime();
    
    // Schedule a specific pattern to start 2 minutes from now
    DateTime twoMinutesLater = now + TimeSpan(0, 0, 1, 0);
    controladorPtr->agregarEscenarioConHora(0b11111111111111111111111111111111, twoMinutesLater);  // All lights on
    
    // Schedule another pattern to start 3 minutes from now
    DateTime threeMinutesLater = now + TimeSpan(0, 0, 2, 0);
    controladorPtr->agregarEscenarioConHora(0b00011111111111111111111111111111, threeMinutesLater);  // Yellow and Green on

    controladorPtr->setOnEscenarioChangeCallback([](uint32_t estado) {
        Serial.printf("Scenario changed to: 0b%s\n", uint32ToBinaryString(estado).c_str());
    });

    controladorPtr->iniciar();
    Serial.println("Traffic Light Controller initialized and started!");
}

void loop() {
    if (controladorPtr) {
        controladorPtr->actualizar();
    }
    
    // Print current time every 10 seconds
    static uint32_t lastTimePrint = 0;
    if (millis() - lastTimePrint >= 10000) {
        DateTime now = controladorPtr->getDateTime();
        Serial.printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n", 
                      now.year(), now.month(), now.day(), 
                      now.hour(), now.minute(), now.second());
        lastTimePrint = millis();
    }
    
    // Other tasks can be added here
    delay(10);  // Small delay to prevent watchdog timer issues
}
#include "ControladorSemaforo.h"

ControladorSemaforo* controladorPtr = nullptr;

// Example usage
void setup()
{
    Serial.begin(115200);

    ControladorSemaforo::Config config = {
        .pinData = 12,
        .pinLatch = 33,
        .pinClock = 15,
        .pinOE = 27,
        .ledPin = LED_BUILTIN};

    auto &controlador = ControladorSemaforo::getInstance(config);

    controlador.agregarEscenario(614006784, 5000);  // Ejemplo: primer LED encendido por 3 segundos
    controlador.agregarEscenario(1638400, 375);     // Ejemplo: segundo LED encendido por 1 segundo
    controlador.agregarEscenario(614006784, 375);   // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(1638400, 375);     // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(614006784, 375);   // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(1638400, 375);     // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(614006784, 375);   // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(1638400, 375);     // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(614006784, 375);   // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(1226375168, 3000); // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2451062784, 5000); // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2449997824, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2451062784, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2449997824, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2451062784, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2449997824, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2451062784, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2449997824, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2451062784, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2452127744, 3000); // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2428698624, 5000); // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2428567552, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2428698624, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2428567552, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2428698624, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2428567552, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2428698624, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2428567552, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2428698624, 375);  // Ejemplo: tercer LED encendido por 3 segundos
    controlador.agregarEscenario(2428829696, 3000); // Ejemplo: tercer LED encendido por 3 segundos
                                                    // ... add more scenarios ...

    controlador.setOnEscenarioChangeCallback([](uint32_t estado)
                                             {
        // Custom logic when scenario changes
        Serial.printf("Escenario changed to: %u\n", estado); });

    controlador.iniciar();
}

void loop()
{
    ControladorSemaforo::getInstance({}).actualizar();
    // Other tasks...
}
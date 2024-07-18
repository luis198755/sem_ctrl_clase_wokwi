#include <Arduino.h>
#include "esp_timer.h"
#include <vector>
#include <functional>

class ControladorSemaforo
{
public:
    // Configuration structure
    struct Config
    {
        int pinData;
        int pinLatch;
        int pinClock;
        int pinOE;
        int ledPin;
    };

    // Callback function type
    using CallbackFunction = std::function<void(uint32_t)>;

private:
    struct Escenario
    {
        uint32_t estado;
        uint32_t duracion;
    };

    std::vector<Escenario> escenarios;
    size_t escenarioActual;
    bool cambioEscenario;
    esp_timer_handle_t timer;
    Config config;
    CallbackFunction onEscenarioChange;

    ControladorSemaforo(const Config &cfg)
        : escenarioActual(0), cambioEscenario(false), timer(nullptr), config(cfg)
    {
        initReg();
    }

    ControladorSemaforo(const ControladorSemaforo &) = delete;
    ControladorSemaforo &operator=(const ControladorSemaforo &) = delete;

public:
    static ControladorSemaforo &getInstance(const Config &cfg)
    {
        static ControladorSemaforo instance(cfg);
        return instance;
    }

    ~ControladorSemaforo()
    {
        if (timer)
        {
            esp_timer_delete(timer);
        }
    }

    void agregarEscenario(uint32_t estado, uint32_t duracion)
    {
        escenarios.push_back({estado, duracion});
    }

    void iniciar()
    {
        escenarioActual = 0;
        cambioEscenario = true;
        configurarTimer();
    }

    void actualizar()
    {
        if (cambioEscenario)
        {
            cambioEscenario = false;
            Serial.printf("Escenario: %zu - Valor: %u\n", escenarioActual, escenarios[escenarioActual].estado);
            interfaceProg(escenarios[escenarioActual].estado);

            if (onEscenarioChange)
            {
                onEscenarioChange(escenarios[escenarioActual].estado);
            }
        }
    }

    uint32_t obtenerEscenarioActual() const
    {
        return escenarios[escenarioActual].estado;
    }

    void setOnEscenarioChangeCallback(CallbackFunction callback)
    {
        onEscenarioChange = std::move(callback);
    }

private:
    void configurarTimer()
    {
        esp_timer_create_args_t timerConfig = {
            .callback = timerCallback,
            .arg = this,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "semaforo_timer"};

        ESP_ERROR_CHECK(esp_timer_create(&timerConfig, &timer));
        iniciarSiguienteTimer();
    }

    void iniciarSiguienteTimer()
    {
        if (!escenarios.empty())
        {
            esp_timer_stop(timer);
            ESP_ERROR_CHECK(esp_timer_start_once(timer, escenarios[escenarioActual].duracion * 1000));
        }
    }

    static void timerCallback(void *arg)
    {
        auto *controlador = static_cast<ControladorSemaforo *>(arg);
        controlador->siguienteEscenario();
    }

    void siguienteEscenario()
    {
        escenarioActual = (escenarioActual + 1) % escenarios.size();
        cambioEscenario = true;
        iniciarSiguienteTimer();
    }

    void initReg()
    {
        pinMode(config.pinData, OUTPUT);
        pinMode(config.pinLatch, OUTPUT);
        pinMode(config.pinClock, OUTPUT);
        pinMode(config.pinOE, OUTPUT);
        pinMode(config.ledPin, OUTPUT);

        digitalWrite(config.pinOE, LOW);
        ledWrite(0xff, 0xff, 0xff, 0xff);
    }

    void interfaceProg(uint32_t var32Bits)
    {
        ledWrite(
            (var32Bits & 0xFF) ^ 0xFF,
            ((var32Bits >> 8) & 0xFF) ^ 0xFF,
            ((var32Bits >> 16) & 0xFF) ^ 0xFF,
            ((var32Bits >> 24) & 0xFF) ^ 0xFF);
    }

    void ledWrite(uint8_t Reg4, uint8_t Reg3, uint8_t Reg2, uint8_t Reg1)
    {
        shiftOut(config.pinData, config.pinClock, LSBFIRST, Reg4);
        shiftOut(config.pinData, config.pinClock, LSBFIRST, Reg3);
        shiftOut(config.pinData, config.pinClock, LSBFIRST, Reg2);
        shiftOut(config.pinData, config.pinClock, LSBFIRST, Reg1);
        digitalWrite(config.pinLatch, HIGH);
        digitalWrite(config.pinLatch, LOW);
    }
};

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
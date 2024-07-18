#ifndef CONTROLADOR_SEMAFORO_H
#define CONTROLADOR_SEMAFORO_H

#include <Arduino.h>
#include <vector>
#include <functional>

class ControladorSemaforo {
public:
    struct Config {
        int pinData;
        int pinLatch;
        int pinClock;
        int pinOE;
        int ledPin;
    };

    using CallbackFunction = std::function<void(uint32_t)>;

    static ControladorSemaforo& getInstance(const Config& cfg);
    ~ControladorSemaforo();

    void agregarEscenario(uint32_t estado, uint32_t duracion);
    void iniciar();
    void actualizar();
    uint32_t obtenerEscenarioActual() const;
    void setOnEscenarioChangeCallback(CallbackFunction callback);

private:
    struct Escenario {
        uint32_t estado;
        uint32_t duracion;
    };

    std::vector<Escenario> escenarios;
    size_t escenarioActual;
    bool cambioEscenario;
    void* timer; // Using void* to avoid including esp_timer.h in the header
    Config config;
    CallbackFunction onEscenarioChange;

    ControladorSemaforo(const Config& cfg);
    ControladorSemaforo(const ControladorSemaforo&) = delete;
    ControladorSemaforo& operator=(const ControladorSemaforo&) = delete;

    void configurarTimer();
    void iniciarSiguienteTimer();
    static void timerCallback(void* arg);
    void siguienteEscenario();
    void initReg();
    void interfaceProg(uint32_t var32Bits);
    void ledWrite(uint8_t Reg4, uint8_t Reg3, uint8_t Reg2, uint8_t Reg1);
};

#endif // CONTROLADOR_SEMAFORO_H
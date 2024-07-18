#include "ControladorSemaforo.h"
#include "esp_timer.h"

ControladorSemaforo& ControladorSemaforo::getInstance(const Config& cfg) {
    static ControladorSemaforo instance(cfg);
    return instance;
}

ControladorSemaforo::ControladorSemaforo(const Config& cfg)
    : escenarioActual(0), cambioEscenario(false), timer(nullptr), config(cfg) {
    initReg();
}

ControladorSemaforo::~ControladorSemaforo() {
    if (timer) {
        esp_timer_delete(static_cast<esp_timer_handle_t>(timer));
    }
}

void ControladorSemaforo::agregarEscenario(uint32_t estado, uint32_t duracion) {
    escenarios.push_back({estado, duracion});
}

void ControladorSemaforo::iniciar() {
    escenarioActual = 0;
    cambioEscenario = true;
    configurarTimer();
}

void ControladorSemaforo::actualizar() {
    if (cambioEscenario) {
        cambioEscenario = false;
        Serial.printf("Escenario: %zu - Valor: %u\n", escenarioActual, escenarios[escenarioActual].estado);
        interfaceProg(escenarios[escenarioActual].estado);
        
        if (onEscenarioChange) {
            onEscenarioChange(escenarios[escenarioActual].estado);
        }
    }
}

uint32_t ControladorSemaforo::obtenerEscenarioActual() const {
    return escenarios[escenarioActual].estado;
}

void ControladorSemaforo::setOnEscenarioChangeCallback(CallbackFunction callback) {
    onEscenarioChange = std::move(callback);
}

void ControladorSemaforo::configurarTimer() {
    esp_timer_create_args_t timerConfig = {
        .callback = timerCallback,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "semaforo_timer"
    };
    
    ESP_ERROR_CHECK(esp_timer_create(&timerConfig, reinterpret_cast<esp_timer_handle_t*>(&timer)));
    iniciarSiguienteTimer();
}

void ControladorSemaforo::iniciarSiguienteTimer() {
    if (!escenarios.empty()) {
        esp_timer_stop(static_cast<esp_timer_handle_t>(timer));
        ESP_ERROR_CHECK(esp_timer_start_once(static_cast<esp_timer_handle_t>(timer), escenarios[escenarioActual].duracion * 1000));
    }
}

void ControladorSemaforo::timerCallback(void* arg) {
    auto* controlador = static_cast<ControladorSemaforo*>(arg);
    controlador->siguienteEscenario();
}

void ControladorSemaforo::siguienteEscenario() {
    escenarioActual = (escenarioActual + 1) % escenarios.size();
    cambioEscenario = true;
    iniciarSiguienteTimer();
}

void ControladorSemaforo::initReg() {
    pinMode(config.pinData, OUTPUT);
    pinMode(config.pinLatch, OUTPUT);
    pinMode(config.pinClock, OUTPUT);
    pinMode(config.pinOE, OUTPUT);
    pinMode(config.ledPin, OUTPUT);

    digitalWrite(config.pinOE, LOW);
    ledWrite(0xff, 0xff, 0xff, 0xff);
}

void ControladorSemaforo::interfaceProg(uint32_t var32Bits) {
    ledWrite(
        (var32Bits & 0xFF) ^ 0xFF,
        ((var32Bits >> 8) & 0xFF) ^ 0xFF,
        ((var32Bits >> 16) & 0xFF) ^ 0xFF,
        ((var32Bits >> 24) & 0xFF) ^ 0xFF
    );
}

void ControladorSemaforo::ledWrite(uint8_t Reg4, uint8_t Reg3, uint8_t Reg2, uint8_t Reg1) {
    shiftOut(config.pinData, config.pinClock, LSBFIRST, Reg4);
    shiftOut(config.pinData, config.pinClock, LSBFIRST, Reg3);
    shiftOut(config.pinData, config.pinClock, LSBFIRST, Reg2);
    shiftOut(config.pinData, config.pinClock, LSBFIRST, Reg1);
    digitalWrite(config.pinLatch, HIGH);
    digitalWrite(config.pinLatch, LOW);
}
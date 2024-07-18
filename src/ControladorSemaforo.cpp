#include "ControladorSemaforo.h"
#include "esp_timer.h"
#include <Wire.h>

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

bool ControladorSemaforo::initRTC() {
    Wire.begin();  // Initialize I2C communication
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        return false;
    }

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, lets set the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    return true;
}

void ControladorSemaforo::setDateTime(const DateTime& dt) {
    rtc.adjust(dt);
}

DateTime ControladorSemaforo::getDateTime() const {
    // Create a non-const copy of rtc to call the non-const now() method
    RTC_DS3231 rtc_copy = rtc;
    return rtc_copy.now();
}

void ControladorSemaforo::agregarEscenario(uint32_t estado, uint32_t duracion) {
    escenarios.push_back({estado, duracion, DateTime(), false});
}

void ControladorSemaforo::agregarEscenarioConHora(uint32_t estado, const DateTime& tiempo) {
    escenarios.push_back({estado, 0, tiempo, true});
}

void ControladorSemaforo::iniciar() {
    escenarioActual = 0;
    cambioEscenario = true;
    configurarTimer();
}

void ControladorSemaforo::actualizar() {
    actualizarEscenariosPorTiempo();
    
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
    if (!escenarios.empty() && !escenarios[escenarioActual].useTiempo) {
        esp_timer_stop(static_cast<esp_timer_handle_t>(timer));
        ESP_ERROR_CHECK(esp_timer_start_once(static_cast<esp_timer_handle_t>(timer), escenarios[escenarioActual].duracion * 1000));
    }
}

void ControladorSemaforo::timerCallback(void* arg) {
    auto* controlador = static_cast<ControladorSemaforo*>(arg);
    controlador->siguienteEscenario();
}

void ControladorSemaforo::siguienteEscenario() {
    do {
        escenarioActual = (escenarioActual + 1) % escenarios.size();
    } while (escenarios[escenarioActual].useTiempo);
    
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

void ControladorSemaforo::actualizarEscenariosPorTiempo() {
    // Create a non-const copy of rtc to call the non-const now() method
    RTC_DS3231 rtc_copy = rtc;
    DateTime now = rtc_copy.now();
    for (size_t i = 0; i < escenarios.size(); ++i) {
        if (escenarios[i].useTiempo && escenarios[i].tiempo == now) {
            escenarioActual = i;
            cambioEscenario = true;
            break;
        }
    }
}
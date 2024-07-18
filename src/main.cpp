#include <Arduino.h>
#include "esp_timer.h"

class ControladorSemaforo {
private:
    uint32_t escenarios[100];
    uint32_t tiempos[100];
    int numEscenarios;
    int escenarioActual;
    bool cambioEscenario;
    esp_timer_handle_t timer;

    // Constructor privado
    ControladorSemaforo() : numEscenarios(0), escenarioActual(0), cambioEscenario(false), timer(nullptr) {}

    // Evitar copia y asignación
    ControladorSemaforo(const ControladorSemaforo&) = delete;
    ControladorSemaforo& operator=(const ControladorSemaforo&) = delete;

public:
    // Método estático para obtener la instancia única
    static ControladorSemaforo& getInstance() {
        static ControladorSemaforo instance;
        return instance;
    }

    ~ControladorSemaforo() {
        if (timer) {
            esp_timer_delete(timer);
        }
    }

    void agregarEscenario(uint32_t escenario, uint32_t tiempo) {
        if (numEscenarios < 100) {
            escenarios[numEscenarios] = escenario;
            tiempos[numEscenarios] = tiempo;
            numEscenarios++;
        }
    }

    void iniciar() {
        escenarioActual = 0;
        cambioEscenario = true;
        configurarTimer();
    }

    void actualizar() {
        if (cambioEscenario) {
            cambioEscenario = false;
            Serial.print("Escenario: ");
            Serial.print(escenarioActual);
            Serial.print(" - Valor: ");
            Serial.println(escenarios[escenarioActual], BIN);
        }
    }

    uint32_t obtenerEscenarioActual() const {
        return escenarios[escenarioActual];
    }

private:
    void configurarTimer() {
        esp_timer_create_args_t timerConfig = {
            .callback = timerCallback,
            .arg = this,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "semaforo_timer"
        };
        
        ESP_ERROR_CHECK(esp_timer_create(&timerConfig, &timer));
        iniciarSiguienteTimer();
    }

    void iniciarSiguienteTimer() {
        if (numEscenarios > 0) {
            esp_timer_stop(timer);
            ESP_ERROR_CHECK(esp_timer_start_once(timer, tiempos[escenarioActual] * 1000000)); // Convertir a microsegundos
        }
    }

    static void timerCallback(void* arg) {
        ControladorSemaforo* controlador = static_cast<ControladorSemaforo*>(arg);
        controlador->siguienteEscenario();
    }

    void siguienteEscenario() {
        escenarioActual = (escenarioActual + 1) % numEscenarios;
        cambioEscenario = true;
        iniciarSiguienteTimer();
    }
};

// Definición de pines
const int PIN_ROJO = 2;    // Ejemplo: conectar LED rojo al pin 2
const int PIN_AMARILLO = 4; // Ejemplo: conectar LED amarillo al pin 4
const int PIN_VERDE = 5;   // Ejemplo: conectar LED verde al pin 5

void setup() {
    Serial.begin(115200);
    
    pinMode(PIN_ROJO, OUTPUT);
    pinMode(PIN_AMARILLO, OUTPUT);
    pinMode(PIN_VERDE, OUTPUT);
    
    auto& controlador = ControladorSemaforo::getInstance();
    
    controlador.agregarEscenario(0b001, 3); // Rojo por 3 segundos
    controlador.agregarEscenario(0b010, 1); // Amarillo por 1 segundo
    controlador.agregarEscenario(0b100, 3); // Verde por 3 segundos

    controlador.iniciar();
}

void actualizarLuces(uint32_t escenario) {
    digitalWrite(PIN_ROJO, escenario & 0b001);
    digitalWrite(PIN_AMARILLO, escenario & 0b010);
    digitalWrite(PIN_VERDE, escenario & 0b100);
}

void loop() {
    auto& controlador = ControladorSemaforo::getInstance();
    controlador.actualizar();
    actualizarLuces(controlador.obtenerEscenarioActual());
    
    // Aquí puedes agregar otras tareas que necesites ejecutar
}


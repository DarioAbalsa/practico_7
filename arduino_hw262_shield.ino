// ============================================================
// Control de E/S con Shield HW-262 — ENTRADAS TOGGLE
//
// Mapeo REAL de pines del HW-262:
//   Pulsadores: A1, A2, A3
//   LEDs:       D1 = 13,  D2 = 12,  D3 = 11   (D4 = 10 no se usa)
//   Display 7 segmentos: Latch = 4, Clock = 7, Data = 8
//
// Comportamiento:
//   - Entradas con toggle: cada pulsación invierte el estado
//     lógico y se mantiene aunque se suelte.
//   - Salidas con toggle por comando serie desde Processing.
//
// Protocolo:
//   PC -> Arduino: '1' '2' '3'  (toggle salida correspondiente)
//   Arduino -> PC: 'A'/'a' 'B'/'b' 'C'/'c'  (entrada lógica HIGH/LOW)
//
// NOTA: LEDs del HW-262 conectados a GND -> LOW = ON, HIGH = OFF.
// ============================================================

// Pines de entrada (pulsadores del shield)
const int pinEntrada[3] = {A1, A2, A3};

// Pines de salida (LEDs del shield: D1=13, D2=12, D3=11)
const int pinSalida[3] = {13, 12, 11};

// Pines del display de 7 segmentos (74HC595)
const int SR_LATCH = 4;
const int SR_CLOCK = 7;
const int SR_DATA  = 8;

// Estado físico previo de cada entrada (para detectar el flanco)
int lecturaAnterior[3] = {HIGH, HIGH, HIGH};

// Estado LÓGICO de cada entrada (el que se mantiene y se envía)
bool estadoLogico[3] = {false, false, false};

void setup() {
  Serial.begin(9600);

  // ------------------------------------------------------------
  // 1. Inicializar y APAGAR el display de 7 segmentos
  //    (evita el estado "semi encendido" por pines flotantes)
  // ------------------------------------------------------------
  pinMode(SR_LATCH, OUTPUT);
  pinMode(SR_CLOCK, OUTPUT);
  pinMode(SR_DATA,  OUTPUT);

  digitalWrite(SR_LATCH, LOW);
  digitalWrite(SR_CLOCK, LOW);
  digitalWrite(SR_DATA,  LOW);

  // Borrar contenido del 74HC595: dos bytes 0x00 + latch
  digitalWrite(SR_LATCH, LOW);
  shiftOut(SR_DATA, SR_CLOCK, MSBFIRST, 0x00);  // segmentos apagados
  shiftOut(SR_DATA, SR_CLOCK, MSBFIRST, 0x00);  // dígitos deseleccionados
  digitalWrite(SR_LATCH, HIGH);
  delay(1);
  digitalWrite(SR_LATCH, LOW);

  // ------------------------------------------------------------
  // 2. Inicializar entradas y salidas
  // ------------------------------------------------------------
  for (int i = 0; i < 3; i++) {
    // Entradas: shield con pull-up integrado. Si es inestable,
    // cambiar a INPUT_PULLUP.
    pinMode(pinEntrada[i], INPUT);

    // Salidas: LEDs a GND -> HIGH = apagado
    pinMode(pinSalida[i], OUTPUT);
    digitalWrite(pinSalida[i], HIGH);

    lecturaAnterior[i] = digitalRead(pinEntrada[i]);
  }
}

void loop() {
  // ---------- 1. Recibir comandos desde Processing ----------
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    if (cmd >= '1' && cmd <= '3') {
      int idx = cmd - '1';
      int estadoActual = digitalRead(pinSalida[idx]);
      digitalWrite(pinSalida[idx], !estadoActual);   // toggle LED
    }
  }

  // ---------- 2. Leer pulsadores y hacer TOGGLE en cada pulsación ----------
  for (int i = 0; i < 3; i++) {
    int lectura = digitalRead(pinEntrada[i]);

    // Flanco: antes suelto (HIGH) y ahora presionado (LOW)
    if (lecturaAnterior[i] == HIGH && lectura == LOW) {
      estadoLogico[i] = !estadoLogico[i];

      char letra = 'A' + i;
      if (estadoLogico[i]) {
        Serial.println(letra);              // 'A' 'B' 'C'  = ALTO
      } else {
        Serial.println((char)(letra + 32)); // 'a' 'b' 'c'  = BAJO
      }
    }

    lecturaAnterior[i] = lectura;
  }

  delay(20);  // Anti-rebote
}

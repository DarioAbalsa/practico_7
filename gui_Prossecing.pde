// ============================================================
// Panel de control Arduino + Shield HW-262  (entradas toggle)
// ============================================================

import processing.serial.*;

Serial puerto;
String puertoNombre;

BotonEntrada[] entradas = new BotonEntrada[3];
BotonSalida[]  salidas  = new BotonSalida[3];

void setup() {
  size(600, 420);
  smooth();

  println("Puertos disponibles: " + Serial.list());
  puertoNombre = Serial.list()[0];     // ajustar si hace falta
  puerto = new Serial(this, puertoNombre, 9600);
  puerto.bufferUntil('\n');
  println("Conectado a: " + puertoNombre);

  for (int i = 0; i < 3; i++) {
    entradas[i] = new BotonEntrada(60, 110 + i * 90, 180, 60,
                                   "Entrada " + (i + 1));
  }
  for (int i = 0; i < 3; i++) {
    salidas[i] = new BotonSalida(360, 110 + i * 90, 180, 60,
                                 "Salida " + (i + 1), i);
  }
}

void draw() {
  background(245);

  fill(0);
  textAlign(CENTER, TOP);
  textSize(22);
  text("Panel de Control — Arduino + HW-262", width/2, 18);

  textSize(16);
  fill(60);
  text("ENTRADAS (toggle)", 150, 78);
  text("SALIDAS (clic)",    450, 78);

  for (BotonEntrada e : entradas) e.dibujar();
  for (BotonSalida  s : salidas)  s.dibujar();

  fill(120);
  textSize(11);
  textAlign(LEFT, BOTTOM);
  text("Puerto: " + puertoNombre, 12, height - 10);
}

void serialEvent(Serial p) {
  String linea = p.readStringUntil('\n');
  if (linea == null) return;
  linea = trim(linea);
  if (linea.length() == 0) return;

  char c = linea.charAt(0);

  if (c >= 'A' && c <= 'C') {
    entradas[c - 'A'].estadoAlto = true;
  } else if (c >= 'a' && c <= 'c') {
    entradas[c - 'a'].estadoAlto = false;
  }
}

void mousePressed() {
  if (mouseButton != LEFT) return;
  for (BotonSalida s : salidas) {
    if (s.contiene(mouseX, mouseY)) {
      s.encendido = !s.encendido;
      puerto.write((char)('1' + s.indice));
      println("Enviado: " + (char)('1' + s.indice));
    }
  }
}

// ============================================================
// Clases auxiliares
// ============================================================

class BotonEntrada {
  float x, y, w, h;
  String etiqueta;
  boolean estadoAlto = false;

  BotonEntrada(float x, float y, float w, float h, String etiqueta) {
    this.x = x; this.y = y; this.w = w; this.h = h;
    this.etiqueta = etiqueta;
  }

  void dibujar() {
    fill(estadoAlto ? color(80, 200, 100) : color(180, 180, 180));
    stroke(0);
    strokeWeight(2);
    rect(x, y, w, h, 12);

    fill(255);
    noStroke();
    textAlign(CENTER, CENTER);
    textSize(15);
    text(etiqueta + "\n" + (estadoAlto ? "ALTO" : "BAJO"),
         x + w/2, y + h/2);
  }
}

class BotonSalida {
  float x, y, w, h;
  String etiqueta;
  int indice;
  boolean encendido = false;

  BotonSalida(float x, float y, float w, float h, String etiqueta, int indice) {
    this.x = x; this.y = y; this.w = w; this.h = h;
    this.etiqueta = etiqueta; this.indice = indice;
  }

  void dibujar() {
    fill(encendido ? color(255, 200, 60) : color(120, 120, 130));
    stroke(0);
    strokeWeight(2);
    rect(x, y, w, h, 12);

    fill(encendido ? 0 : 255);
    noStroke();
    textAlign(CENTER, CENTER);
    textSize(15);
    text(etiqueta + "\n" + (encendido ? "ON" : "OFF"),
         x + w/2, y + h/2);
  }

  boolean contiene(float mx, float my) {
    return mx >= x && mx <= x + w &&
           my >= y && my <= y + h;
  }
}

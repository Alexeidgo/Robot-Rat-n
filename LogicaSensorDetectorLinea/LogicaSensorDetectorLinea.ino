// Sensores
#define S1 18
#define S2 19
#define S3 32
#define S4 23
#define S5 22


// Estados
int Derecha_E = 0;
int Izquierda_E = 1;
int Avanzar_E = 2;
int Detener_E = 3;
int InterseccionT_E = 4;
int InterseccionY_E = 5;

int estadoActual = Avanzar_E;


void setup() {

  Serial.begin(115200);

  // Sensores
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);

}


void loop() {

  // Lectura de sensores

  bool s1 = !digitalRead(S1);
  bool s2 = !digitalRead(S2);
  bool s3 = !digitalRead(S3);
  bool s4 = !digitalRead(S4);
  bool s5 = !digitalRead(S5);


  // Entradas

  bool XI = s1 || s2;
  bool XC = s3;
  bool XD = s4 || s5;

  // Intersección Y
  bool XY = XI && XD;

  // Ningún sensor detecta línea
  bool X0 = !s1 && !s2 && !s3 && !s4 && !s5;


  // Lectura de sensores

  Serial.print("S1: ");
  Serial.print(s1);

  Serial.print(" S2: ");
  Serial.print(s2);

  Serial.print(" S3: ");
  Serial.print(s3);

  Serial.print(" S4: ");
  Serial.print(s4);

  Serial.print(" S5: ");
  Serial.println(s5);


  // Máquina de estados

  if (XY) {

    estadoActual = InterseccionY_E;
    interY();

  }

  else if (XD) {

    estadoActual = Derecha_E;
    girarDerecha();

  }

  else if (XI) {

    estadoActual = Izquierda_E;
    girarIzquierda();

  }

  else if (XC) {

    estadoActual = Avanzar_E;
    avanzar();

  }

  else if (X0) {

    estadoActual = Detener_E;
    detener();

  }

}


// FUNCIONES

void girarIzquierda() {

  Serial.println("Girando a la Izquierda");

}


void girarDerecha() {

  Serial.println("Girando a la Derecha");

}


void avanzar() {

  Serial.println("Avanzando");

}


void interT() {

  Serial.println("Interseccion T");

}


void interY() {

  Serial.println("Interseccion Y");

}
¿

void detener() {

  Serial.println("Robot Detenido");

}
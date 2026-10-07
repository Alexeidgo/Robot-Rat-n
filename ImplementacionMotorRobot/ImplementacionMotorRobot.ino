// Motores
#define EN1 25
#define IN1 26
#define IN2 27

#define EN2 33
#define IN3 14
#define IN4 12


// PWM
#define frec_pwm 5000
#define res_pwm 8

#define velocidad 150


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


  // Motores

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  // PWM

  ledcAttach(EN1, frec_pwm, res_pwm);
  ledcAttach(EN2, frec_pwm, res_pwm);


  // Sensores

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);


  // Estado inicial

  detener();

}


void loop() {

  // Lectura de los sensores

  bool s1 = !digitalRead(S1);
  bool s2 = !digitalRead(S2);
  bool s3 = !digitalRead(S3);
  bool s4 = !digitalRead(S4);
  bool s5 = !digitalRead(S5);


  // Entradas

  bool XI = s1 || s2;
  bool XC = s3;
  bool XD = s4 || s5;


  // Intersección en T

  bool XT = s1 && s2 && s3 && s4 && s5;


  // Intersección en Y

  bool XY = XI && XD;


  // Ningún sensor detecta la línea

  bool X0 = !s1 && !s2 && !s3 && !s4 && !s5;


  // Línea centrada nuevamente

  bool lineaCentrada = !s1 && !s2 && s3 && !s4 && !s5;


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


  // Si estamos ejecutando una intersección en T

  if (estadoActual == InterseccionT_E) {

    if (lineaCentrada) {

      estadoActual = Avanzar_E;
      avanzar();

    }

    else {

      interT();

    }

  }


  // Si estamos ejecutando una intersección en Y

  else if (estadoActual == InterseccionY_E) {

    if (lineaCentrada) {

      estadoActual = Avanzar_E;
      avanzar();

    }

    else {

      interY();

    }

  }


  // Detectar una nueva intersección en T

  else if (XT) {

    estadoActual = InterseccionT_E;
    interT();

  }


  // Detectar una intersección en Y

  else if (XY) {

    estadoActual = InterseccionY_E;
    interY();

  }


  // Línea a la derecha

  else if (XD) {

    estadoActual = Derecha_E;
    girarDerecha();

  }


  // Línea a la izquierda

  else if (XI) {

    estadoActual = Izquierda_E;
    girarIzquierda();

  }


  // Línea central

  else if (XC) {

    estadoActual = Avanzar_E;
    avanzar();

  }


  // Línea perdida

  else if (X0) {

    estadoActual = Detener_E;
    detener();

  }

}


// Funciones


void girarIzquierda() {

  Serial.println("Girando a la Izquierda");


  // Motor izquierdo hacia atras

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);


  // Motor derecho hacia adelante

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);


  // Velocidad

  ledcWrite(EN1, velocidad);
  ledcWrite(EN2, velocidad);

}


void girarDerecha() {

  Serial.println("Girando a la Derecha");


  // Motor izquierdo hacia adelante

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);


  // Motor derecho hacia atras

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);


  // Velocidad

  ledcWrite(EN1, velocidad);
  ledcWrite(EN2, velocidad);

}


void avanzar() {

  Serial.println("Avanzando");


  // Motor izquierdo hacia adelante

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);


  // Motor derecho hacia adelante

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);


  // Velocidad

  ledcWrite(EN1, velocidad);
  ledcWrite(EN2, velocidad);

}


void interT() {

  Serial.println("Interseccion T");

  // Por regla de mano derecha:

  girarDerecha();

}


void interY() {

  Serial.println("Interseccion Y");

  // Por regla de mano derecha

  girarDerecha();

}


void detener() {

  Serial.println("Robot Detenido");


  // Detener motor izquierdo

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);


  // Detener motor derecho

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);


  // PWM en cero

  ledcWrite(EN1, 0);
  ledcWrite(EN2, 0);

}
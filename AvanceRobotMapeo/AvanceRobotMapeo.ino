#define BOTON_INICIO 14

#define EN1 25
#define IN1 26
#define IN2 27
#define EN2 33
#define IN3 14
#define IN4 12

#define frec_pwm 5000
#define res_pwm 8
#define velocidad 150

#define S1 18
#define S2 19
#define S3 32
#define S4 23
#define S5 22

int Derecha_E = 0;
int Izquierda_E = 1;
int Avanzar_E = 2;
int Detener_E = 3;
int InterseccionT_E = 4;
int InterseccionY_E = 5;
int Retorno_E = 6;

int estadoActual = Detener_E;

struct TramoMapa {
  int nodoOrigen;
  int nodoDestino;
  char accionTomada;
  unsigned long tiempoRecorrido;
};

TramoMapa mapaLaberinto[100]; 
int totalTramos = 0;          
int contadorNodos = 0;        

int pilaNodos[50];            
int topePila = 0;             
unsigned long inicioCronometro = 0;
char ultimaAccion = 'A';      

enum ModoRobot { ESPERA, EXPLORACION, FIN_MAPEO };
ModoRobot modoActual = ESPERA;

unsigned long tiempoMeta = 0;
bool detectandoMeta = false;

void setup() {
  Serial.begin(115200);
  pinMode(BOTON_INICIO, INPUT_PULLUP);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  ledcAttach(EN1, frec_pwm, res_pwm);
  ledcAttach(EN2, frec_pwm, res_pwm);

  pinMode(S1, INPUT); pinMode(S2, INPUT); pinMode(S3, INPUT);
  pinMode(S4, INPUT); pinMode(S5, INPUT);

  pilaNodos[0] = 0;
  detener();
  Serial.println("Presionar el botón");
}

void loop() {
  if (digitalRead(BOTON_INICIO) == LOW) {
    delay(200); 
    if (modoActual == ESPERA) {
      Serial.println("Modo exploración iniciado...");
      modoActual = EXPLORACION;
      estadoActual = Avanzar_E;
      inicioCronometro = millis();
    }
    else if (modoActual == FIN_MAPEO) {
      
      totalTramos = 0;   
      contadorNodos = 0; 
      topePila = 0;      
      
      modoActual = EXPLORACION;
      estadoActual = Avanzar_E;
      inicioCronometro = millis();
    }
    while(digitalRead(BOTON_INICIO) == LOW); 
  }

  if (modoActual == ESPERA || modoActual == FIN_MAPEO) {
    return;
  }

  bool s1 = !digitalRead(S1); bool s2 = !digitalRead(S2);
  bool s3 = !digitalRead(S3); bool s4 = !digitalRead(S4); bool s5 = !digitalRead(S5);

  bool XI = s1 || s2;
  bool XC = s3;
  bool XD = s4 || s5;
  bool XT = s1 && s2 && s3 && s4 && s5;
  bool XY = XI && XD;
  bool X0 = !s1 && !s2 && !s3 && !s4 && !s5;
  bool lineaCentrada = !s1 && !s2 && s3 && !s4 && !s5;

  // Detección de Meta
  if (XT) {
    if (!detectandoMeta) {
      tiempoMeta = millis();
      detectandoMeta = true;
    } else if (millis() - tiempoMeta >= 2000) {
      registrarTramoEnMapa();
      detener();
      modoActual = FIN_MAPEO;
      imprimirMapaCompleto();
      return; 
    }
  } else {
    detectandoMeta = false; 
  }

  if (estadoActual == InterseccionT_E || estadoActual == InterseccionY_E || estadoActual == Retorno_E) {
    if (lineaCentrada) {
      estadoActual = Avanzar_E;
      inicioCronometro = millis();
      avanzar();
    }
  }
  else if (XT && !detectandoMeta) { 
    registrarTramoEnMapa();
    estadoActual = InterseccionT_E;
    ultimaAccion = 'D'; 
    girarDerecha();
  }
  else if (XY && !detectandoMeta) {
    registrarTramoEnMapa();
    estadoActual = InterseccionY_E;
    ultimaAccion = 'D'; 
    girarDerecha();
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
    registrarTramoEnMapa();
    estadoActual = Retorno_E;
    ultimaAccion = 'R'; 
    girarDerecha();
  }
}

void registrarTramoEnMapa() {
  unsigned long tiempoDelTramo = millis() - inicioCronometro;
  int nodoDeDondeVengo = pilaNodos[topePila];
  
  contadorNodos++; 
  int nodoNuevo = contadorNodos; 
  
  if (totalTramos < 100) {
    mapaLaberinto[totalTramos].nodoOrigen = nodoDeDondeVengo;
    mapaLaberinto[totalTramos].nodoDestino = nodoNuevo;
    mapaLaberinto[totalTramos].accionTomada = ultimaAccion;
    mapaLaberinto[totalTramos].tiempoRecorrido = tiempoDelTramo;
    
    totalTramos++;
  }

  if (ultimaAccion == 'R') {
    if (topePila > 0) topePila--; 
  } else {
    topePila++;
    pilaNodos[topePila] = nodoNuevo;
  }
}

void imprimirMapaCompleto() {
  Serial.println("Origen\tDestino\tAccion\tTiempo (ms)");
  Serial.println("--------------------------------------------");
  for (int i = 0; i < totalTramos; i++) {
    Serial.print(mapaLaberinto[i].nodoOrigen);    
    Serial.print("\t");
    Serial.print(mapaLaberinto[i].nodoDestino);   
    Serial.print("\t");
    Serial.print(mapaLaberinto[i].accionTomada);  
    Serial.print("\t");
    Serial.println(mapaLaberinto[i].tiempoRecorrido);
  }
  Serial.println("============================================");
}

void girarIzquierda() {
  digitalWrite(IN1, LOW); 
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); 
  digitalWrite(IN4, LOW);
  ledcWrite(EN1, velocidad); 
  ledcWrite(EN2, velocidad);
}

void girarDerecha() {
  digitalWrite(IN1, HIGH); 
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); 
  digitalWrite(IN4, HIGH);
  ledcWrite(EN1, velocidad); 
  ledcWrite(EN2, velocidad);
}

void avanzar() {
  digitalWrite(IN1, HIGH); 
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); 
  digitalWrite(IN4, LOW);
  ledcWrite(EN1, velocidad); 
  ledcWrite(EN2, velocidad);
}

void detener() {
  digitalWrite(IN1, LOW); 
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); 
  digitalWrite(IN4, LOW);
  ledcWrite(EN1, 0); 
  ledcWrite(EN2, 0);
}
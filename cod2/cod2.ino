#include <AFMotor.h>
#include <Ultrasonic.h>

// =====================================
// MOTORES
// =====================================
AF_DCMotor motor1(1); // Frente esquerda
AF_DCMotor motor2(2); // Frente direita
AF_DCMotor motor3(3); // Traseira esquerda
AF_DCMotor motor4(4); // Traseira direita

// =====================================
// ULTRASSONICOS
// =====================================
#define ECHO_FD 32
#define TRIG_FD 33

#define ECHO_FE 30
#define TRIG_FE 31

#define ECHO_TD 34
#define TRIG_TD 35

#define ECHO_TE 36
#define TRIG_TE 37

Ultrasonic ultrassonicoFD(TRIG_FD, ECHO_FD);
Ultrasonic ultrassonicoFE(TRIG_FE, ECHO_FE);
Ultrasonic ultrassonicoTD(TRIG_TD, ECHO_TD);
Ultrasonic ultrassonicoTE(TRIG_TE, ECHO_TE);

// =====================================
// SENSORES IR
// =====================================
#define IR_FD 22
#define IR_FE 25
#define IR_TD 24
#define IR_TE 23

#define BRANCO 0
#define PRETO  1

#define ARENA BRANCO
#define BORDA PRETO

// =====================================
// CONFIGURACOES
// =====================================
int velocidadeAtaque = 255;//255
int velocidadeBusca  = 200;//180,200
int velocidadeRecuo  = 220;//220

int distanciaMaximaAtaque = 100;
int distanciaMinimaAtaque = 35;

float distanciaFrenteDireita = 999;
float distanciaFrenteEsquerda = 999;
float distanciaTraseiraDireita = 999;
float distanciaTraseiraEsquerda = 999;

bool modoAutomatico = true;
bool mostrarSensoresContinuamente = false;
bool mostrarAcaoContinuamente = false;
bool diagnosticoCompleto = false;

unsigned long tempoAnteriorSerial = 0;
unsigned long intervaloSerial = 1500;

String acaoAtual = "Robo iniciado";

// =====================================
// SETUP
// =====================================
void setup() {
  Serial.begin(9600);

  pinMode(IR_FD, INPUT);
  pinMode(IR_FE, INPUT);
  pinMode(IR_TD, INPUT);
  pinMode(IR_TE, INPUT);

  pararRobo();

  mostrarMenuSerial();

  Serial.println("Aguardando 5 segundos antes de iniciar...");
  delay(5000);

  frente(velocidadeBusca);
}

// =====================================
// LOOP
// =====================================
void loop() {
  verificarComandosSerial();

  lerTodosUltrassonicos();

  if (modoAutomatico) {
    controleAutomatico();
  }

  mostrarInformacoesContinuas();
}

// =====================================
// COMANDOS SERIAL
// =====================================
void verificarComandosSerial() {
  if (Serial.available() > 0) {
    char tecla = Serial.read();

    if (tecla == 'h' || tecla == 'H') {
      modoAutomatico = false;
      mostrarSensoresContinuamente = false;
      mostrarAcaoContinuamente = false;
      diagnosticoCompleto = false;

      pararRobo();

      Serial.println();
      Serial.println("ROBO PAUSADO.");
      Serial.println("Modo automatico desativado.");
      Serial.println("Motores parados.");
      mostrarMenuSerial();
    }

    else if (tecla == 's' || tecla == 'S') {
      mostrarDiagnosticoCompleto();
    }

    else if (tecla == 'c' || tecla == 'C') {
      mostrarSensoresContinuamente = !mostrarSensoresContinuamente;
      diagnosticoCompleto = false;

      Serial.print("Leitura continua dos sensores: ");
      Serial.println(mostrarSensoresContinuamente ? "ATIVADA" : "DESATIVADA");
    }

    else if (tecla == 'a' || tecla == 'A') {
      mostrarAcaoContinuamente = !mostrarAcaoContinuamente;
      diagnosticoCompleto = false;

      Serial.print("Exibicao continua da acao atual: ");
      Serial.println(mostrarAcaoContinuamente ? "ATIVADA" : "DESATIVADA");
    }

    else if (tecla == 'd' || tecla == 'D') {
      diagnosticoCompleto = !diagnosticoCompleto;

      if (diagnosticoCompleto) {
        mostrarSensoresContinuamente = false;
        mostrarAcaoContinuamente = false;
      }

      Serial.print("Diagnostico completo continuo: ");
      Serial.println(diagnosticoCompleto ? "ATIVADO" : "DESATIVADO");
    }

    else if (tecla == 'p' || tecla == 'P') {
      modoAutomatico = false;
      pararRobo();

      Serial.println("Robo parado manualmente.");
    }

    else if (tecla == 'r' || tecla == 'R') {
      modoAutomatico = true;
      diagnosticoCompleto = true;

      Serial.println("Modo automatico ATIVADO.");
      Serial.println("Diagnostico completo tambem foi ATIVADO.");
    }
  }
}

void mostrarMenuSerial() {
  Serial.println();
  Serial.println("============== MENU SERIAL ==============");
  Serial.println("h - Pausar robo, parar motores e mostrar menu");
  Serial.println("s - Mostrar sensores e acao uma unica vez");
  Serial.println("c - Ligar/desligar leitura continua dos sensores");
  Serial.println("a - Ligar/desligar exibicao continua da acao");
  Serial.println("d - Ligar/desligar diagnostico completo");
  Serial.println("p - Parar robo e desativar modo automatico");
  Serial.println("r - Retomar modo automatico com diagnostico completo");
  Serial.println("=========================================");
  Serial.println();
}

// =====================================
// EXIBICAO SERIAL
// =====================================
void mostrarInformacoesContinuas() {
  unsigned long tempoAtual = millis();

  if (tempoAtual - tempoAnteriorSerial >= intervaloSerial) {
    tempoAnteriorSerial = tempoAtual;

    if (diagnosticoCompleto) {
      mostrarDiagnosticoCompleto();
    } 
    else {
      if (mostrarSensoresContinuamente) {
        mostrarLeituraSensores();
      }

      if (mostrarAcaoContinuamente) {
        mostrarAcaoAtual();
      }
    }
  }
}

void mostrarDiagnosticoCompleto() {
  Serial.println();
  Serial.println("=============== DIAGNOSTICO DO ROBO ===============");

  mostrarAcaoAtual();
  mostrarLeituraSensores();

  Serial.println("====================================================");
  Serial.println();
}

void mostrarAcaoAtual() {
  Serial.print("ACAO ATUAL: ");
  Serial.println(acaoAtual);
  Serial.print("MODO AUTOMATICO: ");
  Serial.println(modoAutomatico ? "ATIVADO" : "DESATIVADO");
  Serial.println();
}

void mostrarLeituraSensores() {
  int irFrenteDireita = digitalRead(IR_FD);
  int irFrenteEsquerda = digitalRead(IR_FE);
  int irTraseiraDireita = digitalRead(IR_TD);
  int irTraseiraEsquerda = digitalRead(IR_TE);

  Serial.println("SENSORES ULTRASSONICOS:");

  Serial.print("Ultrassonico frente direita    : ");
  Serial.print(distanciaFrenteDireita);
  Serial.println(" cm");

  Serial.print("Ultrassonico frente esquerda   : ");
  Serial.print(distanciaFrenteEsquerda);
  Serial.println(" cm");

  Serial.print("Ultrassonico traseira direita  : ");
  Serial.print(distanciaTraseiraDireita);
  Serial.println(" cm");

  Serial.print("Ultrassonico traseira esquerda : ");
  Serial.print(distanciaTraseiraEsquerda);
  Serial.println(" cm");

  Serial.println();

  Serial.println("SENSORES INFRAVERMELHOS:");

  Serial.print("IR frente direita    : ");
  Serial.print(irFrenteDireita);
  Serial.print(" - ");
  Serial.println(irFrenteDireita == ARENA ? "BRANCO / ARENA" : "PRETO / BORDA");

  Serial.print("IR frente esquerda   : ");
  Serial.print(irFrenteEsquerda);
  Serial.print(" - ");
  Serial.println(irFrenteEsquerda == ARENA ? "BRANCO / ARENA" : "PRETO / BORDA");

  Serial.print("IR traseira direita  : ");
  Serial.print(irTraseiraDireita);
  Serial.print(" - ");
  Serial.println(irTraseiraDireita == ARENA ? "BRANCO / ARENA" : "PRETO / BORDA");

  Serial.print("IR traseira esquerda : ");
  Serial.print(irTraseiraEsquerda);
  Serial.print(" - ");
  Serial.println(irTraseiraEsquerda == ARENA ? "BRANCO / ARENA" : "PRETO / BORDA");

  Serial.println();
}

// =====================================
// LEITURA DOS ULTRASSONICOS COM BIBLIOTECA
// =====================================
float medirDistanciaUltrassonico(Ultrasonic &sensor) {
  long microsec = sensor.timing();

  if (microsec > 0) {
    return sensor.convert(microsec, Ultrasonic::CM);
  } else {
    return 999;
  }
}

void lerTodosUltrassonicos() {
  distanciaFrenteDireita = medirDistanciaUltrassonico(ultrassonicoFD);
  delay(30);

  distanciaFrenteEsquerda = medirDistanciaUltrassonico(ultrassonicoFE);
  delay(30);

  distanciaTraseiraDireita = medirDistanciaUltrassonico(ultrassonicoTD);
  delay(30);

  distanciaTraseiraEsquerda = medirDistanciaUltrassonico(ultrassonicoTE);
  delay(30);
}

// =====================================
// MOVIMENTOS
// =====================================
void definirVelocidadeMotores(int velocidade) {
  motor1.setSpeed(velocidade);
  motor2.setSpeed(velocidade);
  motor3.setSpeed(velocidade);
  motor4.setSpeed(velocidade);
}

void frente(int velocidade) {
  definirVelocidadeMotores(velocidade);

  motor1.run(FORWARD);
  motor3.run(FORWARD);
  motor2.run(FORWARD);
  motor4.run(FORWARD);

  acaoAtual = "Movendo para frente";      
}

void tras(int velocidade) {
  definirVelocidadeMotores(velocidade);

  motor1.run(BACKWARD);
  motor3.run(BACKWARD);
  motor2.run(BACKWARD);
  motor4.run(BACKWARD);

  acaoAtual = "Movendo para tras";
}

void girarDireita(int velocidade) {
  definirVelocidadeMotores(velocidade);

  motor1.run(FORWARD);
  motor3.run(FORWARD);
  motor2.run(BACKWARD);
  motor4.run(BACKWARD);

  acaoAtual = "Girando para direita";
}

void girarEsquerda(int velocidade) {
  definirVelocidadeMotores(velocidade);

  motor1.run(BACKWARD);
  motor3.run(BACKWARD);
  motor2.run(FORWARD);
  motor4.run(FORWARD);

  acaoAtual = "Girando para esquerda";  
} 

void pararRobo() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);

  acaoAtual = "Robo parado";
}

// =====================================
// CONTROLE AUTOMATICO
// =====================================
void controleAutomatico() {
  int irFrenteDireita = digitalRead(IR_FD);
  int irFrenteEsquerda = digitalRead(IR_FE);
  int irTraseiraDireita = digitalRead(IR_TD);
  int irTraseiraEsquerda = digitalRead(IR_TE);

  // PRIORIDADE 1: BORDA PRETA NA FRENTE
  if (irFrenteDireita == BORDA || irFrenteEsquerda == BORDA) {
    tras(velocidadeRecuo);
    acaoAtual = "Borda preta detectada na frente. Recuando.";
    delay(500);

    if (irFrenteDireita == BORDA && irFrenteEsquerda != BORDA) {
      girarEsquerda(velocidadeRecuo);
      acaoAtual = "Borda preta na frente direita. Corrigindo para esquerda.";
    } 
    else if (irFrenteEsquerda == BORDA && irFrenteDireita != BORDA) {
      girarDireita(velocidadeRecuo);
      acaoAtual = "Borda preta na frente esquerda. Corrigindo para direita.";
    } 
    else {
      girarDireita(velocidadeRecuo);
      acaoAtual = "Borda preta nos dois sensores frontais. Girando para direita.";
    }

    delay(600);
    return;
  }

  // PRIORIDADE 2: BORDA PRETA NA TRASEIRA
  if (irTraseiraDireita == BORDA || irTraseiraEsquerda == BORDA) {
    frente(velocidadeRecuo);
    acaoAtual = "Borda preta detectada na traseira. Avancando.";
    delay(500);

    if (irTraseiraDireita == BORDA && irTraseiraEsquerda != BORDA) {
      girarEsquerda(velocidadeRecuo);
      acaoAtual = "Borda preta na traseira direita. Corrigindo para esquerda.";
    } 
    else if (irTraseiraEsquerda == BORDA && irTraseiraDireita != BORDA) {
      girarDireita(velocidadeRecuo);
      acaoAtual = "Borda preta na traseira esquerda. Corrigindo para direita.";
    } 
    else {
      girarDireita(velocidadeRecuo);
      acaoAtual = "Borda preta nos dois sensores traseiros. Girando para direita.";
    }

    delay(600);
    return;
  }

  // PRIORIDADE 3: ADVERSARIO NA FRENTE
  if (
    (distanciaFrenteDireita < distanciaMaximaAtaque && distanciaFrenteEsquerda < distanciaMaximaAtaque) ||
    distanciaFrenteDireita < distanciaMinimaAtaque ||
    distanciaFrenteEsquerda < distanciaMinimaAtaque
  ) {
    frente(velocidadeAtaque);
    acaoAtual = "Adversario detectado na frente. Atacando para frente.";
    delay(300);
    return;
  }

  if (distanciaFrenteDireita < distanciaMaximaAtaque) {
    girarDireita(velocidadeBusca);
    acaoAtual = "Adversario detectado na frente direita. Girando para direita.";
    delay(300);
    return;
  }

  if (distanciaFrenteEsquerda < distanciaMaximaAtaque) {
    girarEsquerda(velocidadeBusca);
    acaoAtual = "Adversario detectado na frente esquerda. Girando para esquerda.";
    delay(300); 
    return;
  }

  // PRIORIDADE 4: ADVERSARIO ATRAS
  if (
    (distanciaTraseiraDireita < distanciaMaximaAtaque && distanciaTraseiraEsquerda < distanciaMaximaAtaque) ||
    distanciaTraseiraDireita < distanciaMinimaAtaque ||
    distanciaTraseiraEsquerda < distanciaMinimaAtaque
  ) {
    tras(velocidadeAtaque);
    acaoAtual = "Adversario detectado atras. Indo para tras.";
    delay(300);
    return;
  }

  if (distanciaTraseiraDireita < distanciaMaximaAtaque) {
    girarDireita(velocidadeBusca);
    acaoAtual = "Adversario detectado na traseira direita. Girando para direita.";
    delay(300);
    return;
  }

  if (distanciaTraseiraEsquerda < distanciaMaximaAtaque) {
    girarEsquerda(velocidadeBusca);
    acaoAtual = "Adversario detectado na traseira esquerda. Girando para esquerda.";
    delay(300);
    return;
  }

  // PRIORIDADE 5: BUSCA
  girarDireita(velocidadeBusca);
  acaoAtual = "Nenhum adversario detectado. Procurando alvo.";
  delay(300);
}

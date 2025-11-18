// Projeto EchoBot: ESP8266 + HC-SR04 + Buzzer + 2 LEDs + Laser

// O projeto utiliza um sensor ultrassónico HC-SR04 para medir distâncias.
// Quando deteta um obstáculo o sistema:
// - acende 2 LEDs 
// - liga 1 Buzzer
// - manda 1 Laser

// PIN DO SENSOR HC-SR04
const int PIN_TRIG = D6;  // Pin do HC-SR04 (GPIO12)
const int PIN_ECHO = D5;  // Pin do HC-SR04 (GPIO14)

// PIN DOS LEDs
const int LED1_PIN = D1;  // LED 1 ligado ao pino D1 (GPIO5)
const int LED2_PIN = D2;  // LED 2 ligado ao pino D2 (GPIO4)

// PIN DO BUZZER
const int BUZZER_PIN = D7;  // Buzzer Ativo ligado ao pino D7 (GPIO13)

// PIN DO LASER
const int LASER_PIN = D3;  // Laser ligado ao pino D3 (GPIO0)

// DISTÂNCIA DA DETEÇÃO DO MOVIMENTO
float threshold_cm = 35.0;  // Se a distância <= 35cm, ativa LEDs, buzzer e laser

// SUGESTÃO DA AI - SMOOTHING
// Serve para evitar leituras erradas. Fazer 5 medições rapidamente e efetuar a média.
const int samples = 5;  // Nº de medições
const int settle_ms = 50;  // Tempo de espera (ms) entre cada amostra

// CONSTANTE DA VELOCIDADE DO SOM
const float SOUND_SPEED = 0.0343f;

// CONFIGURAÇÃO DO BUZZER
int buzzerFreq = 2000;  // Frequência do som em Hz
int buzzerDuration = 100; // Duração de cada beep (ms)
int buzzerPause = 100;  // Pausa entre beeps (ms)
unsigned long lastBuzzerState = 0;  // Último estado do buzzer
bool buzzerState = false;  // Estado atual do buzzer (true = a tocar, false = em pausa)

// FUNÇÃO SETUP 
// Executada no início vai configurar todos os pins e estados dos mesmos.
void setup() {
  // CONFIGURAÇÃO DOS MODOS DOS PINS
  pinMode(PIN_TRIG, OUTPUT);  // TRIG é output porque envio o pulso ultrassónico
  pinMode(PIN_ECHO, INPUT); // ECHO é input porque recebe eco do som
  pinMode(LED1_PIN, OUTPUT);  // LED1 é output, é ativado quando o som é interrompido
  pinMode(LED2_PIN, OUTPUT);  // LED2 é output, é ativado quando o som é interrompido
  pinMode(BUZZER_PIN, OUTPUT);  // Buzzer é saída, imite sons quando é detetado movimento
  pinMode(LASER_PIN, OUTPUT); // Laser é saída, imite laser quando é detetado movimento

  // INICAR O ESTADO INICIAL DOS PINS (TODOS DESLIGADOS)
  digitalWrite(PIN_TRIG, LOW);    
  digitalWrite(LED1_PIN, LOW);   
  digitalWrite(LED2_PIN, LOW);    
  digitalWrite(BUZZER_PIN, LOW);  
  digitalWrite(LASER_PIN, LOW);  

  // INICIAR A COMUNICAÇÃO COM O SERIAL MONITOR
  Serial.begin(9600);  // Inicia comunicação série a 9600 baud - NO SERIAL MONITOR TEM DE ESTAR 9600, SENÃO AS MENSAGENS NÃO APARECEM CORRETAMENTE
  delay(200);         
  Serial.println("EchoBot ON");
}

// FUNÇÃO PARA LER A DISTÂNCIA EM CENTÍMETROS
// Esta função envia um pulso ultrassónico e mede o tempo até receber o eco
float readDistanceCM() {
  // Garantir que o pin de TRIGGER ESTÁ LOW
  digitalWrite(PIN_TRIG, LOW);   
  delayMicroseconds(2);         
  
  // Coloca o pin de TRIGGER EM HIGH, mantém ativa 10s e desliga
  digitalWrite(PIN_TRIG, HIGH);  
  delayMicroseconds(10);         
  digitalWrite(PIN_TRIG, LOW); 

  // CALCULAR O TEMPO DE ESPERA PELO ECHO
  // Distância máxima de 2 metros, faz o cálculo do tempo.
  // Se o som voltar mais rápido quer dizer que encontrou obstáculo e ativa o EchoBot.
  const unsigned long maxDistanceCM = 200;
  unsigned long maxEchoTime = (unsigned long)((maxDistanceCM * 2.0f) / SOUND_SPEED);
  
  // MEDIR O TEMPO QUE O SOM DEMORA A VOLTAR
  // pulseIn() mede quanto tempo o pino ECHO fica em HIGH (tempo de ida e volta do som)
  unsigned long duration = pulseIn(PIN_ECHO, HIGH, maxEchoTime);

  // VERIFICAÇÃO E CÁLCULO DA DISTÂNCIA
  if (duration == 0) return NAN;  // Se não houve eco, retorna NAN (Not A Number)
  
  // Fórmula: distância = (tempo × velocidade_do_som) / 2
  // Dividimos por 2 porque o som faz ida e volta
  return (duration * SOUND_SPEED) / 2.0f;
}

// FUNÇÃO PARA LER DISTÂNCIA COM SUAVIZAÇÃO (MÉDIA DE VÁRIAS LEITURAS) - SMOOTHING SUGERIDO POR AI
// Esta função faz várias leituras e calcula a média para obter um valor mais estável
float readAveragedDistanceCM() {
  int valid = 0; // Contador de leituras
  float sum = 0.0f; // Soma das leituras
  
  // RECOLHA DE 5 AMOSTRAS - variável samples foi definida no início
  for (int i = 0; i < samples; i++) {
    float d = readDistanceCM();  // Faz uma leitura
    
    // Se a leitura for válida (não é NAN), adiciona à soma
    if (!isnan(d)) { 
      sum += d; 
      valid++; 
    }
    
    delay(settle_ms);  // Espera entre leituras - variável definida no início.
  }
  
  // CÁLCULO E RETORNO DA MÉDIA
  // Se houver pelo menos uma leitura válida, retorna a média
  // Caso contrário, retorna NAN
  return (valid > 0) ? (sum / valid) : NAN;

        // EQUIVALENTE A 
        //   if (valid > 0) {
        //     return sum / valid;
        // } else {
        //     return NAN;
        // }
}

// FUNÇÃO PARA CONTROLAR OS LEDs
// Liga ou desliga ambos os LEDs simultaneamente
void setLEDs(bool on) {
  digitalWrite(LED1_PIN, on ? HIGH : LOW);  // Se on=true, liga (HIGH), senão desliga (LOW)
  digitalWrite(LED2_PIN, on ? HIGH : LOW);  // Mesmo comportamento para LED2

  // As expressões são equivalentes a if's. Se ON for igual a HIGH segue, senão devolve LOW.
}

// FUNÇÃO PARA CONTROLAR O LASER
// Liga ou desliga o módulo laser
void setLaser(bool on) {
  digitalWrite(LASER_PIN, on ? HIGH : LOW); // Se on=true, liga, senão desliga

  // Equivalente a um if.
}

// FUNÇÃO PARA CONTROLAR O PADRÃO DE BEEPS DO BUZZER
// Cria um padrão intermitente de beeps quando shouldBeep é true
void updateBuzzer(bool shouldBeep) {
  // SE NÃO DEVE FAZER BEEP, DESLIGA TUDO
  if (!shouldBeep) {
    noTone(BUZZER_PIN); // Pára qualquer som no buzzer
    buzzerState = false;  // Marca estado como desligado
    return;
  }

  // CRIAÇÃO DO PADRÃO INTERMITENTE DE BEEPS
  unsigned long currentTime = millis(); // Obtém o tempo atual em milissegundos
  
  if (buzzerState) {
    // BUZZER ESTÁ A TOCAR - VERIFICA SE DEVE PARAR
    if (currentTime - lastBuzzerState >= buzzerDuration) {
      noTone(BUZZER_PIN); // Pára o som
      buzzerState = false;  // Marca como em pausa
      lastBuzzerState = currentTime;  // Regista o momento da mudança de estado
    }
  } else {
    // BUZZER ESTÁ EM PAUSA - VERIFICA SE DEVE TOCAR NOVAMENTE
    if (currentTime - lastBuzzerState >= buzzerPause) {
      tone(BUZZER_PIN, buzzerFreq); // Inicia o som na frequência definida
      buzzerState = true;  // Marca como a tocar
      lastBuzzerState = currentTime;  // Regista o momento da mudança de estado
    }
  }
}

// FUNÇÃO LOOP - EXECUTADA CONTINUAMENTE
void loop() {
  // LEITURA DA DISTÂNCIA
  float d = readAveragedDistanceCM();  

  // VERIFICAÇÃO SE A LEITURA É VÁLIDA
  if (isnan(d)) {
    // SEM LEITURA VÁLIDA - DESLIGA TUDO
    setLEDs(false);  // Apaga os LEDs
    updateBuzzer(false);  // Desliga o buzzer
    setLaser(false);  // Desliga o laser
    Serial.println("Nada a fazer, estudasses...");
  } else {
    // LEITURA VÁLIDA - VERIFICA SE OBJETO ESTÁ PRÓXIMO
    bool near = (d <= threshold_cm);  // true se distância <= 35cm
    
    // Ativa ou desativa os componentes conforme a proximidade
    setLEDs(near);  // Liga LEDs 
    updateBuzzer(near);  // Ativa beeps 
    setLaser(near);  // Liga laser 

    // ENVIA INFORMAÇÕES
    Serial.print("Distancia: ");
    Serial.print(d, 1);  // Mostra distância com 1 casa decimal
    Serial.print(" cm  -> LEDs ");
    Serial.print(near ? "LIGADOS" : "DESLIGADOS");
    Serial.print(" Buzzer ");
    Serial.print(near ? "A TOCAR" : "DESLIGADO");
    Serial.print(" Laser ");
    Serial.println(near ? "LIGADO" : "DESLIGADO");
  }

  // PAUSA ANTES DA PRÓXIMA ITERAÇÃO
  delay(20);  // Espera 20ms antes de fazer nova leitura
}
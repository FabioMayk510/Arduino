/*Pinos*/

//Pinos para recebimento de dados dos sensores infravermelhos
const int infra_1 = 0; //esquerda
const int infra_2 = 2; //direita

//Pino responsável por enviar o sinal para acender ou apagar o LED
const int led = 10;

//Pinos responsáveis por enviar e receber os sinais do sensor ultrassônico
const int ECHO = 8;
const int TRIG = 9;

//Pinos responsáveis por controlar o motor_A - esquerdo
int IN1 = 2 ;
int IN2 = 4 ;
int velocidadeA = 3;

//Pinos responsáveis por controlar o motor_B - direito
int IN3 = 6 ;
int IN4 = 7 ;
int velocidadeB = 5;

//Pino responsável por enviar os sinais para o buzzer
const int buzzer = 12;


/*Variáveis*/

//Variável que define as configurações de Distancia
const int distancia_carro = 5;

int distancia = 0;

//Variáveis usadas para calcular a curva de som do buzzer
float seno;
int frequencia;

//variavel encarregada de controlar a velocidade do motor
int velocidade = 250;

//variáveis usadas para controle dos sensores infravermelhos
int s1 = 0;
int s2 = 0;

//Variáveis usadas para calcular a desaceleração dos motores individualmente
int des1 = 0;
int des2 = 0;

/*Definição inicial do código, aqui é escrito tudo aquilo que será executado apenas UMA vez no início do algorítmo (antes de entrar em loop)*/

void setup() {

    pinMode(infra_1, INPUT);
    pinMode(infra_2, INPUT);
    Serial.begin(9600);

    //Indica que os 4 pinos principais da Ponte H serão serão de saída de dados (a placa arduino enviará dados para esses pinos)
    pinMode(IN1,OUTPUT);
    pinMode(IN2,OUTPUT);
    pinMode(IN3,OUTPUT);
    pinMode(IN4,OUTPUT);

    //Indica que os 2 pinos auxilares da Ponte H serão serão de saída de dados (a placa arduino enviará dados para esses pinos)
    pinMode(velocidadeA,OUTPUT);
    pinMode(velocidadeB,OUTPUT);

    //Indica o pino do Sensor Ultrassônico que enviará o sinal sonoro (OUTPUT -> a placa arduino enviará dados para esse pino) 
    //e o que receberá o mesmo sinal (INPUT -> a placa arduino receberá dados por esse pino) 
    pinMode(TRIG,OUTPUT);
    pinMode(ECHO,INPUT);

    //Define o pino do LED como saída de dados
    pinMode(led, OUTPUT);

    //Define o pino do buzzer como saída de dados
    pinMode(buzzer, OUTPUT);

    //Faz o carrinho esperar 5 mil milissegundos (5 segundos) antes de começar a andar
    delay(5000);

    //Inicia o Motor A para frente (inverter para mudar a direção)
    /*motor A*/

    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    
    /*motor B*/

    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
}

/* Loop do algorítmo, aqui será escrito aquilo que ficará em execução eternamente no microprocessador */

void loop() {
    retornaSensor();
    vel();
    run();
}


/*Funções*/

void retornaSensor(){
    s1 = analogRead(infra_1);
    s2 = analogRead(infra_2);
    Serial.print("sensor 1: ");
    Serial.print(s1);
    Serial.print("||");
    Serial.print("sensor 2: ");
    Serial.println(s2);

    distancia = sensor_morcego(TRIG,ECHO);
    Serial.print("distancia: ");
    Serial.print(distancia);
}

void vel(){
    des();
    int opt = obj();
    
    switch(opt){
        case 1:
            noTone(buzzer);
            velocidade = 250;
            digitalWrite(led, LOW);
            break;
        case 2:
            while(distancia <= distancia_carro){
                analogWrite(velocidadeA, 0);
                analogWrite(velocidadeB, 0);
                distancia = sensor_morcego(TRIG,ECHO);
                tocaBuzzer();
                digitalWrite(led, HIGH);
            }
            break;
        case 3:
            //se eu nao conseguir fazer o carrinho se manter em linha, usar esta parte para criar um vetor e manter o historico de alterações nos sensores infra, pra saber se ja esteve na linha e poder voltar ou se nao esteve na linha e deve avançar ate ela
        default:
            Serial.print("Sensor Ultrasonico com erro");
            break;
    }
}

void run(){
    analogWrite(velocidadeA, (velocidade - des1));
    analogWrite(velocidadeB, (velocidade - des2));
}

void des(){
    //desaceleração motor 2
    if(s1 > 750)
        des2 = (s1 - 750) / 2; //ESSE VALOR (2, 750)
    else
        des2 = 0;

    //desaceleração motor 1
    if(s2 > 750)
        des1 = (s2 - 750) / 2;
    else
        des1 = 0;
}

int obj(){
    if(distancia > distancia_carro)
        return 1;
    if (distancia <= distancia_carro)
        return 2;
    if (false)
        return 3;
}

//Função responsável por retornar a distância entre o Sensor Ultrassônico e o objeto à sua frente, através do envio de uma onda sonora e recebimento da mesma
//(caso colida com um objeto, a onda retornará), e assim calculando a proximidade à depender do tempo em que demora para a onda retornar
int sensor_morcego(int pinotrig,int pinoecho){
    digitalWrite(pinotrig,LOW);
    delayMicroseconds(2);
    digitalWrite(pinotrig,HIGH);
    delayMicroseconds(10);
    digitalWrite(pinotrig,LOW);
    return pulseIn(pinoecho,HIGH)/58;
}

//Função responsável por enviar a curva sonora para o buzzer, assim podendo reproduzir sons não lineares
void tocaBuzzer(){
    for(int x=0;x<180;x++){
        seno=(sin(x*3.1416/180));
        frequencia = 2000+(int(seno*1000));
        tone(buzzer,frequencia);
        delay(2);
    }
}
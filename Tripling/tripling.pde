
#define VystupPravy 6
#define VystupLavy 7
#define PravaPacka 4
#define LavaPacka 5

//o=neblikame, >0 blikame
volatile int PocetBliknutiPravy=0;
volatile int PocetBliknutiLavy=0;
int stavPravaPacka=LOW;
int stavLavaPacka=LOW;
int predstavPravaPacka=HIGH;
int predstavLavaPacka=HIGH;
int PravaStlacena=0;
int LavaStlacena=0;
long casStlaceniaPravaPacka=0;
long casStlaceniaLavaPacka=0;
long debounce=500;
long stlacenie=1000;

void setup(){
  //Pravy blinker na pine 2
  attachInterrupt(0,PocitajPravy,FALLING);
  //Lavy blinker na pine 3
  attachInterrupt(1,PocitajLavy,FALLING);
  pinMode(VystupPravy,OUTPUT);
  pinMode(VystupLavy,OUTPUT);
  pinMode(PravaPacka,INPUT);
  pinMode(LavaPacka,INPUT);
  digitalWrite(VystupPravy,HIGH);
  digitalWrite(VystupLavy,HIGH);
  Serial.begin(9600);
}

void loop(){
  //nacitame stav Packy
  stavPravaPacka = digitalRead(PravaPacka);
  stavLavaPacka = digitalRead(LavaPacka);
  
  if (predstavPravaPacka==HIGH && stavPravaPacka == LOW) {
    casStlaceniaPravaPacka=millis();
    Serial.print("Prava: ");
    Serial.print(predstavPravaPacka);
    Serial.print(" -> ");
    Serial.println(stavPravaPacka);
    PravaStlacena=0;
  }
  if (predstavLavaPacka==HIGH && stavLavaPacka == LOW) {
    casStlaceniaLavaPacka=millis();
    Serial.print("Lava: ");
    Serial.print(predstavLavaPacka);
    Serial.print(" -> ");
    Serial.println(stavLavaPacka);
    LavaStlacena=0;
  }
    
  if ((millis()-casStlaceniaPravaPacka)>debounce && stavPravaPacka == LOW && predstavPravaPacka == LOW && PravaStlacena==0){
    digitalWrite(VystupLavy,HIGH);
    PocetBliknutiLavy=0;
    digitalWrite(VystupPravy,HIGH);
    PocetBliknutiPravy=0;
    Serial.println("Prava: stlacena dlhsie ako debounce, aktual: ");
    Serial.print(stavPravaPacka);
    Serial.print(", pred: ");
    Serial.println(predstavPravaPacka);
  }
  if ((millis()-casStlaceniaLavaPacka)>debounce && stavLavaPacka == LOW && predstavLavaPacka == LOW && LavaStlacena==0){
    digitalWrite(VystupLavy,HIGH);
    PocetBliknutiLavy=0;
    digitalWrite(VystupPravy,HIGH);
    PocetBliknutiPravy=0;
    Serial.print("Lava: stlacena dlhsie ako debounce, aktual: ");
    Serial.print(stavLavaPacka);
    Serial.print(", pred: ");
    Serial.println(predstavLavaPacka);
  }
  
  if ((millis()-casStlaceniaPravaPacka)<stlacenie && stavPravaPacka == HIGH && predstavPravaPacka == LOW && PravaStlacena==0){
    digitalWrite(VystupPravy,LOW);
    PocetBliknutiPravy=1;
    Serial.print("Prava: stlacena kratsie ako stlacenie, aktual: ");
    Serial.print(stavPravaPacka);
    Serial.print(", pred: ");
    Serial.println(predstavPravaPacka);
  }
  if ((millis()-casStlaceniaLavaPacka)<stlacenie && stavLavaPacka == HIGH && predstavLavaPacka == LOW && LavaStlacena==0){
    digitalWrite(VystupLavy,LOW);
    PocetBliknutiLavy=1;
    Serial.print("Lava: stlacena kratsie ako stlacenie, aktual: ");
    Serial.print(stavLavaPacka);
    Serial.print(", pred: ");
    Serial.println(predstavLavaPacka);
  }
  
  if ((millis()-casStlaceniaPravaPacka)>stlacenie && stavPravaPacka == LOW && PravaStlacena==0){
    PravaStlacena=1;
    digitalWrite(VystupLavy,HIGH);
    PocetBliknutiLavy=0;
    digitalWrite(VystupPravy,HIGH);
    PocetBliknutiPravy=0;
    Serial.print("Prava: stlacena dlhsie ako stlacenie, aktual: ");
    Serial.println(stavPravaPacka);
  }
  if ((millis()-casStlaceniaLavaPacka)>stlacenie && stavLavaPacka == LOW && PravaStlacena==0){
    LavaStlacena=1;
    digitalWrite(VystupLavy,HIGH);
    PocetBliknutiLavy=0;
    digitalWrite(VystupPravy,HIGH);
    PocetBliknutiPravy=0;
    Serial.print("Lava: stlacena dlhsie ako stlacenie, aktual: ");
    Serial.println(stavPravaPacka);
  }
   
  predstavPravaPacka=stavPravaPacka;
  predstavLavaPacka=stavLavaPacka;
}

void PocitajPravy() {
  if (PocetBliknutiPravy>0) {
    PocetBliknutiPravy++;
    if (PocetBliknutiPravy==4) { // 3bliknutia, zaciatok je 1, nastaveny v mailLOOP
       digitalWrite(VystupPravy,HIGH);// stop blikania praveho blinkra
       PocetBliknutiPravy=0; // vynulovanie blikania laveho blinkra
    }
  }
}

void PocitajLavy() {
  if (PocetBliknutiLavy>0) {
    PocetBliknutiLavy++;
    if (PocetBliknutiLavy==4) { // 3bliknutia, zaciatok je 1, nastaveny v mailLOOP
       digitalWrite(VystupLavy,HIGH);// stop blikania praveho blinkra
       PocetBliknutiLavy=0; // vynulovanie blikania laveho blinkra
    }
  }
}

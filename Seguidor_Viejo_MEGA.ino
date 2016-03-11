/* 
 * Seguidor solar Seguidor Viejo (el de Faustino)
Con encoders
   
   
   */
  //DS3231 Reloj
  //programa corregido despues de romper los FC
  //para programarlo hay que quitar los pines 0 y 1, sino da error y no se puede subir el programa
 
#include <math.h>
#include <SunPos.h>
#include <Wire.h>                       // For some strange reasons, Wire.h must be included here
#include <TimeLib.h>
#include <SPI.h>
#include <Encoder.h>
#include <DS3231RTC.h>  // https://github.com/trunet/DS3231RTC/blob/master/DS3231RTC.cpp
//funciona también con la librería DS1307RTC , creo que es más seguro
// include the library code:
#include <LiquidCrystal.h>
#include <string.h>


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

Encoder EncA(18, 22); //18 y 22 son interrupts
Encoder EncZ(19, 24);
long PulsosA  = 0;
long PulsosZ  = 0;
int num_pant=0;
 // definimos el lugar, sun y time de acuerdo con sunpos.h
 cLocation lugar;
 cSunCoordinates sun;
 cTime fecha_hora_PSA;
 time_t t;
 
tmElements_t tm;
char time[20];
char buf [8];
unsigned long  delayTime;
unsigned long  displayTime;
//encoder con 1440 pulsos vuelta
float reductora=208.0;
float pulsos_encoder=14400;
float grados_pulso= 360/(reductora*pulsos_encoder);

float grados_pulso_acimut=  grados_pulso; //para acimut
float grados_pulso_cenit=   grados_pulso; //para cenit
float grados_acimut;
float grados_cenit;
float precision_acimut=1;
float precision_cenit=1;
float pulsos_acimut=0;
float pulsos_cenit=0;

// for your motor
int giroEste =    34;
int giroOeste =   36;
int giroNorte =   38;
int giroSur =     40;

int FC_este_pin=  26;
int FC_oeste_pin= 28;
int FC_norte_pin= 30;
int FC_sur_pin=   32; 
int manual_cenit_boton_pin =50;
int manual_acimut_boton_pin=52; //control manual mediante botonera activado
boolean manual_cenit_boton =50;
boolean manual_acimut_boton=52; //control manual mediante botonera activado
boolean manual_boton;
boolean manual_boton_anterior;


boolean FC_este;
boolean FC_oeste;
boolean FC_norte;
boolean FC_sur; //como no quedan entradas digitales se usa la analógica 3
boolean error_FC_acimut=false;
boolean error_FC_cenit=false;
boolean error_FC=false;
boolean error=false;
int num_error=0;
boolean manual=false;
boolean manual_giraNorte=false;
boolean manual_giraSur=false;
boolean manual_giraEste=false;
boolean manual_giraOeste=false;
int estado_giroE;
int estado_giroO;
int estado_giroN;
int estado_giroS;



float acimut_sol; //referido como 0 a mediodía, negativo al este(mañana)
float cenit_sol; //referido como 0 a mediodía, negativo al este(mañana)
float acimut_anterior=0;
float cenit_anterior=0;

float angulo_FC_este= -177.5;
float angulo_FC_norte=  5.0;
float acimut_actual=  0.0;
float cenit_actual=   0.0;
float acimut_max=     120;
float cenit_max=    85;
boolean iniciado=false;
boolean iniciado_Este=false;
boolean iniciado_Norte=false;
int j;


int reference;

float angulo_retorno=98.0; //angulo cenital para decir cuando vuelve a reposo
int noche=0;
int noche_anterior=0;
unsigned long t_anterior=0;
int t_print=2000;

char message[] = "Seguidor solar Ciemat  - Miguel alonso abella";
int previous = 0;
int pos = 0;

////////////////////////////////////////////SETUP/////////////////////////////////////////////////
void setup()  {
  Serial.begin(9600);
  Serial.println ("INICIO");
  pinMode(giroNorte, OUTPUT);
  pinMode(giroSur, OUTPUT);
  pinMode(giroEste, OUTPUT);
  pinMode(giroOeste, OUTPUT);
  pinMode(FC_este_pin, INPUT);
  pinMode(FC_oeste_pin, INPUT);
  pinMode(FC_norte_pin, INPUT);
  pinMode(FC_sur_pin, INPUT);
  
  pinMode( manual_acimut_boton_pin, INPUT);
  pinMode( manual_cenit_boton_pin, INPUT);
  
  digitalWrite(giroOeste, HIGH); //recordar que los relés van al revés
  digitalWrite(giroEste, HIGH);
  digitalWrite(giroNorte, HIGH);
  digitalWrite(giroSur, HIGH);
  
  
  
     // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0,0);
  lcd.print("Ciemat Suntracking");
  lcd.setCursor(0,1);
  lcd.print("2016:");

  Serial.println("Comprobando la hora");

   //while (!Serial) ; // Needed for Leonardo only
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if (timeStatus() != timeSet) 
     Serial.println(F("Unable to sync with the RTC"));
  else
     Serial.println(F("RTC has set the system time"));  


   Serial.println("comienzo");
  
 lugar.dLongitude=-3.728244;
 lugar.dLatitude=40.454988;
 sun.dZenithAngle=0;
 sun.dAzimuth=0;
 
  Serial.println("Inicio de loop");
  printLine(1);
  delay(2000);
}

///////////////////////////////////////////////////////////LOOP////////////////////////////////////////////////
void loop(){ 
  finales_carrera();
     
  
  
  if (Serial.available()) {
       procesaSerie();
       }

       if(timeStatus()!= timeNotSet) 
          {
            //=======================================
           // fecha de sunpos PSA
          fecha_hora_PSA.iYear=year();
          fecha_hora_PSA.iMonth=month();
          fecha_hora_PSA.iDay=day();
          fecha_hora_PSA.dHours=hour();
          fecha_hora_PSA.dMinutes=minute();
          fecha_hora_PSA.dSeconds=second();
          sunpos(fecha_hora_PSA, lugar, &sun);
          acimut_sol=sun.dAzimuth-180;
          cenit_sol=sun.dZenithAngle;  
          //========================================== 
              }
    
    
  if(cenit_sol>angulo_retorno){ // de noche a reposo
    cenit_sol=0;
    acimut_sol=0;
    noche=true;
  }
  else {noche=false;}
  
  if (noche_anterior==1 && noche ==0){
    iniciado=false;
    iniciado_Este=false;
    iniciado_Norte=false;
    } //por la mañana inicializa.
  noche_anterior=noche;  
  
  if (!iniciado && !manual) {
    inicializa();   
    }  
    
    finales_carrera();  
    if (!error && !manual){
      Seguir_sol();
    }
    if (manual){
      iniciado=false;
      iniciado_Este=false;
      iniciado_Norte=false;
      if (manual_giraNorte && manual_giraSur){manual_giraNorte=0;  manual_giraSur=0; }
      if (manual_giraEste && manual_giraOeste){manual_giraEste=0;  manual_giraOeste=0; }
      if (manual_giraNorte){gira_Norte(1);}else{gira_Norte(0);}
      if (manual_giraSur){gira_Sur(1);}else{gira_Sur(0);}
      if (manual_giraEste){gira_Este(1);}else{gira_Este(0);}
      if (manual_giraOeste){gira_Oeste(1);}else{gira_Oeste(0);}
     }
      
      display();
 if ((millis()-t_anterior)>t_print){
      digitalClockDisplay();
      printangles();
      t_anterior=millis();
    }
}

void printangles(){
  Serial.print("Acimut_actual: ");
  Serial.print(acimut_actual); 
  Serial.print(" Acimut Sol: ");
  Serial.print(acimut_sol); 
  Serial.print(" err: ");
  Serial.print(acimut_sol-acimut_actual); 
  Serial.print("   Cenit_actual: ");
  Serial.print(cenit_actual); 
  Serial.print(" Cenit Sol: ");
  Serial.println(cenit_sol);
  Serial.print(" err: "); 
  Serial.println(cenit_sol-cenit_actual); 

  Serial.print("cenit_sol: ");
  Serial.println(sun.dZenithAngle);
 
  
      if (manual){
          Serial.print(" Manual : ");Serial.print(manual);
          if (manual_giraNorte){Serial.println(F("Manual gira Norte:"));}
          if (manual_giraSur){Serial.println(F("Manual gira Sur:"));}
          if (manual_giraEste){Serial.println(F("Manual gira Este:"));}
          if (manual_giraOeste){Serial.println(F("Manual gira Oeste:"));}
      }
      if (error){
          Serial.print(" Error : ");
          Serial.println(error);
            }
            Serial.print("Iniciado:");Serial.print(iniciado);

      //datos para el raspberry pi
      Serial.println("INI:");
      String s2=String(year())+','+String(month())+','+String(day());
      s2 += ','+String(hour())+','+String(minute())+','+String(second())+',';
      Serial.print(s2);
      Serial.print(sun.dAzimuth-180);
      Serial.print(',');
      Serial.print(sun.dZenithAngle);
      Serial.print(',');
      Serial.print(acimut_actual);
      Serial.print(',');
      Serial.print(cenit_actual);
      
      
    
      s2   = ','+String(manual)+','+String(manual_giraEste)+','+String(manual_giraOeste)+','+String(manual_giraNorte)+','+String(manual_giraSur);
      s2 +=','+String(FC_este)+','+String(FC_oeste)+','+String(FC_norte)+','+String(FC_sur);
      s2 +=','+String(error)+','+String(iniciado);
      s2 +=','+String(iniciado_Este)+','+String(iniciado_Norte);
      s2 +=','+String(estado_giroE)+','+String(estado_giroO)+','+String(estado_giroN)+','+String(estado_giroS);
      s2 +=','+String(manual_acimut_boton)+','+String(manual_cenit_boton)+',';
      Serial.println(s2);
      Serial.println(",FIN");
            

}
  
int digitalClockDisplay(){
   
  if (delayTime >= millis() ) {
        return 0;
  }else{
    delayTime = millis() + 2000UL;
  }
  
   // digital clock display of the time
  Serial.println(F("--------------------------------->>>"));
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print("  ");
  Serial.print(day());
  Serial.print("-");
  Serial.print(month());
  Serial.print("-");
  Serial.print(year()); 
  Serial.println(); 
  
  Serial.print(F("sun.dZenithAngle: "));
  Serial.print(sun.dZenithAngle);
  Serial.print(F("  sun.dAzimuth:  "));
  Serial.print(sun.dAzimuth-180); 
  Serial.println(); 
}


void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
   Serial.print('0');
   Serial.print(digits);
}

   
void finales_carrera(){
      int k=0;
      FC_este =   digitalRead(FC_este_pin);
      FC_oeste =  digitalRead(FC_oeste_pin);
      FC_norte =  digitalRead(FC_norte_pin);
      FC_sur =    digitalRead(FC_sur_pin);
      manual_cenit_boton=digitalRead(manual_cenit_boton_pin);
      manual_acimut_boton=digitalRead(manual_acimut_boton_pin);

      error_FC_acimut=false;
      error_FC_cenit=false;
      error_FC=false;
      error=false;
      if(!FC_este && !FC_oeste){error_FC_acimut=true;num_error=1;}
      if(!FC_norte && !FC_sur){error_FC_cenit=true;num_error=2;}
      if (error_FC_acimut || error_FC_cenit){error_FC=true;error=true;}
}   

void inicializa(){
  if (iniciado){
    return;
    }
  
  lcd.setCursor(0,1);
  lcd.print(F("Inicializando:"));
  
  Serial.println("incializando... Busco Fin carrera Norte ");
  FC_norte = digitalRead(FC_norte_pin);
  FC_este = digitalRead(FC_este_pin);
     
    //INICIALIZA CENIT, primero en cenit, luego en acimut     // busca el final de carrerra    //moverse al este hasta que encuentre el final carrera
     lcd.clear();
    if(FC_norte && !iniciado_Norte){
    FC_norte = digitalRead(FC_norte_pin);
    gira_Norte(1);
   
    Serial.println(F("Buscando final carrera Norte:")); //gira al Norte
    lcd.setCursor(0,0);
    lcd.print(F("Busc FC N :"));
    lcd.setCursor(0,1);
    lcd.print(F("Pul:"));
    lcd.print(EncZ.read());
    }else{
      gira_Norte(0);
      
      cenit_actual=angulo_FC_norte; 
      EncZ.write(0);
      iniciado_Norte=true;
    }
    
    // INICIALIZA ACIMUT    // busca el final de carrerra   //moverse al este hasta que encuentre el final carrera
    Serial.println(F("Buscando final carrera Este:")); //gira al Este
   
    if (FC_este && !iniciado_Este){
    FC_este = digitalRead(FC_este_pin);
    gira_Este(1);
    
    Serial.println(F("Buscando final carrera Este:")); //gira al Norte
    lcd.setCursor(0,0);
    lcd.print(F("Busc FC E:"));
    lcd.setCursor(0,1);
    lcd.print(F("Pul:"));
    lcd.print(EncA.read());
    }else{
      gira_Este(0);
      
      acimut_actual=angulo_FC_este;
      EncA.write(0);
      iniciado_Este=true;
    } 

    if (iniciado_Este && iniciado_Norte){
      iniciado=true;
      Serial.println(F("Inicializado....OK"));
      Serial.println(F("Esperando 5 segundos"));
      lcd.setCursor(0,1);
      lcd.print(F("Esperando 5 seg:"));
      delay(5000);
      Serial.println(F("Siguiendo al sol:"));
    }
} //fin de inicializa

  //==================================GIROS=============================================
  void gira_Este(bool mueve){
     //Serial.print("girando al este:");
     //Serial.println(EncA.read());
     if(FC_este && mueve){digitalWrite(giroEste, LOW);estado_giroE=1;}else{digitalWrite(giroEste, HIGH);estado_giroE=0;}
   }
  
  void gira_Oeste(bool mueve){
    //Serial.print("girando al Oeste:");
    //Serial.println(EncA.read());
    if(FC_oeste && mueve){digitalWrite(giroOeste, LOW);estado_giroO=1;}else{digitalWrite(giroOeste, HIGH);estado_giroO=0;}
 }
  
  void gira_Norte(bool mueve){
    //Serial.print("girando al Norte:");
    // Serial.println(EncZ.read());
    if(FC_norte && mueve){digitalWrite(giroNorte, LOW);estado_giroN=1;}else{digitalWrite(giroNorte, HIGH);estado_giroN=0;}
 }
  
   void gira_Sur(bool mueve){
   //Serial.print("girando al Sur:");
   // Serial.println(EncZ.read());
   if(FC_sur && mueve){digitalWrite(giroSur, LOW);estado_giroS=1;}else{digitalWrite(giroSur, HIGH);estado_giroS=0;}
  }
  
  //==================================GIROS=============================================
/// función para seguir al sol
  void Seguir_sol(){
   
    //seguimiento acimutal 
         
  if (cenit_sol>cenit_max){cenit_sol=cenit_max;} else{cenit_sol=cenit_sol;}
  if (abs(acimut_sol)>acimut_max && acimut_sol>0){acimut_sol=acimut_max;} else{acimut_sol=acimut_sol;}
  if (abs(acimut_sol)>acimut_max && acimut_sol<0){acimut_sol=-acimut_max;} else{acimut_sol=acimut_sol;}
   
    grados_acimut=(acimut_sol-acimut_actual);
    if (abs(grados_acimut) > precision_acimut){
      grados_acimut=2*abs(grados_acimut); // se mueve el doble de lo necesario, la mitad de movimientos
      pulsos_acimut=grados_acimut/grados_pulso_acimut;
      
      if(acimut_sol>acimut_actual){
         gira_Oeste(1);
        
         if (abs(EncA.read())>pulsos_acimut){
            gira_Oeste(0);
            delay(1000);
            acimut_actual=acimut_actual+abs(EncA.read())*grados_pulso_acimut;  
            EncA.write(0);
          }
        }
        
      if(acimut_sol<acimut_actual){
         gira_Este(1);
         
         if (abs(EncA.read())>pulsos_acimut){
            gira_Este(0);
            delay(1000);
            acimut_actual=acimut_actual-abs(EncA.read())*grados_pulso_acimut;  
            EncA.write(0);
          }
        }
      
     }
   //seguimiento cenital
    
    grados_cenit=(cenit_sol-cenit_actual);
     if (abs(grados_cenit) > precision_cenit){
        grados_cenit=2*abs(grados_cenit); // se mueve el doble de lo necesario, la mitad de movimientos 
        pulsos_cenit=grados_cenit/grados_pulso_cenit;
        
        if (cenit_sol>cenit_actual) {
          gira_Sur(1);
          
          if (abs(EncZ.read())>pulsos_cenit){
            gira_Sur(0);
            delay(1000);
            cenit_actual=cenit_actual+abs(EncZ.read())*grados_pulso_cenit;  
            EncZ.write(0);
            }
          }
        if (cenit_sol<cenit_actual){
          gira_Norte(1);
         
          if (abs(EncZ.read())>pulsos_cenit){
            gira_Norte(0);
            delay(1000);
            cenit_actual=cenit_actual-abs(EncZ.read())*grados_pulso_cenit;  
            EncZ.write(0);
          }
        }
         
     }
} // fin de seguir al sol

/////////////////////////SERIAL READ COMMANDS //////////////////////////////////////////////////
/*  code to process time sync messages from the serial port   */

void procesaSerie(){
   char buf = '\0';
   
   buf = Serial.read();
    if (buf == 't' || buf == 'T'){
      while(Serial.available())
         buf = Serial.read();
          Serial.println( "________> TIME AND DATE SETTING <_______________" );
          Serial.println(F( " FORMAT  YYYY,MM,DD,HH,DD,SS,   DO NOT FORGET THE LAST ','" ));
          Serial.println(F( "Set the date and time by entering the current date/time in" ));
          Serial.println(F( "YYYY,MM,DD,HH,DD,SS," ));
          Serial.println(F("example: 2000,12,31,23,59,59, would be DEC 31st, 2000  11:59:59 pm") );
          
      // while(Serial.available())
       //  buf = Serial.read();
       setTimeFunction();
    }
  if (buf == 'M' || buf == 'm'){
      while(Serial.available())
         buf = Serial.read();
          Serial.println( F("_____________> MANUAL CONTROL <________________" ));
          Serial.println( F("Tracker in now in manual control, write E,O,N,S" ));
          Serial.println( F("i.e, E for East movement, ...." ));
          delay(2000);
          manual=true;
          //          setM();
    }
    if (buf == 'a' || buf == 'A'){
      while(Serial.available())
         buf = Serial.read();
          Serial.println( F("_____________> AUTOMATIC CONTROL <________________" ));
          Serial.println( F("Tracker in now in automatic control, write A for automatic" ));
          manual=false;
          manual_giraEste=false;
          manual_giraOeste=false;
          manual_giraNorte=false;
          manual_giraSur=false;
          gira_Este(0);
          gira_Oeste(0);
          gira_Norte(0);
          gira_Sur(0);
          delay(2000);
      }

    if (buf == 'e' || buf == 'E'){
      while(Serial.available())
         buf = Serial.read();
          Serial.println( F("___________> MANUAL CONTROL EAST turn <_______________________" ));
          manual=true;
          manual_giraEste=true;
          manual_giraOeste=false;
          delay(2000);
        }
        if (buf == 'o' || buf == 'O'){
      while(Serial.available())
         buf = Serial.read();
          Serial.println( F("___________> MANUAL CONTROL WEST turn <_______________________" ));
          manual=true;
          manual_giraEste=false;
          manual_giraOeste=true;
          delay(2000);
        }
        if (buf == 'n' || buf == 'N'){
      while(Serial.available())
         buf = Serial.read();
          Serial.println( F("___________> MANUAL CONTROL NORTH turn <_______________________" ));
          manual=true;
          manual_giraNorte=true;
          manual_giraSur=false;
          delay(2000);
        }
        if (buf == 's' || buf == 'S'){
      while(Serial.available())
         buf = Serial.read();
          Serial.println( F("___________> MANUAL CONTROL SOUTH turn <_______________________" ));
          manual=true;
          manual_giraNorte=false;
          manual_giraSur=true;
          delay(2000);
        }
    
}

void setTimeFunction(){
   delayTime = millis() + 10000UL;
  while (delayTime >= millis() && !Serial.available()) {
    delay(10);
  }
  if (Serial.available()) {
        //note that the tmElements_t Year member is an offset from 1970,
        //but the RTC wants the last two digits of the calendar year.
        //use the convenience macros from Time.h to do the conversions.
            int y = Serial.parseInt();
            tm.Year = CalendarYrToTm(y);
            tm.Month = Serial.parseInt();
            tm.Day = Serial.parseInt();
            tm.Hour = Serial.parseInt();
            tm.Minute = Serial.parseInt();
            tm.Second = Serial.parseInt();
            t = makeTime(tm);
     //use the time_t value to ensure correct weekday is set
            if (t != 0) {
                RTC.set(t);   // set the RTC and the system time to the received value
                setTime(t);          
              }
     else
       Serial.println(F("RTC set failed!") );

       Serial.println("RTC set OK!" );
       Serial.println(t);
       
       Serial.println(tm.Year);
       Serial.println(tm.Month);
       Serial.println(tm.Day);
       Serial.println(tm.Hour);
       Serial.println(tm.Minute);
       Serial.println(tm.Second);
            while (Serial.available() > 0) Serial.read();
  }
  else 
    Serial.println( F("timed out, please try again"));

    delay(1000);
}


void setM(){ //manual
    delayTime = millis() + 10000UL;
  while (delayTime >= millis() && !Serial.available()) {
    delay(10);
  }
  if (Serial.available()) {
            int y = Serial.parseInt();
             if (y>0 && y<1200) {
                //irradiancia_limite=y;  
                //EEPROM.put(0, irradiancia_limite);
                   }
     else
       Serial.println("Manual set failed!" );
       Serial.println("Manual set OK!" );
        while (Serial.available() > 0) Serial.read();
  }
  else 
    Serial.println( F("timed out"));
    delay(1000);
}

  
void display() {
 

  lcd.setCursor(0,0);
  lcd.print(hour());
  displayDigits(minute());
  displayDigits(second());
  lcd.print(" ");
  lcd.print(day());
  lcd.print(" ");
  lcd.print(month());
  lcd.print(" ");
  lcd.print(year()); 

  if (manual){
    display_manual(); 
  }else{
 
        if (displayTime >= millis() ) {
              //return 0;
        }else{
          displayTime = millis() + 5000UL;
          num_pant++;   if (num_pant>3){num_pant=0;}
          lcd.clear();}
        
        switch (num_pant) {
          case 0: display_angulosol();    break;
          case 1: display_anguloactual(); break;
          case 2: display_error();        break;
          case 3: display_FC();           break;
          default: display_angulosol();   break;
        }
  }
  int x;
  x = analogRead (0);
  //lcd.setCursor(10,1);
  if (x < 60) {
   // lcd.print ("Right ");
  }
  else if (x < 200) {
  //  lcd.print ("Up    ");
  }
  else if (x < 400){
    //lcd.print ("Down  ");
  }
  else if (x < 600){
    //lcd.print ("Left  ");
  }
  else if (x < 800){
    //lcd.print ("Select");
  }
  //lcd.setCursor(0,0);
  //lcd.print("valor A0 : ");
  //lcd.print(x);
}

void display_angulosol(){
  lcd.setCursor(0,1);
  lcd.print(F("dZ:"));
  lcd.print(sun.dZenithAngle);
  lcd.print(F("dA:"));
  lcd.print(sun.dAzimuth-180); 
}
void display_anguloactual(){
  lcd.setCursor(0,1);
  lcd.print(F("aZ:"));
  lcd.print(cenit_actual);
  lcd.print(F("aA:"));
  lcd.print(acimut_actual); 
}
void display_error(){
  lcd.setCursor(0,1);
  lcd.print(F("Error:"));
  lcd.print(error);
}
void display_FC(){
  lcd.setCursor(0,1);
  lcd.print(F("FCs:"));
  lcd.print(FC_este);
  lcd.print(F(":"));
  lcd.print(FC_oeste);
  lcd.print(F(":"));
  lcd.print(FC_norte);
  lcd.print(F(":"));
  lcd.print(FC_sur);
}

void display_manual(){
  lcd.setCursor(0,1);
  lcd.print(F("Manual:"));
  if (estado_giroE){ lcd.print(F("Este"));}
  if (estado_giroO){ lcd.print(F("Oeste"));}
  if (estado_giroN){ lcd.print(F("Norte"));}
  if (estado_giroS){ lcd.print(F("Sur"));}
  }

void displayDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  lcd.print(":");
  if(digits < 10)
   lcd.print('0');
   lcd.print(digits);
}

  ///lee los encoders //////////////////////////////////////////

//Serial.println("Reset both knobs to zero");
//    EncA.write(0);
//    Encz.write(0);
    
void lee_pulsosA() {
  PulsosA = EncA.read();
  if (PulsosA != PulsosA) {
    PulsosA = PulsosA;
    Serial.println(PulsosA);
  }
}

void lee_pulsosZ() {
  PulsosZ = EncZ.read();
  if (PulsosZ != PulsosZ) {
    PulsosZ = PulsosZ;
    Serial.println(PulsosZ);
  }
}


void printLine(int refreshSeconds){
  //Check if the current second since restart is a mod of refresh seconds , 
  //if it is then update the display , it must also not equal the previously 
  //stored value to prevent duplicate refreshes
  if((millis()/1000) % refreshSeconds == 0 && previous != (millis()/1000)){
    previous =  (millis()/1000);//Store the current time we entered for comparison on the next cycle
    lcd.setCursor(0, 1);//Set our draw position , set second param to 0 to use the top line
    char lcdTop[16];//Create a char array to store the text for the line
    int copySize = 16; // What is the size of our screen , this could probably be moved outside the loop but its more dynamic like this
    if(strlen(message) < 16)
    {
        //if the message is bigger than the current buffer use its length instead;
        copySize = strlen(message);
    }
    //Store the current position temporarily and invert its sign if its negative since we are going in reverse
    int tempPos = pos;
    if(tempPos < 0)
    {
        tempPos = -(tempPos);
    }
    //Build the lcd text by copying the required text out of our template message variable 
    memcpy(&lcdTop[0],&message[tempPos],copySize);
    lcd.print(lcdTop);//Print it from position 0
    //Increase the current position and check if the position + 16 (screen size) would be larger than the message length , if it is go in reverse by inverting the sign.
    pos += 1;
    if(pos +16 >= strlen(message))
    {
      pos = -(pos);
    }
  }
}

  

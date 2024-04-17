#include <Servo.h>

Servo dolne;
Servo gorne;
int serwo_dol_pin = 5;
int serwo_gora_pin = 6;
int serwodol;   //kat serwa dolnego
int serwogora;  //kat serwa gornego

//joystick
int joy_x = A4;
int joy_y = A5;
int guzik_joystick = 2; //bylo 4
int valueX;   //wartosci odczytane z joysticka
int valueY;


int buttonState = LOW;   // Aktualny stan przycisku
int lastButtonState = LOW; // Stan przycisku w poprzednim cyklu pętli
bool auto_mode = false;
unsigned long lastDebounceTime = 0;  // Czas od ostatniego przełączenia
unsigned long debounceDelay = 10;    // Czas opóźnienia debouncingu




//fotorezystory
int jeden = A0;
int dwa = A1;
int trzy = A2;
int cztery = A3;

//guzik kalibracja
int guzik = 13;
bool last_guzik = HIGH;

//LED
int led_tryb = 0;
int led_kalibracja = 1;

//Funkcja zwracajaca sume z fotorezystorow
int suma_swiatlo()
{
return (analogRead(jeden) + analogRead(dwa) + analogRead(trzy) + analogRead(cztery));
}


int roznica_gora() //roznica gora dol
{
  int gora = analogRead(jeden);
  int dol = analogRead(trzy);
 //     Serial.print("\n");
 // Serial.print("GORA0 DOL:  ");
 // Serial.print((gora - dol));
  return gora - dol; 
}


int roznica_dol() //roznica prawo lewo
{

int prawo = analogRead(dwa);
int lewo = analogRead(cztery);
 //   Serial.print("\n");
//  Serial.print("PRAWO LEWO:  ");
//  Serial.print((lewo - prawo));
return lewo - prawo;
}


void sterowanie_reczne()
{
  //Serial.print("\n");
  //Serial.print("MANUAL");

  valueX = analogRead(joy_x);  //przypisanie wartosci osi do zmiennych 
  valueY = analogRead(joy_y);
  
  valueX -=508;
  valueX = valueX / 150;

  valueY -=508;
  valueY = valueY / 150;


if (valueX > 4)
{
serwodol += (valueX + 3);
}
else if (valueX < -4)
{
  serwodol += (valueX - 3);
}
else
{
  serwodol += valueX;
}

serwodol += valueX;

if (serwodol >181)
{
serwodol = 180;
}
else if (serwodol <0)
{
serwodol = 0;
}


if (valueY > 4)
{
serwogora += (valueY + 3);
}
else if (valueY < -4)
{
  serwogora += (valueY - 3);
}
else
{
  serwogora += valueY;
}

serwogora += valueY;

if (serwogora >181)
{
serwogora = 180;
}
else if (serwogora <0)
{
serwogora = 0;
}
dolne.write(serwodol);
gorne.write(serwogora);
//delay(200);
}


void sterowanie_automat()
{
//OBSLUGA SWIATLO
//Serial.print("\n");
//Serial.print("AUTO");


if (roznica_gora() > 30)      //to w strone jedynki  ->góra-dół > 1 -> 1-3  
{
  serwogora =  serwogora - 2;
  //gorne.write(2);
  if (serwogora < 2)
  {
    serwogora = 0;
  }
}
else if(roznica_gora() < 10) 
{
  serwogora = serwogora + 2;
  if (serwogora > 178)
  {
  serwogora = 179;
  }
}



//jezeli dochodzi do granicy to
//góre daj na odwrót i dół obróc o 90 stopni 


//dolne pozycja_dol  pozycja_gora = 180 - pozycja_gora;

if (serwogora > 91) // to prawo jest na 2. 
{
  if (roznica_dol() > 10) // 10 to w strone jedynki
  {
    serwodol =  serwodol - 2;
    if (serwodol < -4)
    {
      //w przypadku gdy za bardzo sie obroci, tak aby sledził ruch do okoła
      serwogora = 180 - serwogora;
      serwodol = 90;   
    }
  }
  else if(roznica_dol() <-32) //-32
  {
    serwodol = serwodol + 2;
    if (serwodol > 184)
    {
    serwogora = 180 - serwogora;
    serwodol = 90;
    }
  }
}

else if (serwogora < 91) // to prawo jest na 4.
{

  if (roznica_dol()  < -32) //-32 to w strone jedynki
  {
    serwodol =  serwodol - 2;
    if (serwodol < -4)
    {
      serwogora = 180 - serwogora;
      serwodol = 90;
    }
  }
  else if(roznica_dol() > 10) // >10
  {
    serwodol = serwodol + 2;
    if (serwodol > 184)
    {
        serwogora = 180 - serwogora;
    serwodol = 90;
    }
  }
} 
gorne.write(serwogora);
dolne.write(serwodol);
}


void kalibracja_led()
{
  static unsigned long poprzedniCzas = 0;
  static bool stanDioda = false;
  
  unsigned long teraz = millis();
  
  if (teraz - poprzedniCzas >= 400) {
    poprzedniCzas = teraz;
    //stanDioda = !stanDioda;
    digitalWrite(led_kalibracja, stanDioda ? HIGH : LOW);
    stanDioda = !stanDioda;
    digitalWrite(led_tryb, stanDioda ? HIGH : LOW);
  }
}


void kalibracja()
{

gorne.write(0);
dolne.write(0);
  int pozycja = 0; //koncowa pozycja gornego serwa 
  int strona = 1;  //zmienna zeby isc od 0 do 180, od 180 do 0 ...

  int maksimum = 0 ; //max jasnosc
  int gora = 0; //pozycje
  int dol = 0 ;
for (int d = 0; d < 91; d += 10) //petla do dolnego serwa 
{
  for (int g = 0; g <180; g +=7)    //petla do gorne serwa
  {
    pozycja += 7 * strona; 
    gorne.write(pozycja); 
    if (suma_swiatlo() >maksimum  )   //sprawdzenie czy mocniej swieci
    {
      maksimum = suma_swiatlo();
      gora = pozycja;
      dol = d;
    }
kalibracja_led();
    delay(80);
  }
  strona = -strona;   //idz z poworotem 
  dolne.write(d);     //przesun dol
  delay(10);
}
serwodol = dol;     //zapisanie wyniku 
serwogora = gora;
gorne.write(serwogora);
dolne.write(serwodol);
digitalWrite(led_kalibracja, LOW);
}




void setup() {
dolne.attach(serwo_dol_pin);
gorne.attach(serwo_gora_pin);
serwodol = 0;
serwogora = 0;
dolne.write(80);
gorne.write(80);

valueX = 0 ;
valueY = 0 ;

pinMode(led_tryb, OUTPUT);
pinMode(led_kalibracja, OUTPUT);

pinMode(guzik_joystick,INPUT_PULLUP);
pinMode(guzik, INPUT);
kalibracja();

}

void loop() {
 int reading = digitalRead(guzik_joystick);

  // Sprawdzanie drgań i opóźnienie
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        // Naciśnięcie przycisku, zmiana stanu diody LED
        if (auto_mode) {
          digitalWrite(led_tryb, LOW);
          //sterowanie_reczne();
          auto_mode = false;
        } else {
          digitalWrite(led_tryb, HIGH);
          //sterowanie_automat();
          auto_mode = true;
        }
      }
    }
  }

  lastButtonState = reading;
 if(auto_mode == 1)
 {
 sterowanie_automat();
 }
 else if(auto_mode == 0)
{
 sterowanie_reczne();
  }


if (digitalRead(guzik)) {
kalibracja();
//digitalWrite(led_kalibracja, HIGH);

}
else
{
//digitalWrite(led_kalibracja,LOW);

}
  delay(50);

}

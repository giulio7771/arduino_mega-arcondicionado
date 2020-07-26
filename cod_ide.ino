#define BLYNK_PRINT Serial

 

#include <LiquidCrystal.h>
#include <UIPEthernet.h>
#include <BlynkSimpleUIPEthernet.h>
#include <DHT.h>

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "WTvZQCC2Bttcxa3QCzdcnTej3sW5Tdqv";

 

#define DHTPIN 4          // What digital pin we're connected to

 

// Uncomment whatever type you're using!
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

 

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

bool arLigado = false;
int arTemp = 25;

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  //Serial.print("V4 Slider value is: ");
  //Serial.println(pinValue);
  //digitalWrite(V2, pinValue);
  arTemp = pinValue;
  atualizaLcd();
}
BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  arLigado = pinValue;
  atualizaLcd();
}  


// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.

void atualizaLcd(){
  lcd.clear();
  lcd.print("Ar-condicionado: ");
  lcd.print(arTemp);
  lcd.print(" g");
  lcd.setCursor(0,1);
  lcd.print("status: ");
  if (arLigado) {
    lcd.print("Ligado");
  } else {
    lcd.print("Desligado");
  }
}

void arAutomatico() {
   float h = dht.readHumidity();
   float t = dht.readTemperature();
   if (t > 30 && !arLigado) {
      Blynk.notify("Ligando ar condicionado");
      arLigado = true;
      Blynk.virtualWrite(V2, 25);
      Blynk.virtualWrite(V3, true);
   } else if (t < 20 && arLigado) {
      Blynk.notify("Desligando ar condicionado");
      arLigado = false;
      Blynk.virtualWrite(V3, false);
   }
   atualizaLcd();
}

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  /*Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V1, analogRead(A0));
  Serial.println((analogRead(A1) * (5.0 / 1024.0))* 100.0);
  */

  //lcd.clear();
  lcd.setCursor(10, 1);
  
  arAutomatico();
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  
  lcd.begin(20, 2);
  lcd.clear();

  
  Blynk.begin(auth);
  // You can also specify server:
  //Blynk.begin(auth, "blynk-cloud.com", 80);
  //Blynk.begin(auth, IPAddress(192,168,1,100), 8080);

  Blynk.virtualWrite(V2, 25);
  Blynk.virtualWrite(V3, 0);

  dht.begin();
  atualizaLcd();

  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
}

 

void loop()
{
  Blynk.run();
  timer.run();
}

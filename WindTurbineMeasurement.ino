#include <Thread.h>
#include <LiquidCrystal.h>

Thread displayThread = Thread();
Thread updateThread = Thread();
Thread timerThread = Thread();

bool freeze = false;
bool firstRun = true;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

double values[30];
int pos = 0;
double maxV = 0, avg = 0;

void displayCallback()
{
  if(freeze) return;
  
  lcd.clear();
  lcd.print("Max: ");
  lcd.print(maxV, 0);
  lcd.print(" mV");
  lcd.setCursor(0, 1);
  lcd.print("Avg: ");
  lcd.print(avg, 0);
  lcd.print(" mV");
}

void updateAverage()
{
  values[pos] = analogRead(A0);
  delay(5);

  values[pos] = values[pos] * (5 / 1.024);

  if(values[pos] > maxV) maxV = values[pos];
  avg = 0;
  for(int i = 0; i < (firstRun ? pos : 30); i++)
  {
    avg += values[i];
  }

  avg /= (firstRun ? pos : 30);

  if(avg > 50) timerThread.enabled = true;

  pos = (pos + 1) % 30;

  if(firstRun && pos == 0)
    firstRun = false;
}

void stopTheGame()
{
  freeze = true;
  updateThread.enabled = false;
}

void setup() 
{
  lcd.begin(16,2);
  lcd.print("Getting Ready.");

  displayThread.onRun(displayCallback);
  displayThread.setInterval(500);
  
  updateThread.onRun(updateAverage);
  updateThread.setInterval(100);

  timerThread.onRun(stopTheGame);
  timerThread.setInterval(30000);
  timerThread.enabled = false;
}

void loop() {
  if(updateThread.shouldRun())
    updateThread.run();
  if(displayThread.shouldRun())
    displayThread.run();

  if(timerThread.shouldRun())
    timerThread.run();
}

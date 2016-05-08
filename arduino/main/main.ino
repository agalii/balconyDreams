
/*
missing: 
* plausibility check to avoid pumping if sensor is broken
*/


// define addresses 
const int numSensors            = 3;
const int SensorPin[numSensors] = {A0, A1, A2}; // red, blue, yellow
const int SensorCtrl            = 6;            // white (one for all) 
const int PumpPin               = 2;            // green
const int ValvePin[numSensors]  = {3, 4, 5};    // red, blue, yellow
const int PiezoPin              = A3;           // potentiometer

const int Threshold             = 25;
const int Pumping               = 10;
const int TimeLag               = 10;
const int MeasInt               = 800;
const int Empty                 = 10;
const int On                    = 1;
const int Off                   = 0;

// states
const int CheckSMstate  = 0;
const int AddWaterState = 1;

int pumpCount[numSensors]  = {0, 0, 0};   // for sensor specific pump duration count
int lagCount[numSensors]   = {0, 0, 0};   // for sensor specific delay between pumping
int smAll[3];                    // soil moisture of all sensors
int pressure;
int state = CheckSMstate;
int waterNeed[numSensors]   = {0, 0, 0};


void setup() {
  Serial.begin(9600);
  
  pinMode(PiezoPin, INPUT);      
  pinMode(SensorCtrl, OUTPUT);      
  pinMode(PumpPin, OUTPUT);      

  digitalWrite(PumpPin, Off);

  for (int i = 0; i < numSensors; i++) {
    pinMode(SensorPin[i], INPUT);      
    pinMode(ValvePin[i], OUTPUT);
    digitalWrite(ValvePin[i], Off);
  }
}

void sendValues(int sens, int sm, int lag, int p, int pump, int valve, int pumpLag) {
    Serial.write("Sensor: ");
    Serial.print(sens);
    Serial.write(", sm: ");
    Serial.print(sm, DEC);
    Serial.write(", lag: ");
    Serial.print(lag, DEC);
    Serial.write(", pressure: ");
    Serial.print(p, DEC);
    Serial.write(", pump: ");
    Serial.print(pump);
    Serial.write(", valve: ");
    Serial.print(valve);
    Serial.write(", pump duration: ");
    Serial.print(pumpLag);
    Serial.write("\n");   
}

int checkSM(int pin) {
  int value  = analogRead(pin);
  int smValue = map(value, 0, 1023, 100, 0);
  
  digitalWrite(SensorCtrl, HIGH);
  delay(200);
  digitalWrite(SensorCtrl, LOW);
  delay(MeasInt);
  
  return(smValue);
}

int checkP(int pin) {
  int value  = analogRead(pin);
  int pValue = map(value, 0, 1023, 0, 100);
    return(pValue);
}


void loop() {
  
  switch (state) {
    case CheckSMstate: 
     
      for (int i = 0; i < numSensors; i++) {
        pressure = checkP(PiezoPin);
        smAll[i] = checkSM(SensorPin[i]);

        sendValues(i, smAll[i], lagCount[i], pressure, 0, 0, pumpCount[i]);
                
        if (smAll[i] < Threshold && lagCount[i] > TimeLag && pressure > Empty) {
          waterNeed[i] = 1;
          state = AddWaterState;
          break;
        }
        
        lagCount[i]++;
      }
      break;

    case AddWaterState:

      for (int i = 0; i < numSensors; i++) {
        pressure = checkP(PiezoPin);
        smAll[i] = checkSM(SensorPin[i]);
        
        if (waterNeed[i]) {
          digitalWrite(ValvePin[i], On);
          digitalWrite(PumpPin, On);
          pumpCount[i]++; 
        }
        
        sendValues(i, smAll[i], lagCount[i], pressure, 1, waterNeed[i], pumpCount[i]);
                
        if (pumpCount[i] > Pumping || pressure < Empty) {
          digitalWrite(ValvePin[i], Off);
          digitalWrite(PumpPin, Off);
          pumpCount[i] = 0;
          lagCount[i]  = 0;
          waterNeed[i] = 0;
          state = CheckSMstate; 
          break;
        }
        lagCount[i]++;
      }
      break;
  }
}





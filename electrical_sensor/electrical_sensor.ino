unsigned int waveHolderValue[20];
unsigned int maxValueHolder[50];
byte frequencyStep = 0;
byte roll = 0;
byte sensorCode = 0;
const byte totalDataSample[4] = {14, 14, 14, 14};
const unsigned int lowestSampleValue[4] = {102, 102, 102, 102};
const unsigned int highestSampleValue[4] = {185, 185, 185, 185};
const byte frequency = 50;
const byte totalWaveValue = 20;
float acVoltageSampleData[14][2] = {{102, 0}, {111, 0.17}, {112, 0.18}, {130, 0.65}, {131, 0.73}, {135, 0.83}, {140, 0.95}, {145, 1.08}, {150, 1.23}, {156, 1.4}, {163, 1.58}, {170, 1.77}, 
{177, 1.96}, {185, 2.15}};
float acCurrentSampleData[14][2] = {{102, 0}, {111, 0.17}, {112, 0.18}, {130, 0.65}, {131, 0.73}, {135, 0.83}, {140, 0.95}, {145, 1.08}, {150, 1.23}, {156, 1.4}, {163, 1.58}, {170, 1.77}, 
{177, 1.96}, {185, 2.15}};
float dcVoltageSampleData[14][2] = {{102, 0}, {111, 0.17}, {112, 0.18}, {130, 0.65}, {131, 0.73}, {135, 0.83}, {140, 0.95}, {145, 1.08}, {150, 1.23}, {156, 1.4}, {163, 1.58}, {170, 1.77}, 
{177, 1.96}, {185, 2.15}};
float dcCurrentSampleData[14][2] = {{102, 0}, {111, 0.17}, {112, 0.18}, {130, 0.65}, {131, 0.73}, {135, 0.83}, {140, 0.95}, {145, 1.08}, {150, 1.23}, {156, 1.4}, {163, 1.58}, {170, 1.77}, 
{177, 1.96}, {185, 2.15}};
float linearRegressionValue[2];
float powerSource[4];

void setup() {  
  Serial.begin(9600);
  Serial.println("System initialization ready");
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);      
  Serial.println("Initialization done & program ready to run");

}

void loop() {
  unsigned int ADCInputValue;
  switch(sensorCode) {
    case 0: ADCInputValue = analogRead(A0); break;
    case 1: ADCInputValue = analogRead(A1); break;
    case 2: ADCInputValue = analogRead(A2); break;
    case 3: ADCInputValue = analogRead(A3); break;
  }
  

  if(roll < totalWaveValue) {
    waveHolderValue[roll] = ADCInputValue;
    roll++;
    
  } else {
    roll = 0;
    
    if(frequencyStep < frequency) {
      maxValueHolder[frequencyStep] = getMaxValue();      
      frequencyStep++;
      
    } else {
      frequencyStep = 0;

      switch(sensorCode) {
         case 0: showValue("Nilai input ADC AC Voltage adalah : ", "Nilai tegangan AC adalah : "); break;
         case 1: showValue("Nilai input ADC AC Current adalah : ", "Nilai arus AC adalah : "); break;
         case 2: showValue("Nilai input ADC DC Voltage adalah : ", "Nilai tegangan DC adalah : "); break;
         case 3: showValue("Nilai input ADC DC Current adalah : ", "Nilai arus DC adalah : "); break; 
      }
      
      
    }  
  }
  
  delayMicroseconds(1000);
}

void showValue(String text1, String text2) {
    unsigned int adcFinalValue = getAverage();
    Serial.print(text1);
    Serial.println(adcFinalValue);      
    float finalValue = getFinalValue(adcFinalValue);     
    Serial.print(text2);
    Serial.println(finalValue);           
    
    switch(sensorCode) {
      case 0: sensorCode = 1; powerSource[0] = finalValue; break;
      case 1: sensorCode = 2; powerSource[1] = finalValue; break;
      case 2: sensorCode = 3; powerSource[2] = finalValue; break;
      case 3: sensorCode = 0; powerSource[3] = finalValue; break;
    }

    if(sensorCode == 0) {
      float valuePower;
      valuePower = powerSource[0] * powerSource[1];
      Serial.print("Nilai daya AC adalah : ");
      Serial.println(valuePower);
      valuePower = powerSource[2] * powerSource[3];
      Serial.print("Nilai daya DC adalah : ");
      Serial.println(valuePower);
      Serial.println();
      
    }
}

unsigned int getMaxValue() {
  
  unsigned int maxValue = 0;
  for(byte i = 0; i < totalWaveValue; i++) {
    
    if(maxValue < waveHolderValue[i]) {
      maxValue = waveHolderValue[i];
      
    }
  }

  return maxValue;
}

// get average value from all wave highest value
unsigned int getAverage() {
  
    unsigned int valueHolder = 0;
    for (byte i = 0; i < frequency; i++) {
      
      valueHolder += maxValueHolder[i];
    }

    valueHolder = valueHolder / frequency;

    return valueHolder;
}

// Linear regression initialization
void linearRegressionInit() {
  unsigned int sigmaX;
  unsigned int sigmaY;
  unsigned int sigmaSquareX;
  unsigned int sigmaXY;
  byte mTotalDataSample;

  switch(sensorCode) {
    case 0: mTotalDataSample = totalDataSample[0]; break;
    case 1: mTotalDataSample = totalDataSample[1]; break;
    case 2: mTotalDataSample = totalDataSample[2]; break;
    case 3: mTotalDataSample = totalDataSample[3]; break;
  }

  for(unsigned int i = 0; i < mTotalDataSample; i++) {
    unsigned int xValue;
    unsigned int yValue;
    switch(sensorCode) {
      case 0: xValue = acVoltageSampleData[i][0]; yValue = acVoltageSampleData[i][1]; break;
      case 1: xValue = acCurrentSampleData[i][0]; yValue = acCurrentSampleData[i][1]; break;
      case 2: xValue = dcVoltageSampleData[i][0]; yValue = dcVoltageSampleData[i][1]; break;
      case 3: xValue = dcCurrentSampleData[i][0]; yValue = dcCurrentSampleData[i][1]; break;
    }
    unsigned int xSquare = sq(xValue);
    unsigned int xy = xValue * yValue;

    sigmaX += xValue;
    sigmaY += yValue;
    sigmaSquareX += xSquare;
    sigmaXY += xy;
    
  }

  linearRegressionValue[0] = ((sigmaY * sigmaSquareX) - (sigmaX * sigmaXY)) / ((mTotalDataSample * sigmaSquareX) - sq(sigmaX));
  linearRegressionValue[1] = ((mTotalDataSample * sigmaXY) - (sigmaX * sigmaY)) / ((mTotalDataSample * sigmaSquareX) - sq(sigmaX));
  
}

// Final process to get final output value from ADC Input
float getFinalValue(unsigned int adcFinalValue) {
  float voltageValue;
  unsigned int lowestADCInputSample;
  unsigned int highestADCInputSample;
  byte mTotalDataSample;

  switch(sensorCode) {
    case 0: lowestADCInputSample = lowestSampleValue[0]; highestADCInputSample = highestSampleValue[0]; mTotalDataSample = totalDataSample[0]; break;
    case 1: lowestADCInputSample = lowestSampleValue[1]; highestADCInputSample = highestSampleValue[1]; mTotalDataSample = totalDataSample[1]; break;
    case 2: lowestADCInputSample = lowestSampleValue[2]; highestADCInputSample = highestSampleValue[2]; mTotalDataSample = totalDataSample[2]; break;
    case 3: lowestADCInputSample = lowestSampleValue[3]; highestADCInputSample = highestSampleValue[3]; mTotalDataSample = totalDataSample[3]; break;
  }

  if(adcFinalValue < lowestADCInputSample) {

    voltageValue = linearRegressionValue[0] + (linearRegressionValue[1] * ((lowestADCInputSample * 2) - adcFinalValue));
    voltageValue = voltageValue * -1;  
    
  } else if(adcFinalValue > highestADCInputSample) {
    linearRegressionInit();
    voltageValue = linearRegressionValue[0] + (linearRegressionValue[1] * adcFinalValue);
    
  } else if(adcFinalValue == lowestADCInputSample) {   

      voltageValue = 0;
    
  } else {

       for(unsigned int i = 0; i < mTotalDataSample; i++) {
          if(sensorCode == 0) {
              if((adcFinalValue > acVoltageSampleData[i][0]) && (adcFinalValue <= acVoltageSampleData[i+1][0])) {
                    voltageValue = ((adcFinalValue - acVoltageSampleData[i][0]) * ((acVoltageSampleData[i+1][1] - acVoltageSampleData[i][1]) / 
                    (acVoltageSampleData[i+1][0] - acVoltageSampleData[i][0]))) + acVoltageSampleData[i][1];        
                    break;        
        
              }  
          } else if(sensorCode == 1) {
              if((adcFinalValue > acCurrentSampleData[i][0]) && (adcFinalValue <= acCurrentSampleData[i+1][0])) {
                    voltageValue = ((adcFinalValue - acCurrentSampleData[i][0]) * ((acCurrentSampleData[i+1][1] - acCurrentSampleData[i][1]) / 
                    (acCurrentSampleData[i+1][0] - acCurrentSampleData[i][0]))) + acCurrentSampleData[i][1];        
                    break;        
        
              }
          } else if(sensorCode == 2) {
              if((adcFinalValue > dcVoltageSampleData[i][0]) && (adcFinalValue <= dcVoltageSampleData[i+1][0])) {
                    voltageValue = ((adcFinalValue - dcVoltageSampleData[i][0]) * ((dcVoltageSampleData[i+1][1] - dcVoltageSampleData[i][1]) / 
                    (dcVoltageSampleData[i+1][0] - dcVoltageSampleData[i][0]))) + dcVoltageSampleData[i][1];        
                    break;        
        
              }
          } else {
              if((adcFinalValue > dcCurrentSampleData[i][0]) && (adcFinalValue <= dcCurrentSampleData[i+1][0])) {
                    voltageValue = ((adcFinalValue - dcCurrentSampleData[i][0]) * ((dcCurrentSampleData[i+1][1] - dcCurrentSampleData[i][1]) / 
                    (dcCurrentSampleData[i+1][0] - dcCurrentSampleData[i][0]))) + dcCurrentSampleData[i][1];        
                    break;        
        
              }
          }           
      
       }
  }

  return voltageValue;
  
}

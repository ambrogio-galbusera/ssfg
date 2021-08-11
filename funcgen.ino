/*
 * StatusMonitor
 *
 * Displays formatted, dynamic, real-time Arduino status information
 * using BasicTerm.
 *
 * It responds to the following input commands:
 * ' '  (space):  Toggle LED
 * '\f' (Ctrl-L): Redraw screen
 */

#include <BasicTerm.h>
#include <SAMD21turboPWM.h>
#include "VTForm.h"
#include "VTLabel.h"
#include "VTOption.h"
#include "VTMenuItem.h"
#include "VTInput.h"
#include "VTCombo.h"

#define FNC_NONE      0
#define FNC_SQUARE    1
#define FNC_SIN       2
#define FNC_TRIANGLE  3
#define FNC_SAWTOOTH  4
#define FNC_USERDEF   5
#define PINS_NUM      5
#define USERDEF_STEPS_MAX 8
#define FUNCTIONS_MAX     PINS_NUM

#define STATUS_ROW  18
#define DIGITAL_ROW 20
#define ANALOG_ROW  22
#define LED 13

typedef struct 
{
    int level;
    int duration;
} USERDEF_STEP;

typedef struct
{
    int fnc;
    int freq;
    int mod;
    int numSteps;
    USERDEF_STEP steps[USERDEF_STEPS_MAX];
} FUNCTION;

typedef struct
{
    long usStep;
    long usPeriod;
    int index;
    int value;
} FUNCTION_STATUS;

void onWaveConfirmed(VTEditItem*);
void onFrequencyConfirmed(VTEditItem*);
void onLevelConfirmed(VTEditItem*);
void onDurationConfirmed(VTEditItem*);
void onUserDefOption(VTItem* item);
void onRunOption(VTItem*);
void onConfirmOption(VTItem*);

int pin = 0;
int waveform = 0;
int frequency = 1000;

const int SIN_TABLE[] = {
  0x80,0x86,0x8c,0x92,0x98,0x9e,0xa5,0xaa,
  0xb0,0xb6,0xbc,0xc1,0xc6,0xcb,0xd0,0xd5,
  0xda,0xde,0xe2,0xe6,0xea,0xed,0xf0,0xf3,
  0xf5,0xf8,0xfa,0xfb,0xfd,0xfe,0xfe,0xff,
  0xff,0xff,0xfe,0xfe,0xfd,0xfb,0xfa,0xf8,
  0xf5,0xf3,0xf0,0xed,0xea,0xe6,0xe2,0xde,
  0xda,0xd5,0xd0,0xcb,0xc6,0xc1,0xbc,0xb6,
  0xb0,0xaa,0xa5,0x9e,0x98,0x92,0x8c,0x86,
  0x80,0x79,0x73,0x6d,0x67,0x61,0x5a,0x55,
  0x4f,0x49,0x43,0x3e,0x39,0x34,0x2f,0x2a,
  0x25,0x21,0x1d,0x19,0x15,0x12,0xf,0xc,
  0xa,0x7,0x5,0x4,0x2,0x1,0x1,0x0,
  0x0,0x0,0x1,0x1,0x2,0x4,0x5,0x7,
  0xa,0xc,0xf,0x12,0x15,0x19,0x1d,0x21,
  0x25,0x2a,0x2f,0x34,0x39,0x3e,0x43,0x49,
  0x4f,0x55,0x5a,0x61,0x67,0x6d,0x73,0x79
};

const int TRIANGLE_TABLE[] = {
  0x4,0x8,0xc,0x10,0x14,0x18,0x1c,0x20,
  0x24,0x28,0x2c,0x30,0x34,0x38,0x3c,0x40,
  0x44,0x48,0x4c,0x50,0x54,0x58,0x5c,0x60,
  0x64,0x68,0x6c,0x70,0x74,0x78,0x7c,0x80,
  0x83,0x87,0x8b,0x8f,0x93,0x97,0x9b,0x9f,
  0xa3,0xa7,0xab,0xaf,0xb3,0xb7,0xbb,0xbf,
  0xc3,0xc7,0xcb,0xcf,0xd3,0xd7,0xdb,0xdf,
  0xe3,0xe7,0xeb,0xef,0xf3,0xf7,0xfb,0xff,
  0xfb,0xf7,0xf3,0xef,0xeb,0xe7,0xe3,0xdf,
  0xdb,0xd7,0xd3,0xcf,0xcb,0xc7,0xc3,0xbf,
  0xbb,0xb7,0xb3,0xaf,0xab,0xa7,0xa3,0x9f,
  0x9b,0x97,0x93,0x8f,0x8b,0x87,0x83,0x80,
  0x7c,0x78,0x74,0x70,0x6c,0x68,0x64,0x60,
  0x5c,0x58,0x54,0x50,0x4c,0x48,0x44,0x40,
  0x3c,0x38,0x34,0x30,0x2c,0x28,0x24,0x20,
  0x1c,0x18,0x14,0x10,0xc,0x8,0x4,0x0,
};

const int SAWTOOTH_TABLE[] = {
  0x2,0x4,0x6,0x8,0xa,0xc,0xe,0x10,
  0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,0x20,
  0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,
  0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,0x40,
  0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,
  0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,0x60,
  0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,
  0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,0x80,
  0x81,0x83,0x85,0x87,0x89,0x8b,0x8d,0x8f,
  0x91,0x93,0x95,0x97,0x99,0x9b,0x9d,0x9f,
  0xa1,0xa3,0xa5,0xa7,0xa9,0xab,0xad,0xaf,
  0xb1,0xb3,0xb5,0xb7,0xb9,0xbb,0xbd,0xbf,
  0xc1,0xc3,0xc5,0xc7,0xc9,0xcb,0xcd,0xcf,
  0xd1,0xd3,0xd5,0xd7,0xd9,0xdb,0xdd,0xdf,
  0xe1,0xe3,0xe5,0xe7,0xe9,0xeb,0xed,0xef,
  0xf1,0xf3,0xf5,0xf7,0xf9,0xfb,0xfd,0xff,
};

int INDEX_TO_PIN[PINS_NUM] = { 3, 5, 6, 9, 10 };

String PINS[] = {
  "3", "5", "6", "9", "10"
};

String WAVEFORMS[] = {
  "NONE",
  "SQUARE",
  "SIN",
  "TRIANGLE",
  "SAW",
  "USERDEF"
};

String LEVELS[] = {
  "NONE",
  "LOW",
  "HIGH"
};

BasicTerm term(&Serial);
TurboPWM pwm;
FUNCTION functions[FUNCTIONS_MAX];
FUNCTION_STATUS status[FUNCTIONS_MAX];

VTItem* mainItems[] = {
  new VTLabel("PIN 3", 5, 5, term),
  new VTLabel("Wave", 5, 15, term),
  new VTCombo(10, 5, 21, term, 10, WAVEFORMS, 6, &functions[0].fnc, onWaveConfirmed),
  new VTLabel("Freq.", 5, 35, term),
  new VTInput(11, 5, 41, term, 5, 0, 10000, &functions[0].freq, onFrequencyConfirmed),
  new VTMenuItem(12, "UserDef", 5, 55, term, onUserDefOption),

  new VTLabel("PIN 5", 6, 5, term),
  new VTLabel("Wave", 6, 15, term),
  new VTCombo(20, 6, 21, term, 10, WAVEFORMS, 6, &functions[1].fnc, onWaveConfirmed),
  new VTLabel("Freq.", 6, 35, term),
  new VTInput(21, 6, 41, term, 5, 0, 10000, &functions[1].freq, onFrequencyConfirmed),
  new VTMenuItem(22, "UserDef", 6, 55, term, onUserDefOption),

  new VTLabel("PIN 6", 7, 5, term),
  new VTLabel("Wave", 7, 15, term),
  new VTCombo(30, 7, 21, term, 10, WAVEFORMS, 6, &functions[2].fnc, onWaveConfirmed),
  new VTLabel("Freq.", 7, 35, term),
  new VTInput(31, 7, 41, term, 5, 0, 10000, &functions[2].freq, onFrequencyConfirmed),
  new VTMenuItem(32, "UserDef", 7, 55, term, onUserDefOption),

  new VTLabel("PIN 9", 8, 5, term),
  new VTLabel("Wave", 8, 15, term),
  new VTCombo(40, 8, 21, term, 10, WAVEFORMS, 6, &functions[3].fnc, onWaveConfirmed),
  new VTLabel("Freq.", 8, 35, term),
  new VTInput(41, 8, 41, term, 5, 0, 10000, &functions[3].freq, onFrequencyConfirmed),
  new VTMenuItem(42, "UserDef", 8, 55, term, onUserDefOption),

  new VTLabel("PIN 10", 9, 5, term),
  new VTLabel("Wave", 9, 15, term),
  new VTCombo(50, 9, 21, term, 10, WAVEFORMS, 6, &functions[4].fnc, onWaveConfirmed),
  new VTLabel("Freq.", 9, 35, term),
  new VTInput(51, 9, 41, term, 5, 0, 10000, &functions[4].freq, onFrequencyConfirmed),
  new VTMenuItem(52, "UserDef", 9, 55, term, onUserDefOption),

  new VTMenuItem(3, "&Run / Stop", 11, 35, term, onRunOption)
};
VTForm mainForm("SIMPLE FUNCTION GENERATOR", 90, 25, term, mainItems, 31);

VTItem* userDefItems[] = {
  new VTLabel("STEP 1", 5, 5, term),
  new VTLabel("Level", 5, 15, term),
  new VTCombo(100, 5, 21, term, 10, LEVELS, 3, onLevelConfirmed),
  new VTLabel("Dur. (ms)", 5, 35, term),
  new VTInput(101, 5, 45, term, 10, 0, 10000, onDurationConfirmed),

  new VTLabel("STEP 2", 6, 5, term),
  new VTLabel("Level", 6, 15, term),
  new VTCombo(200, 6, 21, term, 10, LEVELS, 3, onLevelConfirmed),
  new VTLabel("Dur. (ms)", 6, 35, term),
  new VTInput(201, 6, 45, term, 10, 0, 10000, onDurationConfirmed),

  new VTLabel("STEP 3", 7, 5, term),
  new VTLabel("Level", 7, 15, term),
  new VTCombo(300, 7, 21, term, 10, LEVELS, 3, onLevelConfirmed),
  new VTLabel("Dur. (ms)", 7, 35, term),
  new VTInput(301, 7, 45, term, 10, 0, 10000, onDurationConfirmed),

  new VTLabel("STEP 4", 8, 5, term),
  new VTLabel("Level", 8, 15, term),
  new VTCombo(400, 8, 21, term, 10, LEVELS, 3, onLevelConfirmed),
  new VTLabel("Dur. (ms)", 8, 35, term),
  new VTInput(401, 8, 45, term, 10, 0, 10000, onDurationConfirmed),

  new VTLabel("STEP 5", 9, 5, term),
  new VTLabel("Level", 9, 15, term),
  new VTCombo(500, 9, 21, term, 10, LEVELS, 3, onLevelConfirmed),
  new VTLabel("Dur. (ms)", 9, 35, term),
  new VTInput(501, 9, 45, term, 10, 0, 10000, onDurationConfirmed),

  new VTLabel("STEP 6", 10, 5, term),
  new VTLabel("Level", 10, 15, term),
  new VTCombo(600, 10, 21, term, 10, LEVELS, 3, onLevelConfirmed),
  new VTLabel("Dur. (ms)", 10, 35, term),
  new VTInput(601, 10, 45, term, 10, 0, 10000, onDurationConfirmed),

  new VTLabel("STEP 7", 11, 5, term),
  new VTLabel("Level", 11, 15, term),
  new VTCombo(700, 11, 21, term, 10, LEVELS, 3, onLevelConfirmed),
  new VTLabel("Dur. (ms)", 11, 35, term),
  new VTInput(701, 11, 45, term, 10, 0, 10000, onDurationConfirmed),

  new VTLabel("STEP 8", 12, 5, term),
  new VTLabel("Level", 12, 15, term),
  new VTCombo(800, 12, 21, term, 10, LEVELS, 3, onLevelConfirmed),
  new VTLabel("Dur. (ms)", 12, 35, term),
  new VTInput(801, 12, 45, term, 10, 0, 10000, onDurationConfirmed),

  new VTMenuItem(3, "&Confirm", 14, 35, term, onConfirmOption)
};
VTForm userDefForm("USER-DEFINED WAVEFORM", 90, 25, term, userDefItems, 41);

VTForm* currForm = &mainForm;

bool running = false;
long runStart;
uint32_t last;
uint32_t now;

void onUserDefOption(VTItem* item)
{
  int pinIdx = (item->id() / 10) - 1;

  for (int i=0; i<USERDEF_STEPS_MAX; i++)
  {
    ((VTEditItem*)userDefItems[(i*5)+2])->setValuePtr(&functions[pinIdx].steps[i].level);
    ((VTEditItem*)userDefItems[(i*5)+4])->setValuePtr(&functions[pinIdx].steps[i].duration);
  }

  currForm = &userDefForm;
  currForm->draw();
}

void onWaveConfirmed(VTEditItem* item)
{
}

void onFrequencyConfirmed(VTEditItem* item)
{
}

void onLevelConfirmed(VTEditItem* item)
{
}

void onDurationConfirmed(VTEditItem* item)
{
}

void onRunOption(VTItem* item)
{
  running = !running;
  runStart = micros();
  now = millis();

  initFunctionStatus(); 

  digitalWrite(LED, running? HIGH : LOW);
}

void onConfirmOption(VTItem* item)
{
  currForm = &mainForm;
  currForm->draw();
}

void initFunctionStatus()
{  
  bool pwmInited = false;
  
  for (int i=0; i<PINS_NUM; i++)
  {
    FUNCTION* pf = &functions[i];
    FUNCTION_STATUS* ps = &status[i];
    if ((pf->fnc == FNC_SIN) || (pf->fnc == FNC_TRIANGLE) || (pf->fnc == FNC_SAWTOOTH))
    {
      if (! pwmInited)
      {
        pwm.setClockDivider(1, false);
        pwm.timer(0, 1, 750, false);
        pwm.enable(0, true);
        
        pwmInited = true;
      }
      
      ps->usPeriod = 1000000 / pf->freq;
      ps->usStep = ps->usPeriod >> 7;  
      ps->index = 0;
    }    
    else if (pf->fnc == FNC_SQUARE)
    {
      ps->usPeriod = 1000000 / pf->freq;
      ps->usStep = ps->usPeriod / 2;  
    }    
    else if (pf->fnc == FNC_USERDEF)
    {
      ps->usPeriod = 0;
      for (int i=0; i<USERDEF_STEPS_MAX; i++)
      {
        USERDEF_STEP* step = &pf->steps[i];
        if (step->level == 0)
          continue;

        ps->usPeriod += (step->duration * 1000);
      }
    }    
  }
}

void runUserDefFunction(int pin, FUNCTION* pf, FUNCTION_STATUS* ps)
{
  long us = micros();  
  long usDelta = us - runStart;

  long usFromStart = usDelta % ps->usPeriod;
  long usTotal = 0;
  us = 0;
  for (int i=0; i<USERDEF_STEPS_MAX; i++)
  {
    USERDEF_STEP* step = &pf->steps[i];
    if (step->level == 0)
      continue;
     
    us = step->duration;
    us = us * 1000;
    usTotal += us;

    if (usFromStart < usTotal)
    {
      if (ps->value != step->level-1)
      {
        digitalWrite(pin, (step->level == 1)? LOW : HIGH);
        ps->value = step->level-1;
      }
      
      break;
    }
  }
}

void runTableFunction(int pin, FUNCTION* pf, FUNCTION_STATUS* ps, const int* table)
{
  // get the current microseconds since start
  long us = micros();  
  
  // how many microseconds since the run command?
  long usDelta = us - runStart;

  // usStep is the number of microseconds for each of the 128 steps in the
  // lookup table. usStep is calculated as the waveform period divided by 128
  long lIndex = usDelta / ps->usStep;

  // get the index to the lookup table
  int index = (int)(lIndex & 0x7F);

  // get value from table
  int value = table[index];

  // write to PWM
  if (value != ps->value)
  {
    int ivalue = value * 1000 / 255;
    pwm.analogWrite(pin, ivalue);
    ps->value = value;
    ps->index = index;
  }
}

void runSquareFunction(int pin, FUNCTION* pf, FUNCTION_STATUS* ps)
{
  // get the current microseconds since start
  long us = micros();  

  // how many microseconds since the run command?
  long usDelta = us - runStart;

  // usStep is the number of microseconds for each of the 128 steps in the
  // lookup table. usStep is calculated as the waveform period divided by 128
  long lIndex = usDelta / ps->usStep;

  // get the index to the lookup table
  int value = (int)(lIndex % 2);

  digitalWrite(pin, value);
}

void runFunctions()
{
  for (int i=0; i<PINS_NUM; i++)
  {
    FUNCTION* pf = &functions[i];  
    FUNCTION_STATUS* ps = &status[i];

    if (pf->fnc == FNC_NONE)
      continue;

    int pin = INDEX_TO_PIN[i];

    if (pf->fnc == FNC_SIN)
      runTableFunction(pin, pf, ps, SIN_TABLE);
    else if (pf->fnc == FNC_TRIANGLE)
      runTableFunction(pin, pf, ps, TRIANGLE_TABLE);
    else if (pf->fnc == FNC_SAWTOOTH)
      runTableFunction(pin, pf, ps, SAWTOOTH_TABLE);
    else if (pf->fnc == FNC_SQUARE)
      runSquareFunction(pin, pf, ps);
    else if (pf->fnc == FNC_USERDEF)
      runUserDefFunction(pin, pf, ps);
  }
}

void printStatus()
{
  int i;
  
  now  = millis();

  if (now - last < 1000)
    return;

  last = now;

  term.position(STATUS_ROW, 1);
  term.set_attribute(BT_NORMAL);
  term.print(F("Current millis: "));
  term.print(now);
  term.print(F("\tmillis since last loop: "));
  term.set_attribute(BT_UNDERLINE);
  term.print(now - last);
  term.set_attribute(BT_NORMAL);

  term.print(F("\tStatus: "));
  if (running)
  {
    term.set_attribute(BT_BOLD);
    term.print("RUN ");
  }
  else
    term.print("STOP");
  term.set_attribute(BT_NORMAL);

  term.position(STATUS_ROW+1, 1);
  term.print(F("LED "));
  term.set_color(BT_GREEN, BT_BLACK);
  if(digitalRead(LED) == HIGH) {
    term.set_attribute(BT_BOLD | BT_UNDERLINE);
    term.print(F("ON "));
  } else {
    term.print(F("OFF"));
  }
  term.set_attribute(BT_NORMAL);

  for(i = 0; i <= 13; i++) {
    term.position(DIGITAL_ROW, (i)*6 + 1);
    term.set_attribute(BT_NORMAL);
    term.print("DIN");
    term.print(i);
    term.set_color(BT_RED, BT_BLACK);
    term.position(DIGITAL_ROW + 1, (i)*6 + 1);
    term.print(digitalRead(i));
  }

  term.set_attribute(BT_NORMAL);
  for(i = A0; i <= A5; i++) {
    term.position(ANALOG_ROW, (i-A0)*6 + 1);
    term.set_attribute(BT_NORMAL);
    term.print(F("AIN"));
    term.print(i - A0);
    term.set_color(BT_MAGENTA, BT_BLACK);
    term.position(ANALOG_ROW + 1, (i-A0)*6 + 1);
    term.print(analogRead(i));
  }
}

void setup(void) {
  Serial.begin(152000);

  pinMode(LED, OUTPUT);

  uint8_t i;
  for(i = A0; i <= A5; i++) {
    pinMode(i, INPUT);
  }

  for(i = 2; i <= 12; i++) {
    pinMode(i, INPUT);
  }

  for(i=0; i<PINS_NUM; i++) {
    pinMode(INDEX_TO_PIN[i], OUTPUT);
  }

  digitalWrite(LED, LOW);
    
  term.init();
  term.cls();
  term.show_cursor(false);

  currForm = &mainForm;
  currForm->draw();
}

void loop(void) {
  int key = term.get_key();
  currForm->handleKey(key);
  switch(key) {
    case '\f':
      /* Ctrl-L: redraw screen */
      term.cls();
      currForm->draw();
      break;
  }

  if (running)
  {
    runFunctions();
  }

  if (!running)
      printStatus();
}

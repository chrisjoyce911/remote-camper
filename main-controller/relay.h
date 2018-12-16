unsigned long debounceDelay = 250;    // the debounce time; increase if the output flickers

int ButtonPin1 = 26;
int ButtonPin2 = 25;
int ButtonPin3 = 33;
int ButtonPin4 = 32;

bool ButtonState1 = LOW;
bool ButtonState2 = LOW;
bool ButtonState3 = LOW;
bool ButtonState4 = LOW;

bool LastButtonState1 = LOW;
bool LastButtonState2 = LOW;
bool LastButtonState3 = LOW;
bool LastButtonState4 = LOW;

bool ButtonSwitchNow1 = false;
bool ButtonSwitchNow2 = false;
bool ButtonSwitchNow3 = false;
bool ButtonSwitchNow4 = false;

unsigned long lastDebounceTime1 = 0 ;
unsigned long lastDebounceTime2 = 0 ;
unsigned long lastDebounceTime3 = 0 ;
unsigned long lastDebounceTime4 = 0 ;

int Relay1 = 15; 
int Relay2 = 13;
int Relay3 = 12;
int Relay4 = 14;

bool RelayState1 = HIGH;
bool RelayState2 = HIGH;
bool RelayState3 = HIGH;
bool RelayState4 = HIGH;

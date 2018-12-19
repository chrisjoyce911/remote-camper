unsigned long debounceDelay = 250;    // the debounce time; increase if the output flickers

int ButtonPin1 = 26;
int ButtonPin2 = 25;
int ButtonPin3 = 33;
int ButtonPin4 = 32;

bool ButtonState1 = false;
bool ButtonState2 = false;
bool ButtonState3 = false;
bool ButtonState4 = false;

bool LastButtonState1 = false;
bool LastButtonState2 = false;
bool LastButtonState3 = false;
bool LastButtonState4 = false;

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

bool RelayState1 = false;
bool RelayState2 = false;
bool RelayState3 = false;
bool RelayState4 = false;

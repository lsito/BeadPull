
#define debug_on

#define r_enc 3
#define STEP_1 10
#define DIR_1 11
#define step_speed_min 31
#define step_speed_max 4900
#define NENA 4
#define MS1 5
#define MS2 6
#define MS3 7
#define NRES 8
#define NSLP 9

String inputString = "";
boolean stringComplete = false;
boolean FWD = true;
byte SM_state = 0;  // 0 = stopped, 1 = ramping, 2 = cruising, 3 = deramping
boolean cmd_start = false;
boolean cmd_stop = false;
boolean cmd_kill = false;
boolean gui_mode = false;
int rate_c = 0;
int rate_d = 100;
int rate_deramp = 0;
int acc = 2;
unsigned long r_elapsed = 0;
unsigned long r_previous;
unsigned long r_previous_temp = 0;
int speed_sign = 1;
int step_speed = 900;

void setup() {
  
  Serial.begin(115200);
  delay(1000);
  Serial.println("******************************************************");
  Serial.println("**** Bead Pull Motion Controller V1.0 by Mr. Pato ****");
  Serial.println("******************************************************");
  Serial.println();
  Serial.print("Initializing...  "); 

  pinMode(r_enc, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(r_enc), r_elapse, RISING);
  r_previous = micros();

  pinMode(DIR_1, OUTPUT);
  pinMode(STEP_1, OUTPUT);
  //TCCR1B = (TCCR1B & 0b11111000) | 0x04;  // freq = 122.5 to avoid beeping*
  pinMode(NENA, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(NRES, OUTPUT);
  pinMode(NSLP, OUTPUT);

  digitalWrite(DIR_1, LOW);
  digitalWrite(STEP_1, LOW);
  digitalWrite(NENA, HIGH);
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, LOW);
  digitalWrite(NSLP, HIGH);
  digitalWrite(NRES, LOW);
  
  delay(1000);
  digitalWrite(NRES, HIGH);
  Serial.println("Initialized!"); 
  Serial.println("Send 'h' for help menu."); 

}

void loop() {

  switch (SM_state) {
    case 0:  // stopped
      rate_c = 0;
      if (cmd_start) {
        SM_state++;
        //Serial.println("Starting acceleration ramp..."); 
        cmd_start = false;
      }
      cmd_stop = false;
      break;
    case 1:  // ramping
      if (cmd_stop) {
        SM_state +=2; 
        //Serial.println("Starting de-acceleration ramp..."); 
        cmd_stop = false;
      }
      else {
        if (rate_c < rate_d) {
          rate_c += acc;
          if (rate_c > rate_d) rate_c = rate_d;
        }
        else {
          SM_state++; 
          //Serial.println("Continuing movement at nominal speed..."); 
        }
      }
      break;
    case 2:  // cruising
      if (cmd_stop) {
        SM_state ++; 
        //Serial.println("Starting de-acceleration ramp..."); 
        cmd_stop = false;
      }
      else 
        if (rate_c != rate_d) {
          if (rate_d > rate_c) {
            SM_state = 1;
          }
          else {
            SM_state = 3;
            rate_deramp = rate_d;
          }
        }
      break;
    case 3:  // deramping
      if (rate_c > rate_deramp) {
        rate_c -= acc;
        if (rate_c < rate_deramp) rate_c = rate_deramp;
      }
      else {
        if (rate_c == 0) {
        SM_state = 0; 
        //Serial.println("Stopped."); 
        }
        else SM_state = 2; 
      }
      break;
    default:
      Serial.println("case error"); 
  }

  if (SM_state == 0) {
    digitalWrite(NENA, HIGH);
  }
  else {
	digitalWrite(NENA, LOW);
	step_speed = map((rate_c), 0, 100, step_speed_min, step_speed_max);
  tone(STEP_1,step_speed);
    #ifdef debug_on 
      if (SM_state != 2) {
      Serial.print("Moving at speed  "); Serial.print(rate_c); Serial.print("% and acceleration  "); Serial.print(acc); Serial.println("%.");
      }
    #endif
  }

  if (FWD) {
    digitalWrite(DIR_1, HIGH);
    speed_sign = 1;
  }
  else {
    digitalWrite(DIR_1, LOW);
    speed_sign = -1;
  }
  
  serialEvent();  
  if (stringComplete) {
    #ifdef debug_on 
      Serial.print(" -- processing string: ");Serial.println(inputString); 
    #endif
    stringProcess();
    inputString = "";
    stringComplete = false;  
  }

  if (gui_mode) gui_tasks();
  delay(50);

}


void gui_tasks() {
  //if (SM_state !=0) {
    Serial.print("v"); Serial.print(rate_c); Serial.print(";");
    Serial.print("x"); Serial.print(rate_c); Serial.print(";");
    Serial.print("y"); Serial.println(r_elapsed);
  //}
}

void r_elapse(){
  r_previous_temp = micros();
  r_elapsed = micros() - r_previous;
  r_previous = r_previous_temp;
}


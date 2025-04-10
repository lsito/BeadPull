void serialEvent() {
  while (Serial.available()>0) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == '\r') {
      stringComplete = true;
    }
    else {
    // add it to the inputString:
    inputString += inChar;
    }
  }
}

void stringProcess() {
  int tmp = 0;
  switch(inputString[0]) {
    case 'S':
        tmp = (inputString.substring(1,4)).toInt();
        if ((tmp < 101) & (tmp > 0)) {
          rate_d = tmp;
          Serial.print("Speed set to "); Serial.print(rate_d); Serial.println(" %."); 
        }
        else Serial.println("Speed must be betewen 0 and 100%!"); 
      break;
    case 'A':
        tmp = (inputString.substring(1,4)).toInt();
        if ((tmp < 101) & (tmp > 0)) {
          acc = tmp;
          Serial.print("Acceleration set to "); Serial.print(acc); Serial.println(" %."); 
        }
        else Serial.println("Acceleration must be betewen 0 and 100%!"); 
      break;
    case 'm':
        tmp = (inputString.substring(1)).toInt();
        if (tmp < 8) {
          digitalWrite(MS1,bitRead(tmp,2));
          digitalWrite(MS2,bitRead(tmp,1));
          digitalWrite(MS3,bitRead(tmp,0));
          Serial.print("Microstepping set to "); Serial.print(digitalRead(MS1));Serial.print(digitalRead(MS2));Serial.println(digitalRead(MS3));
        }
      break;
    case 'd':
      if (SM_state == 0) cmd_start = true;
      break;
    case 's':
      cmd_stop = true;
      rate_deramp = 0;
      break;
    case 'k':
      Serial.println("Killing movement!");
      SM_state = 0;
      break;      
    case 'f':
      if (!FWD) {
        if (SM_state == 0) {
          FWD = true;
          Serial.println("Direction changed to Forward."); 
        }
        else Serial.println("Stop before changing direction!"); 
      }
      break;      
    case 'r':
      if (FWD) {
        if (SM_state == 0) {
          FWD = false;
          Serial.println("Direction changed to Reverse."); 
        }
        else Serial.println("Stop before changing direction!"); 
      }
      break;
    case 'G':
      gui_mode = true;
      Serial.println("Running in GUI mode!"); 
      break;
    case 'h':
      Serial.println();
      Serial.println("'Sxxx' sets speed to xxx, between 0 and 100%. Motor should be stopped."); 
      Serial.println("'Axxx' sets acceletarion to xxx, between 0 and 100%. Motor should be stopped."); 
      Serial.println("'f' sets movement direction Forward. Motor should be stopped."); 
      Serial.println("'r' sets movement direction Reverse. Motor should be stopped."); 
      Serial.println("'d' starts the movement. Motor should be stopped."); 
      Serial.println("'s' stops the movement."); 
      Serial.println("'k' kills the movement immediately."); 
      break;
    default:    
      Serial.println("command error"); 
  }
}


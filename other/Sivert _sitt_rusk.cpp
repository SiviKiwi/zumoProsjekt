State state = State::RESET;

switch(state)
  {
    case State::RESET:

      state = State::WAIT_FOR_START_SIGNAL;
      break;

    case State::WAIT_FOR_START_SIGNAL:
      if(buttonA.isPressed()) {
        delay(2000);
        state = State::CALIBRATE_LINESENSORS;
      }

      break;

    case State::CALIBRATE_LINESENSORS:

      unsigned long time_now = millis();
      while (millis() < time_now + 5000)
      {

        lineSensors.calibrate();
        motors.setSpeeds(100, -100);

      }

      motors.setSpeeds(0, 0);
    // evt. egendefinert.batteryReplacement(); Husk da å trekke ut den som sjekker tilstanden 
    // inne i funksjonen.
      state = State::RESET;

      break;
  }
#define BUTTON_COUNT 32
#define DATA_PIN 3
#define SHIELD_PIN 0
#define CLOCK_PIN 1
#define CLOCK_ENABLE_PIN 2
#define SENSOR_PIN A0
#define READ_INTERVAL 50
#define DEBUG_MODE true
#define DEBUG_READ_INTERVAL 1000

typedef struct {
  int sensor;
  bool buttons[BUTTON_COUNT];
} Controller;

Controller pushed_state;
Controller current_state;


void setup()
{
#if DEBUG_MODE
  Serial.begin(38400);
#endif

  // Trigger send manually, otherwise we'll send 32+ packets for every change
  Joystick.useManualSend(true);

  pinMode(SHIELD_PIN, OUTPUT);
  pinMode(CLOCK_ENABLE_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT);

  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(SHIELD_PIN, HIGH);
}

void loop()
{
  update_state();

#if DEBUG_MODE
  print_state();
  delay(DEBUG_READ_INTERVAL);
#else
  if (state_changed()) {
    push_state();
  }
  delay(READ_INTERVAL);
#endif
}

/**
 * Read in the current state of the controller.
 */
void update_state()
{
  read_sensor();
  read_shift_registers();
}

/**
 * Read in the current value of the analog sensor.
 */
void read_sensor()
{
  current_state.sensor = analogRead(SENSOR_PIN);
}

/**
 * Read in the button state from the shift registers.
 */
void read_shift_registers()
{
  int i;
  int j;
  int btn = 0;
  byte shift_data;

  /* Trigger loading the state of the A-H data lines into the shift register */
  digitalWrite(SHIELD_PIN, LOW);
  delayMicroseconds(5); /* Requires a delay here according to the datasheet timing diagram */
  digitalWrite(SHIELD_PIN, HIGH);
  delayMicroseconds(5);

  /* Required initial states of these two pins according to the datasheet timing diagram */
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT);
  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(CLOCK_ENABLE_PIN, LOW); /* Enable the clock */

  for (i = 0; i < 4; i += 1) {
    shift_data = shiftIn(DATA_PIN, CLOCK_PIN, MSBFIRST);

    for (j = 0; j < 8; j += 1) {
      current_state.buttons[btn] = shift_data >> i & 1;
      btn += 1;
    }
  }

  digitalWrite(CLOCK_ENABLE_PIN, HIGH); /* Disable the clock */
}

/**
 * Return true if the controller state has changed since it was last pushed.
 *
 * @return whether pushed state is out of date
 */
bool state_changed()
{
  int i;

  if (current_state.sensor != pushed_state.sensor) {
    return true;
  }

  for (i = 0; i < BUTTON_COUNT; i += 1) {
    if (current_state.buttons[i] != pushed_state.buttons[i]) {
      return true;
    }
  }

  return false;
}

/**
 * Push the controller state to the computer.
 */
void push_state()
{
  int i;

  Joystick.Z(current_state.sensor);

  for (i = 0; i < BUTTON_COUNT; i += 1) {
    Joystick.button(i + 1, current_state.buttons[i]);
  }

  Joystick.send_now();

  pushed_state = current_state;
}

/**
 * Print the current controller state to the serial line.
 */
void print_state()
{
  int i;

  Serial.print("Sensor: ");
  Serial.print(current_state.sensor);
  Serial.print("\n");
  Serial.println("Buttons:");

  for (i = 0; i < BUTTON_COUNT; i += 1) {
    Serial.print(current_state.buttons[i], BIN);
  }

  Serial.print("\n");
}
const int PIN_DOWN = 2;
const int PIN_LEFT = 4;
const int PIN_RIGHT = 7;
const int PIN_UP = 8;
const int PIN_OK = 12;

const int BLOCK_I = 1;
const int BLOCK_J = 2;
const int BLOCK_L = 3;
const int BLOCK_O = 4;
const int BLOCK_S = 5;
const int BLOCK_T = 6;
const int BLOCK_Z = 7;

int buttons = 5;
int button_pins[] = { PIN_DOWN, PIN_LEFT, PIN_RIGHT, PIN_UP, PIN_OK };
bool button_states[] = { 0, 0, 0, 0, 0 };

int board_height = 16;
int board_width = 8;
bool board[16][8];

//bool block

int loop_counter = 0;

int blocks[] = { BLOCK_I, BLOCK_J, BLOCK_L, BLOCK_O, BLOCK_S, BLOCK_T, BLOCK_Z };
int current_block;

// --------------------------------

void setup() {

  Serial.begin(9600);

  for (int i = 0; i < buttons; i++) {
    pinMode(button_pins[i], INPUT);
  }

  digitalWrite(13, LOW);
  
  generate_block();
}

// ------------------------------

void loop() {
  read_buttons();

  loop_counter++;
  if (loop_counter == 100) {
    move_down();
    update_display();
    loop_counter = 0;
  }

  delay(1000/100); // 100hz
}

// -------------------------------------

void generate_block() {
  ;
}

void move_down() {
  ;
}

bool can_move_down() {
  return true;
}

// -------------------------------------

void read_buttons() {
  for (int i = 0; i < buttons; i++) {

    int pin = button_pins[i];

    int state = digitalRead(pin);

    if (button_states[i] != state) {
      button_states[i] = state;

      if (state == 0) {
        button_release(pin);
      }

      if (state == 1) {
        button_press(pin);
      }
    }

    if (state == 1) {
      button_down(pin);
    }
  }
}

void button_press(int pin) {
  digitalWrite(13, HIGH);

  switch (pin) {

    case PIN_DOWN:
      //
      break;

    case PIN_UP:
      //
      break;

    case PIN_LEFT:
      //
      break;

  }

  update_display();
}

void button_release(int pin) {
  digitalWrite(13, LOW);

  switch (pin) {

    case PIN_DOWN:
      //
      break;

  }
}

void button_down(int pin) {
  //Serial.print(pin);
}

void clear_board() {
  for (int y = board_height - 1; y >= 0; y--) {
    for (int x = 0; x < board_width; x++) {
      board[y][x] = 0;
    }
  }
}

void start() {
  digitalWrite(13, HIGH);
}

void update_display() {
  for (int y = board_height - 1; y >= 0; y--) {
    for (int x = 0; x < board_width; x++) {
      Serial.print(board[y][x] > 1 ? "#" : ".");
      Serial.print(" ");
    }
    Serial.println();
  }

  Serial.println();

  for (int i = 0; i < buttons; i++) {
    Serial.print(button_pins[i]);
    Serial.print("(");
    Serial.print(button_states[i] ? "#" : ".");
    Serial.print(") ");
  }

  Serial.println();
  Serial.println();
}

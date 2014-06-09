// Origin 0x0 is bottom left

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

int button_count = 5;
int button_pins[] = { PIN_DOWN, PIN_LEFT, PIN_RIGHT, PIN_UP, PIN_OK };
bool button_states[] = { 0, 0, 0, 0, 0 };

int board_height = 16;
int board_width = 8;

// display board array
// int array to handle colour
int board[16][8];

// Pile layer
int pile_layer[16][8];

// Block layer
int block_layer[18][8]; // two rows taller than board

// auto-drop delay
int loop_counter = 0;

// current_block
int current_block = 0;

// current_block_shape[z][y][x]
int current_block_shape[4][4][4];

int current_block_x;
int current_block_y;

// current_block_rotation = 0-3
int current_block_rotation;


// --------------------------------

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < button_count; i++) {
    pinMode(button_pins[i], INPUT);
  }

  digitalWrite(13, LOW);

  reset_game();
}

// ------------------------------

void loop() {
  read_buttons();

  loop_counter++;
  if (loop_counter == 50) {
    move_down();
    update_display();
    loop_counter = 0;
  }

  delay(1000 / 100); // 100hz
}

// -------------------------------------

void generate_block() {
  current_block = random(0, 7) + 1; // 1 - 7
  current_block_rotation = 0;
  current_block_x = 2;
  current_block_y = 14;

  int block_shape[4][4][4] = {0};

  switch (current_block) {
    case BLOCK_I:
      // - - - -
      // - - - -
      // # # # #
      // - - - -
      block_shape[0][1][0] = BLOCK_I;
      block_shape[0][1][1] = BLOCK_I;
      block_shape[0][1][2] = BLOCK_I;
      block_shape[0][1][3] = BLOCK_I;
      // - # - -
      // - # - -
      // - # - -
      // - # - -
      block_shape[1][3][1] = BLOCK_I;
      block_shape[1][2][1] = BLOCK_I;
      block_shape[1][1][1] = BLOCK_I;
      block_shape[1][0][1] = BLOCK_I;
      // - - - -
      // # # # #
      // - - - -
      // - - - -
      block_shape[2][2][0] = BLOCK_I;
      block_shape[2][2][1] = BLOCK_I;
      block_shape[2][2][2] = BLOCK_I;
      block_shape[2][2][3] = BLOCK_I;
      // - - # -
      // - - # -
      // - - # -
      // - - # -
      block_shape[3][3][2] = BLOCK_I;
      block_shape[3][2][2] = BLOCK_I;
      block_shape[3][1][2] = BLOCK_I;
      block_shape[3][0][2] = BLOCK_I;
      break;
      
    case BLOCK_J:
      // - - - -
      // # # # -
      // - - # -
      // - - - -
      block_shape[0][2][0] = BLOCK_J;
      block_shape[0][2][1] = BLOCK_J;
      block_shape[0][2][2] = BLOCK_J;
      block_shape[0][1][2] = BLOCK_J;
      // - # - -
      // - # - -
      // # # - -
      // - - - -
      block_shape[1][3][1] = BLOCK_J;
      block_shape[1][2][1] = BLOCK_J;
      block_shape[1][1][0] = BLOCK_J;
      block_shape[1][1][1] = BLOCK_J;
      // # - - -
      // # # # -
      // - - - -
      // - - - -
      block_shape[2][3][0] = BLOCK_J;
      block_shape[2][2][0] = BLOCK_J;
      block_shape[2][2][1] = BLOCK_J;
      block_shape[2][2][2] = BLOCK_J;      
      // - # # -
      // - # - -
      // - # - -
      // - - - -
      block_shape[3][3][1] = BLOCK_J;
      block_shape[3][3][2] = BLOCK_J;
      block_shape[3][2][1] = BLOCK_J;
      block_shape[3][1][1] = BLOCK_J;
      break;
      
    case BLOCK_L:
      // - - - -
      // # # # -
      // # - - -
      // - - - - 
      block_shape[0][2][0] = BLOCK_L;
      block_shape[0][2][1] = BLOCK_L;
      block_shape[0][2][2] = BLOCK_L;
      block_shape[0][1][0] = BLOCK_L;
      // # # - -
      // - # - -
      // - # - -
      // - - - - 
      block_shape[1][3][0] = BLOCK_L;
      block_shape[1][3][1] = BLOCK_L;
      block_shape[1][2][1] = BLOCK_L;
      block_shape[1][1][1] = BLOCK_L;
      // - - # -
      // # # # -
      // - - - -
      // - - - - 
      block_shape[2][3][2] = BLOCK_L;
      block_shape[2][2][0] = BLOCK_L;
      block_shape[2][2][1] = BLOCK_L;
      block_shape[2][2][2] = BLOCK_L;
      // - # - -
      // - # - -
      // - # # -
      // - - - - 
      block_shape[3][3][1] = BLOCK_L;
      block_shape[3][2][1] = BLOCK_L;
      block_shape[3][1][1] = BLOCK_L;
      block_shape[3][1][2] = BLOCK_L;
      break;
      
    case BLOCK_O:
      // - - - -
      // - # # -
      // - # # -
      // - - - - 
      block_shape[0][2][1] = BLOCK_O;
      block_shape[0][2][2] = BLOCK_O;
      block_shape[0][1][1] = BLOCK_O;
      block_shape[0][1][2] = BLOCK_O;
      // - - - -
      // - # # -
      // - # # -
      // - - - - 
      block_shape[1][2][1] = BLOCK_O;
      block_shape[1][2][2] = BLOCK_O;
      block_shape[1][1][1] = BLOCK_O;
      block_shape[1][1][2] = BLOCK_O;
      // - - - -
      // - # # -
      // - # # -
      // - - - - 
      block_shape[2][2][1] = BLOCK_O;
      block_shape[2][2][2] = BLOCK_O;
      block_shape[2][1][1] = BLOCK_O;
      block_shape[2][1][2] = BLOCK_O;
      // - - - -
      // - # # -
      // - # # -
      // - - - - 
      block_shape[3][2][1] = BLOCK_O;
      block_shape[3][2][2] = BLOCK_O;
      block_shape[3][1][1] = BLOCK_O;
      block_shape[3][1][2] = BLOCK_O;
      break;
    
    case BLOCK_S:
      // - - - -
      // - # # -
      // # # - -
      // - - - - 
      block_shape[0][2][1] = BLOCK_S;
      block_shape[0][2][2] = BLOCK_S;
      block_shape[0][1][0] = BLOCK_S;
      block_shape[0][1][1] = BLOCK_S;
      // # - - -
      // # # - -
      // - # - -
      // - - - - 
      block_shape[1][3][0] = BLOCK_S;
      block_shape[1][2][0] = BLOCK_S;
      block_shape[1][2][1] = BLOCK_S;
      block_shape[1][1][1] = BLOCK_S;
      // - # # -
      // # # - -
      // - - - - 
      // - - - -
      block_shape[2][3][1] = BLOCK_S;
      block_shape[2][3][2] = BLOCK_S;
      block_shape[2][2][0] = BLOCK_S;
      block_shape[2][2][1] = BLOCK_S;
      // - # - -
      // - # # -
      // - - # -
      // - - - - 
      block_shape[3][3][1] = BLOCK_S;
      block_shape[3][2][1] = BLOCK_S;
      block_shape[3][2][2] = BLOCK_S;
      block_shape[3][1][2] = BLOCK_S;
      break;
      
    case BLOCK_T:
      // - - - -
      // # # # -
      // - # - -
      // - - - - 
      block_shape[0][2][0] = BLOCK_T;
      block_shape[0][2][1] = BLOCK_T;
      block_shape[0][2][2] = BLOCK_T;
      block_shape[0][1][1] = BLOCK_T;
      // - # - -
      // # # - -
      // - # - -
      // - - - - 
      block_shape[1][3][1] = BLOCK_T;
      block_shape[1][2][0] = BLOCK_T;
      block_shape[1][2][1] = BLOCK_T;
      block_shape[1][1][1] = BLOCK_T;
      // - # - -
      // # # # -
      // - - - -
      // - - - - 
      block_shape[2][3][1] = BLOCK_T;
      block_shape[2][2][0] = BLOCK_T;
      block_shape[2][2][1] = BLOCK_T;
      block_shape[2][2][2] = BLOCK_T;
      // - # - -
      // - # # -
      // - # - -
      // - - - - 
      block_shape[3][3][1] = BLOCK_T;
      block_shape[3][2][1] = BLOCK_T;
      block_shape[3][2][2] = BLOCK_T;
      block_shape[3][1][1] = BLOCK_T;
      break;
      
    case BLOCK_Z:
      // - - - -
      // # # - -
      // - # # -
      // - - - - 
      block_shape[0][2][0] = BLOCK_Z;
      block_shape[0][2][1] = BLOCK_Z;
      block_shape[0][1][1] = BLOCK_Z;
      block_shape[0][1][2] = BLOCK_Z;
      // - # - -
      // # # - -
      // # - - -
      // - - - - 
      block_shape[1][3][1] = BLOCK_Z;
      block_shape[1][2][0] = BLOCK_Z;
      block_shape[1][2][1] = BLOCK_Z;
      block_shape[1][1][0] = BLOCK_Z;
      // # # - -
      // - # # -
      // - - - - 
      // - - - -
      block_shape[2][3][0] = BLOCK_Z;
      block_shape[2][3][1] = BLOCK_Z;
      block_shape[2][2][1] = BLOCK_Z;
      block_shape[2][2][2] = BLOCK_Z;
      // - - # -
      // - # # -
      // - # - -
      // - - - - 
      block_shape[3][3][2] = BLOCK_Z;
      block_shape[3][2][1] = BLOCK_Z;
      block_shape[3][2][2] = BLOCK_Z;
      block_shape[3][1][1] = BLOCK_Z;
      break;
  }

  // copy to current_block_shape
  for (int z = 0; z < 4; z++) {
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        current_block_shape[z][y][x] = block_shape[z][y][x];
      }
    }
  }
}

// -----------------------------------

void move_down() {
  if (can_move_down()) {
    current_block_y--;
    loop_counter = 0;
//    Serial.print("current_block_y: ");
//    Serial.println(current_block_y);
  }
  else {
    copy_block_layer_to_pile_layer();
    check_lines();
    generate_block();
  }
}

bool can_move_down() {
  int new_block_y = current_block_y - 1;
  
  // detect floor hit
  int y_lowest = 0;
  for (int y = 3; y >= 0; y--) {
    for (int x = 0; x < 4; x++) {
      if (current_block_shape[current_block_rotation][y][x] > 0) {
        y_lowest = y;
      }
    }
  }
  if (new_block_y + y_lowest < 0) {
    return false;
  }
  
  // detect block collision
  for (int y = 3; y >= 0; y--) {
    for (int x = 0; x < 4; x++) {
      if (current_block_shape[current_block_rotation][y][x] > 0) {
        int collision_y = new_block_y + y;
        int collision_x = current_block_x + x;
        if (pile_layer[collision_y][collision_x] > 0) {
          return false;
        }
      }
    }
  }
  
  // all clear
  return true;
}

// -------------------------------------

void move_left() {
  if (can_move_left()) {
    current_block_x--;
  }
}

bool can_move_left() {
  return true;
}

// -------------------------------------

void move_right() {
  if (can_move_right()) {
    current_block_x++;
  }
}

bool can_move_right() {
  return true;
}

// -------------------------------------

void rotate() {
  current_block_rotation++;
  if (current_block_rotation > 3) {
    current_block_rotation = 0;
  }
}

// -------------------------------------

void check_lines() {
  int full_lines = 0;
  int full_line_numbers[4];
  
  for (int y = 0; y < 16; y++) {
    int line_sum = 0;
    for (int x = 0; x < 8; x++) {
      if (pile_layer[y][x] > 0) line_sum++;
    }
    if (line_sum == 8) {
      full_line_numbers[full_lines] = y;
      full_lines++;
    }
  }
  
  Serial.print("full_lines: ");
  Serial.println(full_lines);
  
  for (int n = full_lines - 1; n >= 0; n--) {
    int from_y = full_line_numbers[n];
    for (int y = from_y; y < 15; y++) {
      for (int x = 0; x < 8; x++) {
        pile_layer[y][x] = pile_layer[y+1][x];
      }
    }
  }
}

// -------------------------------------

void read_buttons() {
  for (int i = 0; i < button_count; i++) {

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
      move_down();
      break;

    case PIN_UP:
      rotate();
      break;

    case PIN_LEFT:
      move_left();
      break;
      
    case PIN_RIGHT:
      move_right();
      break;

    case PIN_OK:
      reset_game();
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
  ;
}

void reset_game() {
  loop_counter = 0;
  clear_board();
  clear_pile_layer();
  clear_block_layer();
  generate_block();
}

void clear_board() {
  for (int y = board_height - 1; y >= 0; y--) {
    for (int x = 0; x < board_width; x++) {
      board[y][x] = 0;
    }
  }
}

void clear_pile_layer() {
  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 8; x++) {
      pile_layer[y][x] = 0;
    }
  }
}

void clear_block_layer() {
  for (int y = 0; y < 18; y++) {
    for (int x = 0; x < 8; x++) {
      block_layer[y][x] = 0;
    }
  }
}

void copy_block_layer_to_pile_layer() {
  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 8; x++) {
      if (block_layer[y][x] > 0) {
        pile_layer[y][x] = block_layer[y][x];
      }
    }
  }
  clear_block_layer();
}

void start() {
  generate_block();
}

void render_block_layer() {
  clear_block_layer();
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      block_layer[current_block_y + y][current_block_x + x] = current_block_shape[current_block_rotation][y][x];
    }
  }
 
}

void render_board() {
  render_block_layer();
  
  for (int y = 0; y < board_height; y++) {
    for (int x = 0; x < board_width; x++) {
      // reset pixel
      board[y][x] = 0;
      // copy on pile layer if set
      if (pile_layer[y][x] > 0) {
        board[y][x] = pile_layer[y][x];
      }
      // copy on block layer if set
      if (block_layer[y][x] > 0) {
        board[y][x] = block_layer[y][x];
      }
    }
  }
}

void update_display() {
  render_board();

  led_display();
  serial_display();
}

void led_display() {
  ;
}

void serial_display() {
  for (int y = board_height - 1; y >= 0; y--) {
    for (int x = 0; x < board_width; x++) {
      Serial.print((board[y][x] > 0) ? String(board[y][x]) : "-");
//      Serial.print((board[y][x] > 0) ? "#" : "-");
      Serial.print(" ");
    }
    Serial.println();
  }

  Serial.println();

  for (int i = 0; i < button_count; i++) {
    Serial.print(button_pins[i]);
    Serial.print("(");
    Serial.print(button_states[i] ? "#" : ".");
    Serial.print(") ");
  }

  Serial.println();
  Serial.println();
}

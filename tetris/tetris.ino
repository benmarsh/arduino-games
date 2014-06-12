#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

// Origin 0x0 is bottom left

const byte PIN_DOWN = 4;
const byte PIN_LEFT = 5;
const byte PIN_RIGHT = 7;
const byte PIN_UP = 8;
const byte PIN_A = 12; // rotate clockwise
const byte PIN_B = 11; // rotate anti-clockwise

const byte BLOCK_I = 1;
const byte BLOCK_J = 2;
const byte BLOCK_L = 3;
const byte BLOCK_O = 4;
const byte BLOCK_S = 5;
const byte BLOCK_T = 6;
const byte BLOCK_Z = 7;

const byte button_count = 6;
const byte button_pins[] = { PIN_DOWN, PIN_LEFT, PIN_RIGHT, PIN_UP, PIN_B, PIN_A };
bool button_states[] = { 0, 0, 0, 0, 0, 0 };

const byte board_height = 16;
const byte board_width = 8;

// display board array
// int array to handle colours
byte board[16][8] = {0};

// Pile layer
byte pile_layer[16][8] = {0};

// Block layer
byte block_layer[18][8] = {0}; // two rows taller than board

// auto-drop delay
unsigned long loop_timer = 0;
unsigned int down_interval = 500; // ms

//long button_timer = 0;
//int 

// current_block
byte current_block_type = 0;

// current_block_shape[rotation][y][x]
byte current_block_shape[4][4][4] = {0};

int current_block_x = 2;
int current_block_y = 14;

// current_block_rotation = 0-3
byte current_block_rotation = 0;

// LED displays
Adafruit_BicolorMatrix matrix_upper = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix_lower = Adafruit_BicolorMatrix();
byte matrix_brightness = 10;

// game scoring
int rows_cleared;
int tetris_cleared;
int level;
int level_up_in;

// --------------------------------

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < button_count; i++) {
    pinMode(button_pins[i], INPUT);
  }

  digitalWrite(13, LOW);
  
  matrix_upper.begin(0x70);
  matrix_upper.setBrightness(matrix_brightness);
  matrix_upper.clear();
  matrix_upper.writeDisplay();
  
  matrix_lower.begin(0x71);
  matrix_lower.setBrightness(matrix_brightness);
  matrix_lower.clear();
  matrix_lower.writeDisplay();

  reset_game();
}

// ------------------------------

void loop() {
  read_buttons();

  if (loop_timer < millis()) {
    reset_loop_timer();
    
    move_down();
    update_display();
  }
  
  delay(10);
}



void reset_loop_timer() {
  loop_timer = millis() + down_interval;
}

// -------------------------------------

void reset_game() {
  randomSeed(analogRead(0));
  
  current_block_type = 0;
  down_interval = 500;
  level = 0;
  level_up_in = 10;
  rows_cleared = 0;
  tetris_cleared = 0;
  
  for (int y = 15; y >= 0; y--) {
    for (int x = 0; x < 8; x++) {
      pile_layer[y][x] = random(1, 4);
    }
    update_display();
    delay(20);
  }
  
  for (int y = 15; y >= 0; y--) {
    for (int x = 0; x < 8; x++) {
      pile_layer[y][x] = 0;
    }
    update_display();
    delay(20);
  }
  
  clear_board();
  clear_pile_layer();
  clear_block_layer();
  
  delay(1000);
  
  // fill lower 4 rows
//  for (int y = 0; y < 4; y++) {
//    for (int x = 0; x < 8; x++) {
//      pile_layer[y][x] = random(1, 4);
//    }
//  }

  level_up();
  
  generate_block();
}


void level_up() {
  level++;
  level_up_in = 10;
  down_interval = 500 - (level * 40);
  
  delay(200);
  
  matrix_lower.setTextWrap(false);
  matrix_lower.setTextSize(1);
  matrix_lower.setTextColor(LED_YELLOW);
  matrix_lower.clear();
  matrix_lower.setCursor(2,0);
  matrix_lower.print(level);
  matrix_lower.writeDisplay();
  
  matrix_upper.setTextWrap(false);
  matrix_upper.setTextSize(1);
  matrix_upper.setTextColor(LED_YELLOW);
  for (int8_t x=7; x>=-30; x--) {
    matrix_upper.clear();
    matrix_upper.setCursor(x,0);
    matrix_upper.print("Level");
    matrix_upper.writeDisplay();
    delay(60);
  }
  
  delay(100);
}



void generate_block() {
  current_block_type = random(0, 7) + 1; // 1 - 7
  current_block_rotation = 0;
  current_block_x = 2;
  current_block_y = 14;

  int block_shape[4][4][4] = {0};

  switch (current_block_type) {
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
  
//  Serial.println(free_sram());
}

// -----------------------------------

void move_down() {
  if (can_move_down()) {
    current_block_y--;
  }
  else {
    if (is_game_over()) {
      game_over();
    }
    else {
      move_block_layer_to_pile_layer();
      check_rows();
      generate_block();
      
      Serial.print("Score: ");
      Serial.println(rows_cleared);
    }
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



void drop_down() {
  while (can_move_down()) {
    move_down();
    update_display();
    //delay(20);
  }
}

// ---------------------------------------------

bool is_game_over() {
  if (!can_move_down() && current_block_y == 14) {
    return true;
  }
  return false;
}



void game_over() {
  move_block_layer_to_pile_layer();
  delay(1000);
  reset_game();
}

// -------------------------------------

void move_left() {
  if (can_move_left()) {
    current_block_x--;
  }
}



bool can_move_left() {
  int new_block_x = current_block_x - 1;
  
  // detect wall hit
  int x_lowest = 0;
  for (int x = 3; x >= 0; x--) {
    for (int y = 3; y >= 0; y--) {
      if (current_block_shape[current_block_rotation][y][x] > 0) {
        x_lowest = x;
      }
    }
  }
  if (new_block_x + x_lowest < 0) {
    return false;
  }
  
  // detect block collision
  for (int y = 3; y >= 0; y--) {
    for (int x = 0; x < 4; x++) {
      if (current_block_shape[current_block_rotation][y][x] > 0) {
        int collision_y = current_block_y + y;
        int collision_x = new_block_x + x;
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

void move_right() {
  if (can_move_right()) {
    current_block_x++;
  }
}



bool can_move_right() {
  int new_block_x = current_block_x + 1;
  
  // detect wall hit
  int x_highest = 0;
  for (int x = 0; x < 4; x++) {
    for (int y = 3; y >= 0; y--) {
      if (current_block_shape[current_block_rotation][y][x] > 0) {
        x_highest = x;
      }
    }
  }
  if (new_block_x + x_highest >= board_width) {
    return false;
  }
  
  // detect block collision
  for (int y = 3; y >= 0; y--) {
    for (int x = 0; x < 4; x++) {
      if (current_block_shape[current_block_rotation][y][x] > 0) {
        int collision_y = current_block_y + y;
        int collision_x = new_block_x + x;
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

void rotate_clockwise() {
  if (can_rotate_clockwise()) {
    current_block_rotation = (current_block_rotation + 1) % 4;
  }
}

bool can_rotate_clockwise() {
  int new_block_rotation = (current_block_rotation + 1) % 4;
  
  // detect wall hit
  int x_lowest = 0;
  for (int x = 3; x >= 0; x--) {
    for (int y = 3; y >= 0; y--) {
      if (current_block_shape[new_block_rotation][y][x] > 0) {
        x_lowest = x;
      }
    }
  }
  if (current_block_x + x_lowest < 0) {
    return false;
  }
  
  int x_highest = 0;
  for (int x = 0; x < 4; x++) {
    for (int y = 3; y >= 0; y--) {
      if (current_block_shape[new_block_rotation][y][x] > 0) {
        x_highest = x;
      }
    }
  }
  if (current_block_x + x_highest >= board_width) {
    return false;
  }
  
  // detect block collision
  for (int y = 3; y >= 0; y--) {
    for (int x = 0; x < 4; x++) {
      if (current_block_shape[new_block_rotation][y][x] > 0) {
        int collision_y = current_block_y + y;
        int collision_x = current_block_x + x;
        if (pile_layer[collision_y][collision_x] > 0) {
          return false;
        }
      }
    }
  }
  
  return true;
}

// -------------------------------------

void rotate_anti_clockwise() {
  if (can_rotate_anti_clockwise()) {
    current_block_rotation = (current_block_rotation + 4 - 1) % 4;
  }
}

bool can_rotate_anti_clockwise() {
  int new_block_rotation = (current_block_rotation + 4 - 1) % 4;
  
  // detect wall hit
  int x_lowest = 0;
  for (int x = 3; x >= 0; x--) {
    for (int y = 3; y >= 0; y--) {
      if (current_block_shape[new_block_rotation][y][x] > 0) {
        x_lowest = x;
      }
    }
  }
  if (current_block_x + x_lowest < 0) {
    return false;
  }
  
  int x_highest = 0;
  for (int x = 0; x < 4; x++) {
    for (int y = 3; y >= 0; y--) {
      if (current_block_shape[new_block_rotation][y][x] > 0) {
        x_highest = x;
      }
    }
  }
  if (current_block_x + x_highest >= board_width) {
    return false;
  }
  
  // detect block collision
  for (int y = 3; y >= 0; y--) {
    for (int x = 0; x < 4; x++) {
      if (current_block_shape[new_block_rotation][y][x] > 0) {
        int collision_y = current_block_y + y;
        int collision_x = current_block_x + x;
        if (pile_layer[collision_y][collision_x] > 0) {
          return false;
        }
      }
    }
  }
  
  return true;
}

// -------------------------------------

void check_rows() {
  int full_rows = 0;
  int full_row_numbers[4] = {0};
  
  for (int y = 0; y < 16; y++) {
    int row_sum = 0;
    for (int x = 0; x < 8; x++) {
      if (pile_layer[y][x] > 0) {
        row_sum++;
      }
    }
    if (row_sum == 8) {
      full_row_numbers[full_rows] = y;
      full_rows++;
    }
  }
  
  // add sum to global total
  rows_cleared += full_rows;
  
  // check if it is tetris!
  if (full_rows == 4) {
    tetris_cleared++;
  }
    
  // rem full rows
  int row[4][8] = {0};
  for (int n = 0; n < full_rows; n++) {
    int y = full_row_numbers[n];
    for (int x = 0; x < 8; x++) {
      row[n][x] = pile_layer[y][x];
    }
  }
    
  // flash full rows
  for (int n = 0; n < 4; n++) {
    
    for (int n = 0; n < full_rows; n++) {
      int full_row_y = full_row_numbers[n];
      for (int x = 0; x < 8; x++) {
        pile_layer[full_row_y][x] = 0;      
      }
    }
    
    update_display();
    delay(40);
    
    for (int n = 0; n < full_rows; n++) {
      int full_row_y = full_row_numbers[n];
      for (int x = 0; x < 8; x++) {
        pile_layer[full_row_y][x] = row[n][x];
      }
    }
    
    update_display();
    delay(40);
  }
    
  for (int n = (full_rows - 1); n >= 0; n--) { 
    int full_row_y = full_row_numbers[n];
    // move row down 
    for (int y = full_row_y; y < 15; y++) {
      for (int x = 0; x < 8; x++) {
        pile_layer[y][x] = pile_layer[y+1][x];
      }
    }
  }
  
  // level up?
  level_up_in -= full_rows;
  if (level_up_in < 0) {
    level_up();
  }
}

// -------------------------------------

void read_buttons() {
  for (byte i = 0; i < button_count; i++) {

    byte pin = button_pins[i];

    bool state = digitalRead(pin);

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



void button_press(byte pin) {
  switch (pin) {
    
    case PIN_DOWN:
      reset_loop_timer();
      move_down();
      break;

    case PIN_UP:
      reset_loop_timer();
      drop_down();
      break;

    case PIN_LEFT:
      reset_loop_timer();
      move_left();
      break;
      
    case PIN_RIGHT:
      reset_loop_timer();
      move_right();
      break;

    case PIN_A:
      reset_loop_timer();
      rotate_clockwise();
      break;
    
    case PIN_B:
      reset_loop_timer();
      rotate_anti_clockwise();
      break;
      
  }
  
  update_display();
}



void button_release(byte pin) {
  switch (pin) {

    case PIN_DOWN:
      //
      break;

  }
}



void button_down(byte pin) {
  switch (pin) {
  
    case PIN_DOWN:
      //move_down();
      break;
      
  }
}

// ----------------------------------

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



void move_block_layer_to_pile_layer() {
  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 8; x++) {
      if (block_layer[y][x] > 0) {
        pile_layer[y][x] = block_layer[y][x];
      }
    }
  }
  clear_block_layer();
  current_block_type = 0;
}



void render_block_layer() {
  clear_block_layer();
  if (current_block_type > 0) {
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        block_layer[current_block_y + y][current_block_x + x] = current_block_shape[current_block_rotation][y][x];
      }
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
//        board[y][x] = LED_RED;
      }
      // copy on block layer if set
      if (block_layer[y][x] > 0) {
        board[y][x] = block_layer[y][x];
//        board[y][x] = LED_GREEN;
      }
    }
  }
}



void update_display() {
  render_board();

  led_display();
  //serial_display();
}



void led_display() {
  matrix_upper.clear();
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      int colour = block_to_led_colour(board[y+8][x]);
      matrix_upper.drawPixel(x, 7-y, colour);
    }
  }
  matrix_upper.setBrightness(matrix_brightness);
  matrix_upper.writeDisplay();
  
  matrix_lower.clear();
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      int colour = block_to_led_colour(board[y][x]);
      matrix_lower.drawPixel(x, 7-y, colour);
    }
  }
  matrix_lower.setBrightness(matrix_brightness);
  matrix_lower.writeDisplay();
}



int block_to_led_colour(int block) {
  // LED_RED = 1, 2 or 3
  // LED_YELLOW = 2
  // LED_GREEN = 3
  if (block == 0) return 0;
  return (block % 3) + 1;
}



//void serial_display() {
//  for (int y = board_height - 1; y >= 0; y--) {
//    for (int x = 0; x < board_width; x++) {
//      Serial.print((board[y][x] > 0) ? String(board[y][x]) : "-");
//      Serial.print(" ");
//    }
//    Serial.println();
//  }
//
//  Serial.println();
//
//  for (int i = 0; i < button_count; i++) {
//    Serial.print(button_pins[i]);
//    Serial.print("(");
//    Serial.print(button_states[i] ? "#" : ".");
//    Serial.print(") ");
//  }
//
//  Serial.println();
//  Serial.println();
//}



//int free_sram() {
//  extern int __heap_start, *__brkval; 
//  int v; 
//  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
//}

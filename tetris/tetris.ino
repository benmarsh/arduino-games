#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

// Origin 0x0 is bottom left

const int PIN_DOWN = 4;
const int PIN_LEFT = 5;
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

const int button_count = 5;
const int button_pins[] = { PIN_DOWN, PIN_LEFT, PIN_RIGHT, PIN_UP, PIN_OK };
bool button_states[] = { 0, 0, 0, 0, 0 };

const int board_height = 16;
const int board_width = 8;

// display board array
// int array to handle colours
int board[16][8] = {0};

// Pile layer
int pile_layer[16][8] = {0};

// Block layer
int block_layer[18][8] = {0}; // two rows taller than board

// auto-drop delay
int loop_counter = 0;

// current_block
int current_block_type = 0;

// current_block_shape[rotation][y][x]
int current_block_shape[4][4][4] = {0};

int current_block_x = 0;
int current_block_y = 0;

// current_block_rotation = 0-3
int current_block_rotation = 0;

// LED displays
Adafruit_BicolorMatrix matrix_upper = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix_lower = Adafruit_BicolorMatrix();
int matrix_brightness = 1;


// --------------------------------

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < button_count; i++) {
    pinMode(button_pins[i], INPUT);
  }

  digitalWrite(13, LOW);
  
  matrix_upper.begin(0x70);
  matrix_upper.clear();
  matrix_upper.writeDisplay();
  
  matrix_lower.begin(0x71);
  matrix_lower.clear();
  matrix_lower.writeDisplay();

  reset_game();
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

  delay(5); // 100hz
}

// -------------------------------------

void reset_game() {
  randomSeed(analogRead(0));
  
  loop_counter = 0;
  
  current_block_type = 0;
  
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
  
  generate_block();
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
  
  Serial.println(free_sram());
}

// -----------------------------------

void move_down() {
  if (can_move_down()) {
    current_block_y--;
    loop_counter = 0;
  }
  else {
    if (is_game_over()) {
      game_over();
    }
    else {
      move_block_layer_to_pile_layer();
      check_rows();
      generate_block();
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

void rotate() {
  current_block_rotation++;
  if (current_block_rotation > 3) {
    current_block_rotation = 0;
  }
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
  
  if (full_rows == 4) {
    // that's TETRIS!
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



void start() {
  generate_block();
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



int free_sram() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

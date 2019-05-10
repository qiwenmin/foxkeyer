/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this program.  If not, see
<http://www.gnu.org/licenses/>.
*/

#include <fsmosc.h>

#ifdef ARDUINO
#include <avr/pgmspace.h>
const char msg[] PROGMEM = {
#else
const char msg[] = {
#endif // ARDUINO
  "__ ___ .  "      // MOE
//  "__ ___ ..  "     // MOI
//  "__ ___ ...  "    // MOS
//  "__ ___ ....  "   // MOH
//  "__ ___ ......  " // MO5
};

#define CW_WPM (15)
#define IDLE_MS (500)

#define E_DURATION (1200 / CW_WPM)

// ARDUINO (digispark)
#ifdef ARDUINO

#include <Arduino.h>

#define KEY_PIN (1)

#define GET_MSG_CHAR(p) pgm_read_byte_near(p)

static void init_pin() {
  pinMode(KEY_PIN, OUTPUT);
}

static void key_up() {
  digitalWrite(KEY_PIN, 0);
}

static void key_down() {
  digitalWrite(KEY_PIN, 1);
}

#endif // ARDUINO

// STM8S103
#ifdef __SDCC_stm8

#include <stm8s.h>

#define KEY_PIN (5)

#define set_bit(register_8, bit) (register_8 |= (1 << bit))
#define clear_bit(register_8, bit) (register_8 &= ~(1 << bit))
#define toggle_bit(register_8, bit) (register_8 ^= (1 << bit))

#define GET_MSG_CHAR(p) (*(p))

static void init_pin() {
  set_bit(PB_DDR, KEY_PIN); 
  set_bit(PB_CR1, KEY_PIN); 
}

static void key_up() {
  clear_bit(PB_ODR, KEY_PIN);
}

static void key_down() {
  set_bit(PB_ODR, KEY_PIN);
}

#endif // __SDCC_stm8

enum {
  KEY_DOWN = FO_STATE_TASK_DEF_BEGIN + 1,
  KEY_UP,
  KEY_IDLE
};

static uint8_t _msg_pos;

void keying_task_f(fo_task_ptr t, FO_STATE state, FO_STATE new_state) {
  state; // not used
  char ch;
  switch (new_state) {
  case FO_STATE_INIT:
    _msg_pos = 0;

    init_pin();

    fo_goto_state(t, KEY_IDLE);
  case KEY_DOWN:
    ch = GET_MSG_CHAR(msg + _msg_pos);

    if ((ch == '.') || (ch == '_')) {
      key_down();
    } else {
      key_up();
    }

    fo_delay(t,
      E_DURATION * (
        ch == '.' ? 1 : (ch == '_' ? 3 : 2)
        ),
      KEY_UP);
    break;
  case KEY_UP:
    key_up();
    _msg_pos ++;
    fo_delay(t, E_DURATION, KEY_IDLE);
    break;
  case KEY_IDLE:
    ch = GET_MSG_CHAR(msg + _msg_pos);
    if (ch == 0) {
      _msg_pos = 0;
      fo_delay(t, IDLE_MS, KEY_DOWN);
    } else {
      fo_goto_state(t, KEY_DOWN);
    }
  default:
    break;
  }
}

FO_INIT(1)

void setup() {
  fo_create_task(keying_task_f);
}

void loop() {
  fo_loop();
}

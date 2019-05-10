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
#include <avr/pgmspace.h>

#define CW_WPM (15)
#define IDLE_MS (500)

const char msg[] PROGMEM = {
  "__ ___ .  "      // MOE
//  "__ ___ ..  "     // MOI
//  "__ ___ ...  "    // MOS
//  "__ ___ ....  "   // MOH
//  "__ ___ ......  " // MO5
};

#define KEY_PIN (1)

#define E_DURATION (1200 / CW_WPM)

enum {
  KEY_DOWN = FO_STATE_TASK_DEF_BEGIN + 1,
  KEY_UP,
  KEY_IDLE
};

static uint8_t _msg_len;
static uint8_t _msg_pos;

void keying_task_f(fo_task_ptr t, FO_STATE, FO_STATE new_state) {
    char ch;
    switch (new_state) {
    case FO_STATE_INIT:
      _msg_len = strlen_P(msg);
      _msg_pos = 0;

      pinMode(KEY_PIN, OUTPUT);

      fo_goto_state(t, KEY_IDLE);
    case KEY_DOWN:
      ch = pgm_read_byte_near(msg + _msg_pos);
 
      digitalWrite(KEY_PIN, (ch == '.') || (ch == '_') ? 1 : 0);
      fo_delay(t,
        E_DURATION * (
          ch == '.' ? 1 : (ch == '_' ? 3 : 2)
          ),
        KEY_UP);
      break;
    case KEY_UP:
      digitalWrite(KEY_PIN, 0);
      _msg_pos ++;
      fo_delay(t, E_DURATION, KEY_IDLE);
      break;
    case KEY_IDLE:
      if (_msg_pos == _msg_len) {
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

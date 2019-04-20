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

#include <fsmos.h>
#include <avr/pgmspace.h>

#define CW_WPM (15)
#define IDLE_MS (5000)

const char msg[] PROGMEM = {
  "_.. .  "                   // DE
  "_... __. .____ ._. . _.  " // BG1REN
  ".._. ___ _.._  .____  _._  "   // FOX 1 K
};

#define KEY_PIN (1)

#define E_DURATION (1200 / CW_WPM)

enum {
  KEY_DOWN = FSM_STATE_USERDEF + 1,
  KEY_UP,
  KEY_IDLE
};

class KeyingTask : public FsmTask {
private:
  uint8_t _msg_len;
  uint8_t _msg_pos;
public:
  virtual void init() {
    _msg_len = strlen_P(msg);
    _msg_pos = 0;

    pinMode(KEY_PIN, OUTPUT);

    this->gotoState(KEY_IDLE);
  };

  virtual bool on_state_change(int8_t new_state, int8_t /* old_state */) {
    char ch;
    switch (new_state) {
    case KEY_DOWN:
      ch = pgm_read_byte_near(msg + _msg_pos);
 
      digitalWrite(KEY_PIN, (ch == '.') || (ch == '_') ? 1 : 0);
      this->delay(
        E_DURATION * (
          ch == '.' ? 1 : (ch == '_' ? 3 : 2)
          ),
        KEY_UP);
      break;
    case KEY_UP:
      digitalWrite(KEY_PIN, 0);
      _msg_pos ++;
      this->delay(E_DURATION, KEY_IDLE);
      break;
    case KEY_IDLE:
      if (_msg_pos == _msg_len) {
        _msg_pos = 0;
        this->delay(IDLE_MS, KEY_DOWN);
      } else {
        this->gotoState(KEY_DOWN);
      }
    default:
      break;
    }
    return true;
  };

  virtual void in_state(int8_t) {};
};

FsmOs os(1);
KeyingTask kt;

void setup() {
  os.addTask(&kt);

  os.init();
}

void loop() {
  os.loop();
}

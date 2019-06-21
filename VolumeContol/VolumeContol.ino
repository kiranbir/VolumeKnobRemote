/*Libraries
 * IRREMOTE
 * RotaryEncoder
 */
#include <RotaryEncoder.h>
#include <IRremote.h>
#include <stdint.h>

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG 1

const uint32_t VOL_UP = 0xB54A50AF;
const uint32_t VOL_DOWN = 0xB54AD02F;
const uint32_t MUTE[] = {0xB54A9867, 0xF50A0CF3};

RotaryEncoder encoder(A2, A3);
const uint32_t RECV_PIN = 11;
const uint32_t BUTTON_PIN = 12;

IRrecv irrecv(RECV_PIN);
IRsend irsend;

int lastButtonState;

const uint32_t TX_DELAY_MS = 40;

void setup()
{
  #if defined(DEBUG)
    Serial.begin(115200);
  #endif
  
  Serial.println(F("Volume Remote Knob"));
  
  PCICR |= (1 << PCIE1);
  PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);
  
  pinMode(BUTTON_PIN, INPUT);
}

ISR(PCINT1_vect)
{
  encoder.tick(); // just call tick() to check the state.
}

void loop()
{
  static int pos = 0;

  int newPos = encoder.getPosition();
  if (pos != newPos)
  {
    if (newPos > pos)
    {
      Serial.println(F("Vol+"));
      irsend.sendNEC(VOL_UP, 32);
      delay(TX_DELAY_MS);
    }
    else
    {
      Serial.println(F("Vol-"));
      irsend.sendNEC(VOL_DOWN, 32);
      delay(TX_DELAY_MS);
    }

    pos = newPos;
  }

  int buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && buttonState == LOW)
  {
    Serial.println(F("Mute"));
    irsend.sendNEC(MUTE[0], 32);
    delay(TX_DELAY_MS);
    irsend.sendNEC(MUTE[1], 32);
    delay(TX_DELAY_MS);
  }

  lastButtonState = buttonState;

}

#include <AltSoftSerial.h>
#include <ssrn.h>

AltSoftSerial altSerial;

void led_on()
{
  digitalWrite(LED_BUILTIN, HIGH);
}

void led_off()
{
  digitalWrite(LED_BUILTIN, LOW);
}

#define LED_TIMER SSRN_TIMER0

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  ssrn_init(&altSerial);

  led_on();
  ssrn_set_timer(LED_TIMER, 100);
}

void loop()
{
  while(1){
    ssrn_event_t *event = ssrn_next_event();

    if (SSRN_EVENT_TIMER == event->type){
      // if the LED on timer expired, turn the LED off
      if (LED_TIMER == event->timer_idx){
        led_off();
      }
    } else if (SSRN_EVENT_PACKET == event->type){
      uint8_t *t = &event->packet->data[20];
      if (ssrn_pkt_type_eq(t, "LED-ON")){
        led_on();
        // reply identical to request
      } else if (ssrn_pkt_type_eq(t, "LED-OFF")){
        led_off();
        // reply identical to request
      } else if (ssrn_pkt_type_eq(t, "LED-BLINK")){
        // dedicate timer0 to LED blinking
        led_on();
        ssrn_set_timer(LED_TIMER, 10);
      } else if (ssrn_pkt_type_eq(t, "READ")){
        uint16_t value = analogRead(A0);
        
        // $SRN|+NNN|+NNN|NNNN|READ|
        // 0000000000111111111122222
        // 0123456789012345678901234
        event->packet->write_ptr = event->packet->data + 25;
        ssrn_pkt_ascii_uint32(event->packet, value, 0);
        ssrn_pkt_str(event->packet, "|*");
      } else {
        ssrn_unknown_packet(event);
      }
    }      
  }
}

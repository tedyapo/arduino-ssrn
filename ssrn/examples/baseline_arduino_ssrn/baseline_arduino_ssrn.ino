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

static uint8_t led_timer_set = 0;
static uint32_t led_off_time = 0;

void led_timer_processing()
{
  if (led_timer_set && millis() > led_off_time){
    led_off();
    led_timer_set = 0;
  }
}

void led_blink()
{
  led_on();
  led_timer_set = 1;
  led_off_time = millis() + 100;
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  ssrn_init(&altSerial);
  led_blink();
}

uint8_t ssrn_local_packet(ssrn_packet_t *p)
{
  uint8_t *t = &p->data[20];
  Serial.write('#');
  if (ssrn_pkt_type_eq(t, "LED-ON")){
    led_on();
    // reply identical to request
  } else if (ssrn_pkt_type_eq(t, "LED-OFF")){
    led_off();
    // reply identical to request
  } else if (ssrn_pkt_type_eq(t, "LED-BLINK")){
    led_blink();
    // reply identical to request
  } else {
    return 0;
  }
  return 1;
}

void loop()
{
  //ssrn_main_loop();
  while(1){
    ssrn_network();
    ssrn_processing();
    led_timer_processing();
  }
}

#include "Arduino.h"
#include "pinmux.inc"

const uint16_t P400_b[] = {
PIN_PWM|CHANNEL_6|PWM_CHANNEL_A|GPT_ODD_CFG,
PIN_SCL|CHANNEL_0,
PIN_INTERRUPT|CHANNEL_0,
SCI_CHANNEL|PIN_SCK|CHANNEL_0|SCI_EVEN_CFG|LAST_ITEM_GUARD
};
#define P400 P400_b

const uint16_t P408_b[] = {
PIN_PWM|CHANNEL_5|PWM_CHANNEL_B|GPT_ODD_CFG,
PIN_INTERRUPT|CHANNEL_7,
SCI_CHANNEL|PIN_CTS_RTS_SS|CHANNEL_1|SCI_EVEN_CFG,
SCI_CHANNEL|PIN_RX_MISO_SCL|CHANNEL_9|SCI_ODD_CFG|LAST_ITEM_GUARD
};
#define P408 P408_b

const uint16_t P014_b[] = {
PIN_DAC|CHANNEL_0,
PIN_ANALOG|CHANNEL_9|LAST_ITEM_GUARD
};
#define P014 P014_b

extern "C" const PinMuxCfg_t g_pin_cfg[] = { 
  /* DIGITAL OUTPUT */
  { BSP_IO_PORT_01_PIN_13,    P113   }, /* (0) D0 rev 0.1  */
  { BSP_IO_PORT_01_PIN_07,    P107   }, /* (1) D1 rev 0.1  */
  { BSP_IO_PORT_01_PIN_06,    P106   }, /* (2) D2 rev 0.1  */
  { BSP_IO_PORT_01_PIN_10,    P110   }, /* (3) D3 rev 0.1  */
  { BSP_IO_PORT_03_PIN_03,    P303   }, /* (4) D4 rev 0.1  */
  { BSP_IO_PORT_02_PIN_04,    P204   }, /* (5) D5 rev 0.1  */
  { BSP_IO_PORT_02_PIN_05,    P205   }, /* (6) D6 rev 0.1  */
  { BSP_IO_PORT_02_PIN_06,    P206   }, /* (7) D7 rev 0.1  */
  { BSP_IO_PORT_01_PIN_05,    P105   }, /* (8) D8 rev 0.1  */
  { BSP_IO_PORT_01_PIN_12,    P112   }, /* (9) D9 rev 0.1  */
  { BSP_IO_PORT_01_PIN_11,    P111   }, /* (10) D10 rev 0.1  */
  { BSP_IO_PORT_01_PIN_09,    P109   }, /* (11) D11 rev 0.1  */
  /* DIGITAL INPUT */
  { BSP_IO_PORT_04_PIN_09,    P409   }, /* (12) D12 rev 0.1  */
  { BSP_IO_PORT_04_PIN_08,    P408   }, /* (13) D13 rev 0.1  */
  /* ANALOG INPUT */
  { BSP_IO_PORT_00_PIN_00,    P000   }, /* (14) A0 rev 0.1  */
  { BSP_IO_PORT_00_PIN_01,    P001   }, /* (15) A1 rev 0.1  */ 
  { BSP_IO_PORT_00_PIN_02,    P002   }, /* (16) A2 rev 0.1  */
  { BSP_IO_PORT_00_PIN_03,    P003   }, /* (17) A3 rev 0.1  */
  { BSP_IO_PORT_00_PIN_04,    P004   }, /* (18) A4 rev 0.1  */
  { BSP_IO_PORT_00_PIN_12,    P012   }, /* (19) A5 rev 0.1  */
  { BSP_IO_PORT_00_PIN_13,    P013   }, /* (20) A6 rev 0.1  */
  { BSP_IO_PORT_00_PIN_14,    P014   }, /* (21) A7 rev 0.1  */
  { BSP_IO_PORT_00_PIN_15,    P015   }, /* (22) A8 rev 0.1  */
  { BSP_IO_PORT_01_PIN_01,    P100   }, /* (23) A9 rev 0.1  */
  { BSP_IO_PORT_05_PIN_02,    P502   }, /* (24) A10 rev 0.1  */
  { BSP_IO_PORT_05_PIN_01,    P501   }, /* (25) A11 rev 0.1  */
  { BSP_IO_PORT_05_PIN_00,    P500   }, /* (26) A12 rev 0.1  */
  { BSP_IO_PORT_01_PIN_01,    P101   }, /* (27) A13 rev 0.1  */
  { BSP_IO_PORT_01_PIN_02,    P102   }, /* (28) A14 rev 0.1  */
  { BSP_IO_PORT_01_PIN_03,    P103   }, /* (29) A15 rev 0.1  */
  /* DIGITAL CONTROLS */
  { BSP_IO_PORT_03_PIN_04,    P304   }, /* (30) DETECT_IN  rev 0.1  */
  { BSP_IO_PORT_01_PIN_04,    P104   }, /* (31) DETECT_OUT rev 0.1  */
  /* UART */
  { BSP_IO_PORT_03_PIN_02,    P302   }, /* (32) UART_TX  rev 0.1  */
  { BSP_IO_PORT_03_PIN_01,    P301   }, /* (33) UART_RX  rev 0.1  */
  /* I2C */
  { BSP_IO_PORT_04_PIN_01,    P401   }, /* (34) SDA  rev 0.1  */
  { BSP_IO_PORT_04_PIN_00,    P400   }, /* (35) SCL  rev 0.1  */
  
  /* CAN ? */

  

  
};

extern "C" const size_t g_pin_cfg_size = sizeof(g_pin_cfg);

int32_t getPinIndex(bsp_io_port_pin_t p) {
  int max_index = g_pin_cfg_size / sizeof(g_pin_cfg[0]);
  int rv = -1;
  for(int i = 0; i < max_index; i++) {
    if(g_pin_cfg[i].pin == p) {
      rv = i;
      break;
    }
  }
  return rv;
}

extern "C" {
    unsigned int PINCOUNT_fn() {
        return (sizeof(g_pin_cfg) / sizeof(g_pin_cfg[0]));
    }
}

#include "FspTimer.h"

void initVariant() {
  // bootloader configures LED_BUILTIN as PWM output, deconfigure it to avoid spurious signals
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);
  FspTimer::set_initial_timer_channel_as_pwm(GPT_TIMER, GET_CHANNEL(getPinCfgs(3, PIN_CFG_REQ_PWM)[0]));
  FspTimer::set_initial_timer_channel_as_pwm(GPT_TIMER, GET_CHANNEL(getPinCfgs(5, PIN_CFG_REQ_PWM)[0]));
  FspTimer::set_initial_timer_channel_as_pwm(GPT_TIMER, GET_CHANNEL(getPinCfgs(6, PIN_CFG_REQ_PWM)[0]));
  FspTimer::set_initial_timer_channel_as_pwm(GPT_TIMER, GET_CHANNEL(getPinCfgs(9, PIN_CFG_REQ_PWM)[0]));
  FspTimer::set_initial_timer_channel_as_pwm(GPT_TIMER, GET_CHANNEL(getPinCfgs(10, PIN_CFG_REQ_PWM)[0]));
  FspTimer::set_initial_timer_channel_as_pwm(GPT_TIMER, GET_CHANNEL(getPinCfgs(11, PIN_CFG_REQ_PWM)[0]));
}
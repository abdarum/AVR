# External Watchdog for Raspberry Pi

If inactivity of FORCE_WDG_IN pin is too long ATTiny13 will make hard reset of RPi. It is possible to force always on power output for RPi

## Pinout:

PB0 - ARD_RX_PIN - added in advance as possible uart

PB1 - ARD_TX_PIN - added in advance as possible uart

PB2 - WDG_INT_IN_PIN, pin reset countdown timer, PCINT2 interrupt

PB3 - Output of the power, when high state RPi is powered

PB4 - FORCE_WDG_IN pin, if it is activted, RPi power is always on

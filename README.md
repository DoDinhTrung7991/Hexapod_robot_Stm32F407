This project is for building Hexapod Robot
Robot Movement: stm32f407vet6 PWM
Self Balance: stm32f407vet6 I2C -- MPU-6050, stm32f407vet6 PWM
Robot Control:
    stm32f407vet6 UART -- HC-05 -- personal laptop (bluetoothctl, picocom, rfcomm)
    stm32f407vet6 I2C -- esp8266 -- personal laptop (picocom, esptool)
Timer generate pulse 50Hz trigger event read data and send control signal to motor and other MCU.

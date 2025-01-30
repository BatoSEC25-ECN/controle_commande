// // #include <SoftwareSerial.h>
// // SoftwareSerial Serial2(2, 3);

void setup()
{
  // Configuration de Serial2 avec 9600 bauds, 8 data bits, aucune parity, 1 stop bit
  Serial2.begin(9600, SERIAL_8N1);                 
  Serial.begin(115200);               
}

/***************************************
*  MAIN LOOP
***************************************/

void loop()
{
  if (Serial2.available()) {
    while(Serial2.available())  {
      Serial.write(Serial2.read());
    }
  }
}


// #include <hardware/serial.h>

// #define UART1_ID uart1
// #define UART1_PIN_TX 5 // Pin GP5
// #define UART1_PIN_RX 4 // Pin GP4

// void setup()
// {
//   // Configuration de l'UART1 avec 9600 bauds, 8 data bits, aucune parity, 1 stop bit
//   uart_init(UART1_ID, 9600);
//   gpio_set_function(UART1_PIN_TX, GPIO_FUNC_UART);
//   gpio_set_function(UART1_PIN_RX, GPIO_FUNC_UART);
//   gpio_set_pud(UART1_PIN_RX, true); // Activer la résistance de pull-up sur le pin RX

//   Serial.begin(115200); // Initialisation de la liaison série pour la communication avec l'ordinateur
// }

// /***************************************
// *  MAIN LOOP
// ***************************************/

// void loop()
// {
//   while (uart_is_readable(UART1_ID)) {
//     uint8_t caractere = uart_getchar(UART1_ID);
//     Serial.write(caractere);
//   }
// }

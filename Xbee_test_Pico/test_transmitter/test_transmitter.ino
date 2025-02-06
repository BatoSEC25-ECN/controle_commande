// #include <SoftwareSerial.h>
// SoftwareSerial Serial1(2, 3);
int count;

void setup()
{
    // Configuration de Serial1 avec 9600 bauds, 8 data bits, aucune parity, 1 stop bit
    Serial1.begin(9600, SERIAL_8N1);  
    Serial.begin(115200); // Port série principal (utilisé pour le debug)
    count = 0;
}

/***************************************
*  MAIN LOOP
***************************************/

void loop()
{
  String phrase = "Envoi du mot Sending ";
  phrase += count;             // Ajout de la valeur de count dans la phrase
  Serial.println(phrase);      // Affiche la phrase dans le terminal de debug
  Serial1.println(phrase);     // Envoie le message "coucou" via le port série Serial1
  count++;                     // Incrémente le compteur
  delay(5000);                  // Pause de 300 ms
}

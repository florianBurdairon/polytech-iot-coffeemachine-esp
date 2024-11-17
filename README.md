Files for esp_arduino
Libs included in the .ino file :
    - #include <ArduinoJson.h> with pluggin "ArduinoJson by Benoit Blanchon"
    - #include <Firebase.h> with pluggin "Firebase by Rupak Poddar"

Listes des composants arduino utilisés pour la cafetière :
- 3 relais pour les 3 boutons : on-off / café 1tasse / café 2tasses
- un câble avec 2 fils dénudé à une extrémité pour le branchement de l'ESP vers la led de la cafetière
- pour le pont diviseur de tension associé à la led : 2 résistances de 10 kOhms 
- un détecteur IR pour la détection de la tasse
- un capteur de niveau d'eau Grove
- un ESP32 avec le câble USB 
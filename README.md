# Projet IoT : Coffee Machine
## Partie microcontrôleur
### Liste de matériels utilisé
- Microcontrôleur ESP32 s3 NoPSRAM 8Mo Flash équipé BLE + Wifi
- 1 capteur de niveau d'eau Grove
- 3 relais Grove de 5V pour les boutons  : 
    - ON/OFF
    - verser 1 café
    - verser 2 cafés
- 1 capteur de proximité infrarouge (IR Distance Interrupter v1.2)
- un optocoupleur pour lire les états de la led intégré dans la cafétière (sincères remerciements à Mr Buathier qui a réalisé le montage)
- Cafetière Senso Philips HD7825 (apport personnel de BURDAIRON Florian)

### Librairies utilisées
Pour la programmation de l’ESP, il a été fait usage de l’IDE Arduino (développement C++) ainsi que les librairies suivantes : 
 - **ArduinoJson** télécharger avec le pluggin de Benoit Blanchon pour la sérialisation/désérialisation des données envoyées/reçues
 - **Firebase** télécharger avec le pluggin de Rupak Poddar pour la réception et l'envoi de données sur Firebase Realtime Database
 - **NTPClient** télécharger avec le pluggin de Fabrice Weinberg pour l'accès à un server NTP pour générer des timestamp en secondes
 - **WiFiUdp** utilisé avec le server NTP (inclut dans les librairies de base d'Arduino)
 - **BLEDevice**, **BLEServer** et **BLEUtils** pour la connection Bluetooth Low Energy (inclut dans les librairies de base d'Arduino)
 - **Wifi** pour la connection Wifi (inclut dans les librairies de base d'Arduino)
 - **EEPROM** pour l’écriture en dur sur la mémoire de l’ESP du SSID et du mot de passe utilisé pour la connexion wifi (inclut dans les librairies de base d'Arduino)
 - **Wire** pour permettre la communication avec les ports I2C (Inter-Integrated Circuit), dans notre cas pour l'utilisation du capteur de niveau d'eau (inclut dans les librairies de base d'Arduino)
 - **esp_mac** qui donne accès à l'obtention de diverses adresses mac, ici en l'occurence pour celle du Wifi
 - **Arduino** pour l’ensemble des différentes fonctions disponible avec Arduino (inclut dans les librairies de base d'Arduino)
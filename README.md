# La machine à café connectée "SenseCo"
### BLUMET Thomas et BURDAIRON Florian
### 5A-INFORMATIQUE Polytech Lyon 11/2024
## <ins> Description </ins>
> Envie d’un café ? Grâce à notre application Android, piloter directement votre machine à café Senseo. Lancer la préparation de votre café à distance en vous connectant, via un réseau Wifi disponible, à votre machine branchée sur secteur.

## 1. <ins>Préambule : fonctionnement classique de la machine à café Senseo </ins>

Bien qu’il existe différentes versions de la machine à café Senseo, il s’avère que le fonctionnement général est dans l'ensemble similaire. Pour avoir plus de spécifications sur les caractéristiques techniques, on peut se reporter aux liens suivants : 

- [Mode d'emploi Philips Senseo Original HD6553 (Français - 14 des pages)](https://www.modesdemploi.fr/philips/senseo-original-hd6553/mode-d-emploi?p=10)
- [Viva Café Machine à café à dosettes HD7825/68 | Philips](https://www.philips.fr/c-p/HD7825_68/viva-cafe-machine-a-cafe-a-dosettes#see-all-benefits)

Voici les principales étapes qui aboutissent à la préparation d’un ou deux cafés :

1. Allumage de la cafetière avec le bouton ON/OFF
2. La cafetière se met alors en chauffe : une led témoin clignotante à faible fréquence l’indique
3. Au bout de 2 min, la chauffe s’arrête : la led arrête de clignoter et reste allumée
4. L’appuie en fonction du choix de verser 1 ou 2 cafés sur l’un des boutons présents de part et d’autres du bouton ON/OFF permet alors de faire verser le café dans la tasse placée juste en-dessous du bec verseur. 
5. En cas d’inutilisation prolongée de 30 min, la machine s’éteint d’elle-même.
Attention néanmoins à certains cas bloquant lié à l’état de la led témoin. En effet, si la led se met à clignoter rapidement dès le début de l’allumage de la machine à café, c’est qu’il y a un problème pouvant être de nature différente : 
 - soit le bac est mal positionné dans son emplacement
 - soit il n’y a pas assez d’eau dans le bac lorsque l’on tente d’appuyer sur le versement du café
 - le dernier cas est le blocage du flotteur présent dans le bac pour mesurer le niveau d’eau. Dans notre projet cela n’arrive pas, étant donné que le flotteur n’est pas utilisé (non relié à la machine donc aucune erreur détectable) car défectueux. À la place nous avons mis un détecteur de niveau d’eau Grove relié à l’ESP qui, à un certain niveau minimum, va empêcher l’utilisateur de pouvoir cliquer sur un des boutons de l’application mobile. Ceci afin d’éviter que la pompe à eau de la machine à café n’aspire de l’air.

## 2. <ins>Matériels et technologies utilisées </ins>
Afin de réaliser ce projet s’insérant dans le cadre de l’IoT, il a été nécessaire de le séparer en 2 parties distinctes qui ont nécessité des besoins différents

### A. Côté hardware
Concernant le matériel utilisé pour réaliser notre machine connectée, il nous a fallu : 
 - une machine à café Senseo Philips HD7825 (apport personnel)
 - Micro-contrôleur de type ESP32 s3 avec 8Mo de mémoire flash équipé BLE + Wifi
 - Capteur de niveau d’eau Grove pour Arduino
 - Détecteur IR Distance Interrupter afin de détecter la présence de tasses en dessous du bec verseur
 - un optocoupleur (réalisé sur demande à Mr Buathier) pour permettre la lecture des états de la led intégrée à la machine à café, ceci afin de ne pas avoir de problème de couplage avec l’alimentation de la machine à café lors du setting de l’ESP avec nos PC.
 - 3 relais de 5V pour les boutons  : 
    - ON/OFF
    - verser 1 café
    - verser 2 cafés

Pour la programmation de l’ESP, il a été fait usage de l’IDE Arduino (codage basé sur C++) ainsi que les librairies suivantes : 
 - **#include <ArduinoJson.h>** téléchargé via le pluggin "ArduinoJson by Benoit Blanchon" pour la sérialisation/désérialisation des données envoyées/reçues
 - **#include <Firebase.h>** téléchargé via le pluggin "Firebase by Rupak Poddar" pour la réception des requêtes de l’application mobile vers la machine et l’envoi de données depuis cette dernière vers cet espace de stockage Cloud qui permet en même temps la gestion des API en tant que server distant
 - **#include <NTPClient.h>** téléchargé via le pluggin "NTPClient by Fabrice Weinberg" pour l'accès à un server NTP pour générer le timestamp en secondes écoulées depuis l'epoch du 01/01/1970
 - **#include <BLE.h>** pour la connection Bluetooth Low Energy
 - **#include <Wifi.h>** pour la connection Wifi
 - **#include <EEPROM.h>** pour l’écriture en dur sur la mémoire de l’ESP du SSID et du mot de passe utilisé pour le mode station Wifi qui connecte l’ESP au réseau Wifi identifié par le SSID
 - **#include <Arduino.h>** pour l’ensemble des différentes fonctions disponible avec Arduino

### B. Côté software

## 3. <ins>Scénarios d'utilisation de la SenseCo</ins>
### A. Cas de la toute première connexion
Il s'agit de l'étape de "setup". L'utilisateur souhaitant se servir un café doit au préalable branché l'ESP pour l'alimenter en éléctricité sur secteur. Via l'application Android, l'utilisateur va lancer le scan BLE afin de détecter la cafetière, plus précisémment l'ESP qui s'est lancé en mode BLE. L'utilisateur saisi le ssid et le mot de passe du réseau Wifi auquel l'ESP va se connecter. Afin de sauvegarder pour les connexions futures ces identifiants Wifi, l'ESP va les inscrire dans sa mémoire EEPROM. Une fois fait, l'ESP redémarre afin de se lancer cette fois en mode station Wifi et se connecte au réseau précisé par le ssid et le mot de passe que l'ESP récupère dans sa mémoire EEPROM. L'utilisateur doit alors basculer sa connexion Wifi sur le même réseau que l'ESP.  
### B. Cas hors première connexion


## 4. <ins>Diagrammes</ins>
### A. Architecture distribuée
### B. Cas d’utilisation
### C. Activité liée à la connexion BLE & Wifi
### D. Activité liée à la gestion des requêtes vers Firebase
### E. Classes utilisées dans le code de l'ESP

## 5. <ins>Aperçu visuel de l’application mobile</ins>

## 5. <ins>Bilan & conclusion</ins>
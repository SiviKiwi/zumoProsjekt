# Gruppe 8 Smartcityprosjekt

## Filstruktur 
#### Hovedfiler
Dette prosjektet er laget med PlatformIO. *platformio.ini* definerer avhengigheter og microkontrolleren som blir brukt.

Under src/ finnes hovedfila *Zumo32U4_program.cpp*.

I mappen lib/Zumo32U4\_bibliotek\_gruppe\_8/ ligger gruppas bibliotek.

#### Filene under other/ 
Mappen other inneholder koden til ladestasjon, Zumo-ens ESP, Arduino-skjerm og bane:
- *bane\_ESP32\_program.cpp* er fila til ESP-en som overvåker rundetider og sier i fra hvis Zumo-en passerer lysporten dens.
- *ladestasjon\_ESP32\_program.cpp* er fila til ESP-en som kontrolerer ladestasjonen.
- *screenReciver\_UNO\_program.cpp* er fila som styrer skjermen til ladestasjonen og Ultralydsensoren på ladestasjoen og banen.
- *zumo32U4\_ESP32\_Komm.cpp* er fila som sitter på Zumo_en og sender og motar data fra Node Red til Zumo.


## Bidragsytere
Laget av gruppe 8 BIELEKTRO V22: 
- Alvar(Executive Cloud Computing and Database Wizard⭐)
- Sivert(Lord Prime Minister and Supreme Archive Overseer) 
- Iver(Senior Integrated Circuit Developer God).

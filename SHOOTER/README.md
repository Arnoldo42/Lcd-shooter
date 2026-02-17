# LCD Shooter
Autor: Arnošt Hostomský

## Popis
LCD Shooter je arkádová hra pro Arduino s 16×2 LCD displejem.
Hráč ovládá loď joystickem, střílí nepřátele a bojuje s bossem.
Obtížnost se zvyšuje se skóre.

## Hardware
- Arduino (UNO / Nano / kompatibilní)
- LCD 16×2 s I2C (adresa 0x27)
- Analogový joystick
- Tlačítko (pin 32)
- Propojovací vodiče

## Ovládání
- Joystick nahoru/dolů – pohyb mezi řádky
- Tlačítko – střelba

## Funkce
- Nepřátelé s náhodným spawnem
- Boss po každých 20 bodech
- Více vzorů střel bosse
- Skóre a životy
- Animace výbuchu

## Spuštění
1. Otevři `lcd-shooter.ino` v Arduino IDE  
2. Připoj Arduino  
3. Nahraj kód  
4. Zapoj hardware podle schématu  

## Co bych mohl vylepšit
- Lepší AI bosse
- Více typů nepřátel
- Zvuk



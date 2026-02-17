# Peg – Terminal Roguelike (C)

Autor: Arnošt Hostomský

## Popis
Peg je jednoduchá textová hra napsaná v jazyce C, která běží přímo v terminálu.
Hráč se pohybuje po mapě 10×10, sbírá zlato a je pronásledován nepřáteli,
kteří se pohybují směrem k hráči.

Hra obsahuje systém skóre, zlata, jednoduchou AI nepřátel
a speciální schopnost **Shockwave**, která zničí nepřátele v okolí.

## Ovládání
| Klávesa | Akce |
|---------|------|
| W | Nahoru |
| S | Dolů |
| A | Doleva |
| D | Doprava |
| E | Shockwave (5 gold) |
| R | Restart |
| Q | Konec |

## Herní mechaniky
- Nepřátelé s AI (pohyb k hráči)
- Náhodný cíl (zlato)
- Skóre a gold
- Shockwave v okruhu 5×5

## Spuštění
```bash
gcc main.c -o peg
./peg

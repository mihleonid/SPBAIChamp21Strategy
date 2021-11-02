# The strategy to win!

*For authorised use only.*

## How to build
Using bare cmake:
```sh
mkdir build && cd build && cmake .. && make -j 32
```

## How to test
Run command for debug:
```sh
(./spb_ai_champ localhost 31002&);(./spb_ai_champ localhost 31003&);(gdb ./spb_ai_champ;killall spb_ai_champ)
```
Run command for release:
```sh
(./spb_ai_champ localhost 31002&);(./spb_ai_champ localhost 31003&);(./spb_ai_champ;killall spb_ai_champ)
```

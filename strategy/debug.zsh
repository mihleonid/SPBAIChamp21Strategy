#!/usr/bin/zsh
(./spb_ai_champ localhost 31002&);(./spb_ai_champ localhost 31003&);(gdb ./spb_ai_champ;killall spb_ai_champ)

#!/bin/sh
#an example of a simple script.  This will only work if the called program has no sound itself and doesn't mess with the cpu clock...
cp /mnt/sd/autorun.gpu /mnt/sd/autorun.bak
cp /mnt/sd/ommpc2x/scripts/autorun.gpu /mnt/sd/

# Run game
cd /mnt/sd/Games/
./somegame.gpe

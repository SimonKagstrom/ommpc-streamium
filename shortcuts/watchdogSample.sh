#!/bin/sh
# replace autorun in order to return
# this obviously could end up with a bogus autorun in place if the gp2x dies
# while running the launched program...
cp /mnt/sd/autorun.gpu /mnt/sd/autorun.bak
cp /mnt/sd/ommpc2x/scripts/autorun.gpu /mnt/sd/

# Set volume(optional) to and run watchdog
cd /mnt/sd/ommpc2x/
./mpc/mpc volume 30

./watchdog&

# Run game
cd /mnt/sd/Games/greader
./Greader2x.gpe


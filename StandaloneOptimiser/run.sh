#!/bin/bash
echo $1
python3 basic.py $1
cat $1.gcode > $1_new.gcode
#echo "\nG1 Z5" >> $1_new.gcode
echo "\nG1 Z5;" >> $1_new.gcode
echo "G1 Z0;" >> $1_new.gcode
echo "G1 Z5;" >> $1_new.gcode
bin/Debug/CNC $1_new.gcode $2 $3 $1_new.nc

#!/bin/bash
echo $1
#python3 basic.py
cat $1.gcode > $1_new.gcode
echo "G1 Z5" >> $1_new.gcode
echo "G1 Z5" >> $1_new.gcode
echo "G1 Z0" >> $1_new.gcode
echo "G1 Z5" >> $1_new.gcode
bin/Debug/CNC $1_new.gcode 1 100000 $1_new.nc

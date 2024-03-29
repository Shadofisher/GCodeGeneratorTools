from svg_to_gcode.svg_parser import parse_file
from svg_to_gcode.compiler import Compiler, interfaces
import sys

print("Name of drwaing file: {}".format(sys.argv[1]))

# Instantiate a compiler, specifying the interface type and the speed at which the tool should move. pass_depth controls
# how far down the tool moves after every pass. Set it to 0 if your machine does not support Z axis movement.
gcode_compiler = Compiler(interfaces.Gcode, movement_speed=1000, cutting_speed=1000, pass_depth=1)

input_file = sys.argv[1] + ".svg"
output_file = sys.argv[1] + ".gcode"
print(input_file," ",output_file)

#curves = parse_file("drawing.svg") # Parse an svg file into geometric curves
curves = parse_file(input_file) # Parse an svg file into geometric curves

gcode_compiler.append_curves(curves) 
#gcode_compiler.compile_to_file("drawing.gcode", passes=2)
gcode_compiler.compile_to_file(output_file, passes=2)

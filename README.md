# GCodeGeneratorTools

The following set of tools originates from a need to generate GCode files from images that do not always contain closed loops.
This would be suitable for laser cutters,drawing applications or engraving.

Couldn't find tools to suit my needs as most of them only worked with oulines as well as closed loops.

The first application generates a generic Gcode file form an image (.data format from gimp) and takses the following arguments:

gcodegenerate XPixels YPixel Xmm Ymm Inputfile OutpotFile

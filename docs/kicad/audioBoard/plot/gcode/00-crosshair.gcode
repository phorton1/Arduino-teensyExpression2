G0 Z20
G21
G90
G94
F50.00
M03 S45

G00 Z3.0000

; draw a vertical line at 0,0

G0 X0 Y0
G4 P1
G01 Z-0.2500
g0 Y3
G00 Z3.0000

; draw a vertical line at 65mm

G0 X65 Y0
G4 P1
G01 Z-0.2500
g0 Y3
G00 Z3.0000

; draw the cross hairs

G00 X3 Y3
G4 P1
G01 Z-0.2500
g0 x1.5
g0 x4.5
g0 x3
g0 y1.5
g0 y4.5
g0 y3
G00 Z3.0000


G00 X0Y0
M05
G0 Z20
G0 X0 Y100
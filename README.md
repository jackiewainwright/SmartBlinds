# SmartBlinds
A senior design project that has a mobile application that sends user preferences (time, light, temp) to a micro controller system. The micro controller system tells the blinds whether to go up or down based off input from the sensor subsystem. The micro controller controls the motors which raises blinds up/down and opens slats.


# Motors
File descriptions:
- motorhw: is an Eagle project that contains the PCB I printed. The PCB was only half used since it was printed before we switched motors. The half containing the Nano and transceiver is used, while the half containing motor drivers is unused. Pictures of the schematic and layouts are in motorhw\_schematic.png and motorhw\_layout.png. 
- hardware\_photo.jpg and switch\_photo.jpg: Pictures of the final products.
- motor\_demonstration.mov: Video of the blinds being lifted, lowered and rotated to their limits. 
- logic\_diagram.png and power\_diagram.png: Basic schematics made in Fritzing. These are kind of low-quality because I couldn't find files for every part. In the logic diagram, two jacks are shown; the top supplies 12V and the bottom supplies 5V. In the drivers, A1,A2,B1,B2 aren't connected; they each should be connected to a motor. In the power diagram, imagine the battery is a Li-Ion, replace the TP4056 with a TP5410, imagine it's on a protoboard instead of breadboard. 
- keiths\_program.ino: Program used by Keith on the nano. 
- my\_program.ino: Program used by me to develop and test the motor subsystem.


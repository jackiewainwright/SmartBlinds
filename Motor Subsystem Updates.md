9-25

Rough PCB design finished. However, we've decided to get a protoboard version working. For the final product, a protoboard will work. The only problem is reduced technical merit. 

9-30 

Finished proto board.

This proto board is split into 3 boards: nano board, motor board, and power board. 
* Nano board contains the nano, transceiver, multiple resistors for switches, and two transistors. 
* Motor board contains two motor drivers and three header rows providing an interface with the motors. 
* Power board contains the battery charger chip. It has headers connecting the battery, solar panel, and multiple power and ground connections. 

The nano board isn't soldered; it can easily, reliably, and more temporarily be connected with jumper wires. In the future, it may be soldered to become more reliable.

The motor board is fully soldered and tested, and it works.

The power board has the solar and battery components soldered. However, it doesn't have USB power connections soldered; this part may need minor design revisions first. 

Right now, we have a very solid option in the proto boards. There's no real need to get a PCB, except to increase technical merit and make it closer to a marketable product. 


10-14
We decided to change things up. The 28BYJs are pretty weak and lifting the blinds is too big a job for them. We bought NEMA-17s and drivers, and we'll use them to lift and probably rotate the blinds. 

10-22
Obtained the PCB finally. I got two copies in case one failed. The traces of one all work properly, so it seems like it'll work. I will solder female headers to attach the Nano and nRF transceiver. Currently I need surface-mount resistors, transistors, and capacitors, but if we go forward with the new motors the transistors and caps may not be necessary. 
If we replace the 28BYJ-48s with NEMA-17s, the right half of the board will be unnecessary, so I'll cut it off. 

10-27
Soldered power board together. 
Due to a miscommunication we didn't have a voltage booster chip. I bought a new battery-charging chip (TP5410) that boosts voltage as well. 

10-28
Tested the motor drivers. They can, in fact, run the motors. At max current, the drivers pull 12V 410mA, then deliver to the motor 1.05A per-phase. 

10-29
Soldered two motor drivers onto a protoboard. Put one motor on blinds, then tested whether it can lift half the blinds (successfully). Made video of motor lifting its share of blinds. 
The protoboard will be part of the final product. It'll have many jumpers connected to the power board and Arduino, but that's okay. The jumpers should be orderly enough to be more reliable than a breadboard. 

10-30
In lab, we drilled holes in the frame. One string is used for each side to lift the blinds, and they go through the holes to the top of the frame, where motors lift them. One shaft adaptor is put on each motor to attach the string.
It turned out we're missing a shaft adaptor, so we couldn't test rotation. We know it has enough torque because the 28BYJs did, and we'll implement this later. 

11-03
I accidentally burned out the battery charging board in a puff of smoke, so that's fun. However, I managed to test it enough to determine that the voltage boosting will be good enough (4.7V at 50mA), and it does charge the battery. I bought a new one and it'll ship on the 5th. 

11-19
The A4988 motor drivers burn out really easily, and I've had to buy 5 new ones. Made a new protoboard, fitted with female headers so I can add and replace driver boards whenever they burn out again.

11-24
Keith's motor program is complete and validated, and videos were made. The rotation is calibrated to within 5°.

11-25
The demo is tomorrow, and every part of the motor subsystem works.

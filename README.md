# Remo-te


## Remo-te?
YES! This project aims to build an automated machine for tea brewing!<br>
Imagine jumping into your car after work and having to drive for quite some time. Wouldn't be great if once you get home a hot cup of freshly brewed tea is waiting for you just at the perfect temperature for you to not burn your tongue? And what if this was just a tap away from your finger?
<br>
<br>
Now that's what we are talking about! and you ca find beelow all the documentation that i wrote during the process of making this machine. Take it not as a tutorial but more as an inspiration for your very own version.<br>
<br>

## Materials
### for the electronic part we will need
- (optional) Arduino nano for code debugging
- ESP 8266 or Arduino nano IoT for cloud connection
- 2 relays 5Vdc 10A
- 2 Micro Servo 9g MS18
- one L298N stepper motor module
- Power supply 12v 3A
- A generic 12V pump and a small pipe
- A stepper motor 17OM-J349-G2VS Minabea Motor
- A Breadboard to hold all the components
- A triple plug to join al the power cables into one plug to be attached to the house power grid.

  
### for the mechanical part we will need
- To print all the .stl files in the "prints" folder
- A water tank to hold the water
- An electrical boiler with auto-shut-off function when boiling point is reached
- A plastic pulley
- A box to hold the circuitry
- A thick piece of twine
- (optional) A bearing to make the twine flow easily

## Circuit
In the folder "circuit" can be seen the history of the circuits. The last version is this one
circuit_history/circuit_Remo_te_v4_squares.png


The board is powered from the USB port. (in the picture is shown an arduino nano but i wil use an ESP 8266)
the pin involved are:
- 2,3,4,5 respectively for the first coil (2,3) and the second coil (4,5) of the stepper motor
- 6 controls the servo motor which turns the boiler power on
- 7 controls the power supply relay
- 8 controls the "power router" relay which choses between pump and stepper motor 
- 9 controls the servo motor which starts the falling process
this is a photo of my setup.

## Code
The most updated code is the file remo_test_motor_tuning.ino
It provides a series of functions callable from serial communication and allows to test all the functionalities one by one. The code is commented and i think it is pretty easy to understand


## Mechanical building
The assembling process is simple and easily customizable depending on your setting. the only few important rules are 
- the water tank should lay below the boiler otherwise the pump will let the water flow into the boiler
- the circuit board should rest in a place relatively safe from water drops
- the motor should stay in the specific housing in the printed part "remo_te dock.stl"
- the pulley should be attached to the motor 

## Arduino cloud


# Remo-te


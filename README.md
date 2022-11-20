# LuckArms

This repository contains both the 3D models and the code which I am using for my Luck cosplay with the lightning armor. It is worth noting that the parts are pretty big, but I sized them to fit me specifically so it may require some modifications to make them fit you. 

<b>What is in the REPO</b>

Models:

In this repository, I have the third (most recent) version of my 3D models, as well as two arduino files with my code. The HandAgainV3g file has the fingers attached to the hand, while the fingerlesshand file only has the thumb attached. That folder also contains each of the fingers separately for use with the fingerless hand (although the models do not have a joint for them to be attached to, so I would just use tape or glue).

The leg models from V2 are also posted here - I did not remodel them as part of V3 so V2 is the latest version of those. There are uploads of those files both with and without holes for the LEDs (version 2 had holes that the LEDs could fit into) and like the arms there are minimal joints. The bottom part of the leg-top model is designed to slide into the slit at the top of the shoe model and the heel of the shoe has shapes which help hold it in place with the rest of the shoe (particularly when the leg top is also in the slot). 

Code: 

The lightningcode5 file is meant to be used with only one programmable LED strip. The onestripcondensed file is meant to be used in combination with a demultiplexor to run several (up to 8) LED strips off of one arduino.

  <b>What I did</b>

Printed using: Artillery Sidewinder x2 with this filament: https://www.amazon.com/dp/B07N1FD3GL?psc=1&ref=ppx_yo2ov_dt_b_product_details.

I use the Cura slicer, and would recommend using the "tree" support structure as opposed to the default since it makes the supports much easier to remove once the model is done printing.

For the circuits, I am using an Arduino Nano powered by a basic portable cell phone charger and several programmable LED strips for the lights. The lights are programmed using the FastLED library. I am also using a demultiplexor (https://www.amazon.com/dp/B016G4MQH8?psc=1&ref=ppx_yo2ov_dt_b_product_details) so I can use bit-packing to run more LEDs than would otherwise be possible given the limited memory of the arduino nano. 


Hope this helps!

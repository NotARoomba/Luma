---
title: "Luma"
author: "Nathan Alspaugh (NotARoomba)"
description: "A minecraft inspired lantern controllable by a custom app."
created_at: "2025-07-16"
time_spent: ~21 Hours
---

# July 16: Start and Schematic Design

I was scrolling TikTok out of boredom when I found a minecraft lantern that someone was selling and that could be controlled by an IR remote and in that moment I thought of a way to improve it so that's when Luma was started. I wanted to make a minecraft lantern that can be controlld by an app/bluetooth and that you can give to your friends to communicate through lights.

To do this I would need a chip that has bluetooth/wifi (I have designd my own before and would like to save myself the time lol) so I ended up using an ESP32-WROOM-32E(8MB) along with 9 LED's in a 3x3 matrix. I planned on having it be charged through USB-C and connect to a LiPo battery. I would make the case 3D-printeable and inclue acrylic panels with parchment paper to create a sort of "opacity". After having a clear idea of what I wanted to make, I started wiring up the components.

![alt text](/assets/schem_usb.png)

I started off with the ESP32 and the USB-C circuit bassed off of the application note before going to sleep.

**Time Spent:** 4 Hours

# July 17: Finish Schematic and PCB

After making part of the schematic I started work on the battery charging circuits, reusing a design that I implmented in my other project (Linea). Then I added the LED's and connected them to the ESP32.

![alt text](/assets/new_schem.png)

After that I started wiring and grouping the components to organize the wiring and make it easier. I decided to make 2 ground planes for the fun of it and wire all the components on one side and then the LED's on the other. I planned on the board being on the bottom or top of the case so the LED's could illuminate the area well.

After a while I ended up with this, but then didn't like how the board was not modular or that I couldn't change the number of LED's (just in case) so I decided to remake the board out of concerns that the 9 LED's wouldn't be bright enough.

![alt text](/assets/schem_header.png)

![alt text](/assets/old_pcb.png)

**Time Spent:** 7 Hours

# July 18: Start CAD

I took a break from the schematics and decided to work on the case.
![alt text](/assets/cad_base.png)

I wanted the case to be modular so I created some insets in the base to add the walls.

![alt text](/assets/cad_walls.png)

I then created the walls of the lantern with more insets for the acrylic panels of the case. I decided to use acrylic panels of 2 mm so that the light from inside could get through.

![alt text](/assets/cad_finish.png)

After creating the walls, I added the top (with mroe insets) and then started working on the chains. I also wanted them to be modular and easy to asssemble so I added more inserts to be able to glue them later.

![alt text](/assets/cad_chain.png)

**Time Spent:** 5 Hours

# July 19-20: Finish New PCB and Code

After starting again, I removed all of the LED's and added a header to be able to solder a strip of LED's to allow for a more modular approach.

![alt text](/assets/new_pcb.png)

I also created the code using C++ (arduino). I added a test code to get the lights working as I plan to add the app functionality when I build the lamp.

**Time Spent:** 5 Hours

# RC Boat Thingy - an RC boat that goes WROOM WROOM   

<img width="813" height="411" alt="title_pic" src="https://github.com/user-attachments/assets/2ace20f8-e052-46fb-adb4-74ec5545da06" />

A Boat controller with an **IMU**, **Gyro** and **GPS** packed into a 3D-printed hull, powering an **ESC** and a **BLDC** motor. Along with it is a small controller that controls the boat over about a 2 km range.


# How does it work?

There's a simple RP2040-based controller IC that connects to all the electronics in the boat's hull. The controller then remotely connects to the receiver  and allows you to control its speed, direction, etc. When powered on and set up properly, the system works together to drive the propeller and move the boat across the water. 


# Why did I make it?

This was inspired by a boat my friends and I built back in the junior year of high school. While the initial idea we had back then worked in theory, the actual execution was.... Let's just say less successful. Because the boat we made in 2023~2024 had no Gyro with just a lacklustre IMU, its stabilisation was non-existent, with it doing WROOM WROOM underwater (thank god transparent nail polish is waterproof)

"RC Boat Thingy" is basically our second attempt at doing it properly, but approaching it with actual electronics and an actual control system, so it's actually controllable and usable without turning it into a submarine

# Components Required for the Build


| No. | Quantity | Description                                              | Supplier   | Supplier Part | Product Link                                                                           | Cost   | Total (USD) |
| --- | -------- | -------------------------------------------------------- | ---------- | ------------- | -------------------------------------------------------------------------------------- | ------ | ----------- |
| 1   | 6        | 1x1 2.54mm pins for testing and troubleshooting          | LCSC       | C7501259      | https://www.lcsc.com/product-detail/C7501259.html                                      | 0.015  | 0.09        |
| 2   | 1        | 1x6 2.54mm pins for connecting the ESC to the Controller | LCSC       | C34498        | https://www.lcsc.com/product-detail/C34498.html                                        | 0.51   | 0.51        |
| 3   | 1        | 1x4 2.54mm pins for SWD Branch out Header                | LCSC       | C2935929      | https://www.lcsc.com/product-detail/C2935929.html                                      | 0.035  | 0.035       |
| 4   | 2        | BOOT and RESET buttons for the RP2040                    | LCSC       | C918859       | https://www.lcsc.com/product-detail/C918859.html                                       | 0.2275 | 0.455       |
| 5   | 8        | 2.2uF 0603 Capacitors                                    | LCSC       | C23630        | https://www.lcsc.com/product-detail/C23630.html                                        | 0.0093 | 0.0744      |
| 6   | 25       | 100nF 0603 Capacitors                                    | LCSC       | C191538       | https://www.lcsc.com/product-detail/C191538.html                                       | 0.0122 | 0.305       |
| 7   | 2        | 15pF 0603 Capacitor                                      | LCSC       | C107037       | https://www.lcsc.com/product-detail/C107037.html                                       | 0.0046 | 0.0092      |
| 8   | 1        | Antenna Connector for GPS Module                         | LCSC       | C88374        | https://www.lcsc.com/product-detail/C88374.html                                        | 0.108  | 0.108       |
| 9   | 1        | 33nH Inductor for GPS Antenna                            | LCSC       | C90209        | https://www.lcsc.com/product-detail/C90209.html                                        | 0.0134 | 0.0134      |
| 10  | 1        | LDO for RP2040 Battery to MC Power Rail                  | LCSC       | C82942        | https://www.lcsc.com/product-detail/C82942.html                                        | 0.51   | 0.51        |
| 11  | 1        | 0603 Red LED                                             | LCSC       | C84263        | https://www.lcsc.com/product-detail/C84263.html                                        | 0.0095 | 0.0095      |
| 12  | 1        | 0603 Blue LED                                            | LCSC       | C965799       | https://www.lcsc.com/product-detail/C965799.html                                       | 0.0035 | 0.0035      |
| 13  | 1        | Diode for LDO current Regulation                         | LCSC       | C20512        | https://www.lcsc.com/product-detail/C20512.html                                        | 0.0614 | 0.0614      |
| 14  | 5        | 1KΩ 0603 Resistor                                        | LCSC       | C22548        | https://www.lcsc.com/product-detail/C22548.html                                        | 0.0016 | 0.008       |
| 15  | 1        | 470Ω 0603 Resistor                                       | LCSC       | C114669       | https://www.lcsc.com/product-detail/C114669.html                                       | 0.0018 | 0.0018      |
| 16  | 1        | 200KΩ 0603 Resistor                                      | LCSC       | C105574       | https://www.lcsc.com/product-detail/C105574.html                                       | 0.0016 | 0.0016      |
| 17  | 1        | 100KΩ 0603 Resistor                                      | LCSC       | C14675        | https://www.lcsc.com/product-detail/C14675.html                                        | 0.0014 | 0.0014      |
| 18  | 1        | 200Ω 0603 Resistor                                       | LCSC       | C114664       | https://www.lcsc.com/product-detail/C114664.html                                       | 0.0016 | 0.0016      |
| 19  | 1        | 1Ω 0603 Resistor                                         | LCSC       | C112305       | https://www.lcsc.com/product-detail/C112305.html                                       | 0.0042 | 0.0042      |
| 20  | 6        | 10KΩ 0603 Resistor                                       | LCSC       | C965184       | https://www.lcsc.com/product-detail/C965184.html                                       | 0.0032 | 0.0192      |
| 21  | 2        | 27Ω 0603 Resistor                                        | LCSC       | C25190        | https://www.lcsc.com/product-detail/C25190.html                                        | 0.0015 | 0.003       |
| 22  | 4        | 5.1KΩ 0603 Resistor                                      | LCSC       | C23186        | https://www.lcsc.com/product-detail/C23186.html                                        | 0.0016 | 0.0064      |
| 23  | 2        | 4.7KΩ 0603 Resistor                                      | LCSC       | C99782        | https://www.lcsc.com/product-detail/C99782.html                                        | 0.0015 | 0.003       |
| 24  | 1        | RP2040 Microcontroller                                   | LCSC       | C2040         | https://www.lcsc.com/product-detail/C2040.html                                         | 0.95   | 0.95        |
| 25  | 1        | W25Q128JVSIQ Flash Memory                                | LCSC       | C97521        | https://www.lcsc.com/product-detail/C97521.html                                        | 2.64   | 2.64        |
| 26  | 1        | 12MHz 10pF Crystal Oscillator                            | LCSC       | C5288803      | https://www.lcsc.com/product-detail/C5288803.html                                      | 0.59   | 0.59        |
| 27  | 1        | BMP388 Digital Pressure Sensor                           | LCSC       | C779278       | https://www.lcsc.com/product-detail/C779278.html                                       | 2.37   | 2.37        |
| 28  | 1        | M216Z-D80 GPS Module                                     | LCSC       | C49286686     | https://www.lcsc.com/product-detail/C49286686.html                                     | 5.09   | 5.09        |
| 29  | 1        | USB C connector                                          | LCSC       | C2765186      | https://www.lcsc.com/product-detail/C2765186.html                                      | 0.0704 | 0.0704      |
| 30  | 1        | RICHTEK RT9080-33GJ5(REGULARTOR FOR USB C                | LCSC       | C841192       | https://www.lcsc.com/product-detail/C841192.html                                       | 0.1182 | 0.1182      |
| 31  | 1        | RED LED 0805                                             | LCSC       | C84256        | https://www.lcsc.com/product-detail/C84256.html                                        | 0.0134 | 0.0134      |
| 32  | 2        | BOOT AND RESET SWITCHES FOR THE CONTOLER                 | LCSC       | C720477       | https://www.lcsc.com/product-detail/C720477.html                                       | 0.054  | 0.108       |
| 33  | 1        | FLIP SWITCHES FOR CONTOLER                               | LCSC       | C908284       | https://www.lcsc.com/product-detail/C908284.html                                       | 1.664  | 1.664       |
| 34  | 3        | TACT BUTTON FOR CONTOLER                                 | LCSC       | C455118       | https://www.lcsc.com/product-detail/C455118.html                                       | 0.0581 | 0.1743      |
| 35  | 1        | CAPASITOR FOR CONTORLER                                  | LCSC       | C72522        | https://www.lcsc.com/product-detail/C72522.html                                        | 0.0221 | 0.0221      |
| 36  | 3        | LED for controller                                       | LCSC       | C965560       | https://www.lcsc.com/product-detail/C965560.html                                       | 0.55   | 1.65        |
| 37  | 3        | USB C backwards current diode                            | LCSC       | C41409280     | https://www.lcsc.com/product-detail/C41409280.html                                     | 0.0243 | 0.0729      |
| 38  | 2        | Joystick                                                 | ElectroPi  | EPI7882       | https://www.electropi.in/dual-axis-xy-joystick-module                                  | 0.29   | 0.58        |
| 39  | 2        | NRF24L01 Receiver and Transmitter                        | makerBazar | SKU:422942410 | https://makerbazar.in/products/2-4ghz-nrf24l01-pa-lna-sma-wireless-transceiver-antenna | 1.54   | 3.08        |
| 40  | 1        | IMU & Accelerometer Sensor                               | robu       | SKU: R184320  | https://robu.in/product/icm-20948-tdk-invensense                                       | 14.19  | 14.19       |
| 41  | 1        | Boat Controller PCB                                      | JLCPCB     | NA            | https://cart.jlcpcb.com/quote                                                          | 1.3    | 1.3         |
| 42  | 1        | Transmitter PCB                                          | JLCPCB     | NA            | https://cart.jlcpcb.com/quote                                                          | 1.3    | 1.3         |
|     |          |                                                          |            |               |                                                                                        | Total  | 38.2179     |




You can find the BOM in the repo as well!


# PCB and Schematic

I designed it in EasyEDA Pro. You can find the Gerber files in the repo. The schematic is:

The primary Boat Controller has the following schematics and PCBs:

<img width="532" height="334" alt="schematic_board" src="https://github.com/user-attachments/assets/64975151-196c-4880-a5f0-1722ec51aabd" />
<img width="453" height="208" alt="pcb_board" src="https://github.com/user-attachments/assets/c26b6122-0452-4bae-9049-61cb018bf98c" />

The Transmitter has the following schematics and PCBs:

<img width="430" height="284" alt="schematic_controller" src="https://github.com/user-attachments/assets/793d7845-4a37-4dd3-9cc5-7f3e58fb9b67" />
<img width="326" height="371" alt="pcb_controller" src="https://github.com/user-attachments/assets/23bf796c-9c8e-4433-9707-a0da878c2a22" />

# Case

The Control Board is meant to be a fully tunable board with different motor control profiles based on the boat's dimensions. 

The transmitter needs its own case with its CAD files mentioned in the CAD_Files folder. 
The top and bottom view of the total transmitter assembly is attached below. 

<img width="301" height="325" alt="controller_front" src="https://github.com/user-attachments/assets/5e283015-d7ca-42b5-b24e-8e260e19e382" />

<img width="260" height="297" alt="controller_back" src="https://github.com/user-attachments/assets/5a0730b5-de66-40df-96e2-8636e9c09bdc" />

<img width="301" height="324" alt="controller_internal" src="https://github.com/user-attachments/assets/a88e7f3f-783c-4881-809a-43f0934d9676" />

The case features a snap-fit design, so you just have to like align the top and bottom parts of the 3D print together and press them together. You only need screws to screw the PCB into place with the bottom part.

<img width="393" height="343" alt="controller_screw_assembly" src="https://github.com/user-attachments/assets/9f83e61d-9928-438e-aa38-7f1af8f310fa" />


# Assembly

Please do note that these assembly instructions are not final and can change. I've yet to build it, and when I do, I'll update this section to be more accurate. In the meanwhile, don't be afraid to change things up or make use of the several exposes GPIO pins/testing pad to make any modifications you would like to make. 

Note : Soldering Instructions are SUGGESTIONS and are typically personal preference , If your used to doing things a certain way , do not change it. I personally tend to do SMD components close to THT components first , followed by the THT components . 

1. Get the parts and PCB (It would be preferred if you get an SMD stencil to make assembly much easier using a hot air or an hotplate )
2. Start my soldering the components near the USB C connector (the diodes and decoupling capacitors) , Specifically for the boat controller you can solder R19, R18, C1, C2, C3, C4 and LDO1 followed by C1, D1, D2, D3, R1, R2 ,U4, C2, R5 on the transmitter PCB. You can solder the USB C connector now on both the PCB's 
3. Solder the RP2040 and ESP32 IC's on each of their respective boards, use a hot air or hotplate setup to reflow the solder with relative ease and BE GENEROUS WITH THE GND PAD SOLDER. 
4. From here on out, the order doesn't really matter as long as you can solder all of the components at their respective places as the BOM instructs you to do. All parts for this reason are 0603 or higher to make hand soldering a viable plan if your brave enough to approach doing so. Also , be generous with flux while soldering, it literally cant hurt (most of the times at least)
5. Inspect everything with your eyes , looking at every component individually. Its recommended to use an Multimeter or an LCR meter to just sanity check all of the values and check for continuity on all of your passives.
6. Use some Isopropanol to clean the Flux and solder residue off the board and connect it to a power source to make sure it doesn't explode.
7. Connect both of the boards to 5V of power and use a multimeter to check for signal continuity and voltage stability on the 5V , 3.3V , 1.1V test pads. 

# Hardware at a Glance 

|            | Transmitter                                    | Boat Controller                           |
| ---------- | ---------------------------------------------- | ----------------------------------------- |
| MCU/Core   | ESP32-S3 class board, Arduino-ESP32 core       | RP2040, Earle Philhower arduino-pico core |
| Radio      | nRF24L01(+) on SPI                             | nRF24L01(+) on SPI                        |
| Role       | PTX (sends, stopListening())                   | PRX (listens, startListening())           |
| Inputs     | 2 joysticks, 2 push buttons, joystick switches | -                                         |
| Outputs    | 3× WS2812B status LEDs                         | rudder servo + ESC/BLDC motor             |
| Sensors    | -                                              | ICM-20948 IMU, BMP388 baro, GPS           |
| USB Serial | 115200 baud (status log)                       | 115200 baud (telemetry + serial commands) |

# Setting up the Boat Controller Software

Boat controller software setup :- 
1. Plug the Boat Controller board into your device first and hopefully it should show up as a "RPI-RP2 USB Device" or just another "USB Host Device". Either ways, once you've validated that your PC at least recognizes the board , head onto the next step 
2. Make sure to install all of the dependencies for the software mentioned in the dependencies.txt file to ensure the code runs and doesn't break down mid compilation
3. Reconnect the boat controller with the BOOT button held down to enter in BOOTSEL mode
4. Your computer should recognize the board as a USB mass storage device and will typically be named "RPI-RP2" 
5. All of the code being uploaded to the boat controller will be done so through the Arduino IDE, henceforth please make sure to add the RP2040 as an additional board. (go to File -> Preferences -> Additional Board Manager URLs -> Add this URL"[ ](https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json)")
6. Go to Boards manager and search for RP2040, if there's any compatibility issues than install "Raspberry Pi Pico/RP2040/RP2350" by Earle F. Philhower
7. Before uploading the sketch provided in the Boat-Controller Firmware, configure your boat profile based on the  size and dimensions of your boat chassis. Run the python script the tools folder and take the files you get from that and dump them into the Firmware folder before proceeding to upload it to the board. 
8. Change the default nRF24L01 pin number to the GPIO's you have connected your module to and update the code accordingly
9. Upload the code with all of the changes accordingly and proceed to setup the transmitter.

# Transmitter pin map 
| Pin Name      | name in code | GPIO | Notes                                      |
|---------------|--------------|------|--------------------------------------------|
| nRF CE        | PIN_NRF_CE   | 4    |                                            |
| nRF CSN       | PIN_NRF_CSN  | 5    |                                            |
| nRF MISO      | PIN_NRF_MISO | 6    |                                            |
| nRF SCK       | PIN_NRF_SCK  | 12   |                                            |
| nRF MOSI      | PIN_NRF_MOSI | 13   |                                            |
| nRF IRQ       | PIN_NRF_IRQ  | 14   | input only; not used by the polling driver |
| Joystick 1 X  | PIN_VRX1     | 1    |                                            |
| Joystick 1 Y  | PIN_VRY1     | 2    | throttle                                   |
| Joystick 1 SW | PIN_SW1      | 21   | latching KILL toggle                       |
| Joystick 2 X  | PIN_VRX2     | 3    | steering                                   |
| Joystick 2 Y  | PIN_VRY2     | 36   | declared, currently unused                 |
| Joystick 2 SW | PIN_SW2      | 11   | currently unused                           |
| Status LEDs   | PIN_LED      | 48   | 3× WS2812B (NUM_LED = 3)                   |

# Setting up the Transmitter Software

Boat controller software setup :- 
1. Plug the Boat Controller board into your device first and hopefully it should show up as a "Silicon Labs CP210x USB to UART Bridge" or "FTDI USB Serial Converter". Either ways, once you've validated that your PC at least recognizes the board , head onto the next step 
2. Add the ESP32 boards core to the Arduino IDE , File → Preferences → Additional  
   Boards Manager URLs*, add:
   ```

   https://espressif.github.io/arduino-esp32/package_esp32_index.json

   ```
   
   Then *Tools → Board → Boards Manager…*, search **esp32** (by Espressif Systems) and install it.
  
3. Install the required libraries 

| Library           | Author   | Used for        |
| ----------------- | -------- | --------------- |
| RF24              | TMRh20   | nRF24L01 driver |
| Adafruit NeoPixel | Adafruit | status LEDs     |
`SPI`, `nRF24L01.h` (bundled with RF24), along with `radio_protocol.h` (local file in this  
folder) needs no installation.
4. Set the Pins to match any Physical wiring youve done Open `Transmitter.ino`. The pin block near the top is the single place to edit if  your board is wired differently:
```cpp
// nRF24L01 on SPI

static const int PIN_NRF_CE = 4;
static const int PIN_NRF_CSN = 5;
static const int PIN_NRF_MISO = 6;
static const int PIN_NRF_SCK = 12;
static const int PIN_NRF_MOSI = 13;

// Sticks

static const int PIN_VRY1 = 2;
static const int PIN_VRX2 = 3;

// LEDs

static const int PIN_LED  = 48;
static const int NUM_LED  = 3;
```

On the ESP32-S3 the SPI pins are routed through the GPIO matrix, so any free GPIO works for SCK/MOSI/MISO/CE/CSN. (Different from the boat controller)
5.  Confirm the radio link contents match the boat (the boats radio link is governed by a very high security password by default, a list of the letters BOAT)
```cpp
static const uint8_t NRF_ADDRESS[5] = {'B','O','A','T','1',};
static const uint8_t NRF_RF_CHANNEL = 76;
#define NRF_PA_LEVEL RF24_PA_LOW
```
Data rate (RF24_250KBPS), auto-ack(true), and the static 15-byte packet is set in setup() and already should match the boat controller.
6. Compile the Sketch and Upload with the board settings according to the WROOM-MODULE soldered on.
 **At power-on, leave the steering stick centered and untouched The firmware
 auto-captures the steering center during the first 50 ms of boot (`gSteerCenter`). If you're holding the stick over at boot, that offset becomes the new "center." 
# Transmitter Setup and Connection 

-  Transmits at a fixed 50Hz (`TX_PERIOD_MS = 20`). Every frame is read into the `RadioCommandPacket`, fills the checksum **last**, and calls  `radio.write()` . There is a verification function that stores a Boolean state of the value (ack received?) is stored in `radio.write()`.
- A failsafe trips if it hears nothing for 500ms (`COMMAND_TIMEOUT_US`). 
- ARMS WITH THROTTLE , NOT A BUTTON. On boot, the transmitter forces throttle to set itself to  idle (`US_MIN = 1000`). It does not arm or change throttle settings without having seen both sticks at idle .
- Boats DIE , like very easily , a drone may or may not fly into the abyss but its Carbon Fiber so it may or may not break physically. A boat will torpedo itself into the water while containing a timebomb off a battery so there is a very important kill switch that is triggered by SW1. 
- **Buttons:** `PB0` cycles mode 0→1→2→3→0; `PB1` toggles the kill switch
- The modes and their respective purposes are

| Mode              | Value | Behavior                                                                     |
| ----------------- | ----- | ---------------------------------------------------------------------------- |
| MODE_MANUAL       | 0     | raw throttle + steering passthrough                                          |
| MODE_ASSISTED     | 1     | steering becomes a yaw rate command to like it wont accelerate into the void |
| MODE_HEADING_HOLD | 2     | holds headingDeg via a simple PID algorithm                                  |
| MODE_COURSE_HOLD  | 3     | holds GPS location                                                           |
- The LED Key is : 

| LED   | Meaning                                                            |
| ----- | ------------------------------------------------------------------ |
| LED 0 | red = killed , Orange = arming , green= linked , pink = not linked |
| LED 1 | current mode colour : green/cyan/amber/purple for modes 0-3        |
| LED 2 | throttle level (brightness increases with throttle)                |

### The packet (shared `radio_protocol.h`, 15 bytes, little-endian)
```cpp
struct_attribute_((packed)) RadioCommandPacket {

  uint16_t magic;     
  uint16_t seq;       
  uint16_t throttleUs; 
  uint16_t steeringUs;  
  uint16_t headingDeg;  
  uint16_t speedCmps;    
  uint8_t  mode;        
  uint8_t  flags;        
  uint8_t  checksum;  
}
```

- MAKE SURE BOTH ENDS MATCH , one link mismatches and the link dies
**[TX] `Transmitter.ino`:**
```cpp
static const uint8_t NRF_ADDRESS[5] = {'B','O','A','T','2'};  // new address
static const uint8_t NRF_RF_CHANNEL = 90;                     // new channel (0..125)
```
**[BOAT CONTROLLER] `Boat_Controller/config.h`:**
```cpp
static constexpr uint8_t NRF_RF_CHANNEL = 90;                       // <-- match
static constexpr uint8_t NRF_ADDRESS[5] = {'B','O','A','T','2'};   // <-- match
```

- If throttle or steering runs backwards, flip it in **one** place only
**[TX] `Transmitter.ino`:**
```cpp
static const bool INVERT_THROTTLE = true; // bottom of travel now = 1000us
static const bool INVERT_STEERING = true;
```

# Pairing the transmitter and the boat controller for the first time 

- Make sure all of the settings mentioned below are the same or tuned similar to the table. 

| Setting   | Transmitter           | Boat Controller       |
|-----------|-----------------------|-----------------------|
| Channel   | NRF_RF_CHANNEL = 76   | NRF_RF_CHANNEL = 76   |
| Address   | {'B','O','A','T','1'} | {'B','O','A','T','1'} |
| Data rate | RF24_250KBPS          | RF24_250KBPS          |
| Payload   | 15 bytes              | 15 bytes              |
| Pins      | TX(2,3)               | Boat wiring (4,6)     |
- Power the boat controller and the transmitter and open both of their serial monitor at 115200 baud rate
- Let the firmware do its thing and pray to god its pairs (give it like a good minute of two) 
-  confirm pairing status based on LED0 status (Green is linked , Orange is Arming. Anything else is probably an issue waiting to happen or one that has already happen ) 
- Before arming let the GPS get a tower hit, It may take a while for the first time but it should blink a solid red once it has connected. 
- Let it Arm by moving joystick one away from throttle after waiting for 2.5S (the minimum link to ARM time differential)
# Code Tweaks / Example implementation 

Each snipped contains info on what file to edit and what to edit in that particular file.
Usually labeled either as :
`[TX]` = transmitter  (`Transmitter.ino`) or as `[BOAT_CONTROLLER]` = boat (`Boat_Controller/…`).

**[TX] `Transmitter.ino`:**
```cpp
static const uint8_t NRF_ADDRESS[5] = {'B','O','A','T','2'};  // new address
static const uint8_t NRF_RF_CHANNEL = 90;                     // new channel (0..125)
```
**[BOAT CONTROLLER] `Boat_Controller/config.h`:**
```cpp
static constexpr uint8_t NRF_RF_CHANNEL = 90;                       // <-- match
static constexpr uint8_t NRF_ADDRESS[5] = {'B','O','A','T','2'};   // <-- match
```

Pick a channel away from busy 2.4 GHz Wi-Fi. Channels ~70–125 (≈2.470–2.525 GHz)  are usually quieter. Address is 5 bytes; both arrays must be identical.

`Adjusting Steering center/deadband`
**[TX] `Transmitter.ino`** — the dead band (how far off-center before the rudder  
reacts) is a constant:
```cpp
static const int STEER_DEADBAND = 60;   // ADC counts; bigger = larger dead zone
```
The center is auto-captured at boot from the resting stick. To hard-code a fixed  center instead (e.g. a mechanically off-center pot), replace the auto-cal block in  `setup()`:
```cpp
// Instead of sampling PIN_VRX2 at boot: 
gSteerCenter = 1980; // your measured resting ADC value (0. .4095)
```

`Changing the transmit rate`
**[TX] `Transmitter.ino`:**
```cpp
static const uint32_t TX_PERIOD_MS = 10;//measured resting ADC value (0. .4095)
```
Keep it well under the boat's 500 ms timeout. Faster than ~100 Hz buys little and  
can cost ack reliability. If you ever raise the boat's `COMMAND_TIMEOUT_US`,  
recompute your minimum safe rate.

`Change the kill-switch behaviour wired to joystick-1 click`
The firmware ships with a Latching kill switch on SW1 - in this block in loop():
```cpp
if (buttonPressed(btnSW1)) {            // joystick-1 click = kill toggle
  gKill = !gKill;                       // click to kill, click again to release
  Serial.printf("[TX] kill -> %d\n", gKill);
}
```
if you want to set up a momentary dead-man switch where "held = killed , released = live", you can replace that block with a direct level read: 
```cpp
gKill = (digitalRead(PIN_SW1)==LOW); // pressed (LOW) = kill , release = live
```
Either way, `gKill` forces transmitted throttle to idle and sets `RADIO_FLAG_KILL`;  
the boat releases the kill as soon as it receives a packet with the bit cleared, and  
re-arms once you hold idle throttle again.

`BOAT increase boat speed/change rudder throw`
**Rudder throw and ESC idle/limits  `Boat_Controller/config.h`** (used by  `actuator.cpp`):
```cpp
static constexpr int SERVO_MAX_DEFLECT_US = 340;
static constexpr int ESC_US_IDLE = 1100;
static constexpr int ESC_US_MAX = 2000;
```

`Throttle Scaling (used by radio.cpp's bp_shape_throttle():`

```cpp
#define BP_THROTTLE_SCALE 0.30f // caps effective throttle at 30%ish here 
```

`Send a Speed hold target from the transmitter :`
`speedCmps` is in cm/s and runs as function only when >0 and the boat has a GPS fix.
**[TX] `Transmitter.ino`**, in `loop()`
```cpp
uint16_t spd = (uint16_t)((long)readAdcAvg(PIN_VRY2) * 300 / ADC_MAX);
p.speedCmps = spd; //eg 100 == 1m/s
```
**[BOAT_CONTROLLER]** ensure `ENABLE_SPEED_HOLD = true` and `USE_GPS = true` in `config.h`.

`PID assist modes and tuning :`
control.cpp at boot:

```cpp
static constexpr float PID_HEADING_KP = 2.00f	// turning sensitivity 
static constexpr float PID_HEADING_KI = 0.05f
static constexpr float PID_HEADING_KD = 0.05f

static constexpr float PID_YAWRATE_KP = 2.5f // no clue what it means , example sketch said you just tune this off running it multiple times
static constexpr float PID_SPEED_KP = 0.50f  // GPS speed threshold
```

# Settings up the Transmitter case 

Put the PCB into the bottom case aligning it with the screw holes , then screw in two M3 Phillips screws, make sure you dont screw it in too deep as it may thread the bottom out but screw them in till the board feels just secured, Now put the Top part of the case on and you should be done.
![[Pasted image 20260618032216.png]]


# Files

```cmd
RC_Boat_Thingyy:.
├───Assets
│       controller_back.png
│       controller_front.png
│       controller_internal.png
│       controller_screw_assembly.png
│       pcb_board.png
│       pcb_controller.png
│       schematic_board.png
│       schematic_controller.png
│
├───CAD_Files
│       RCBoatThingy.f3z
│       RCBoatThingy.step
│       RCBoatThingy_Transmitter.f3z
│       RCBoatThingy_Transmitter.step
│
├───Firmware
│   ├───Boat Controller
│   │       actuator.cpp
│   │       actuator.h
│   │       Boat_Controller.ino
│   │       boat_profile.c
│   │       boat_profile.h
│   │       commands.cpp
│   │       commands.h
│   │       config.h
│   │       control.cpp
│   │       control.h
│   │       CONTROLLER_HANDOVER.md
│   │       estimator.cpp
│   │       estimator.h
│   │       failsafe.cpp
│   │       failsafe.h
│   │       radio.cpp
│   │       radio.h
│   │       radio_protocol.h
│   │       readme.md
│   │       sensors.cpp
│   │       sensors.h
│   │       telemetry.cpp
│   │       telemetry.h
│   │       types.h
│   │
│   └───Transmitter
│           radio_protocol.h
│           Transmitter.ino
│
└───PCB_Files
        PickAndPlace_Board.xlsx
        PickAndPlace_Transmitter.xlsx
        RCBoatThingy_Gerber_Board.zip
        RCBoatThingy_Gerber_Transmitter.zip
        Source_Boat-Board.epro
        Source_Boat_Transmitter.epro
```

# Zine 
<img width="2819" height="4000" alt="RC_boat_thingy_zine" src="https://github.com/user-attachments/assets/81ca36a4-989c-4227-a6fe-1045033b8dfa" />


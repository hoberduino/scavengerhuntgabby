#if (defined(__AVR__))
#include <avr\pgmspace.h>
#else
#include <pgmspace.h>
#endif

#include <SPI.h>
#include <MFRC522.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include <FastLED.h>


/*

 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#define NUM_LEDS    217
#define FIRST_HOUSE_LED 61
#define FIRST_MUSIC_ROOM_LED 72
#define LAST_MUSIC_ROOM_LED 81
#define FIRST_PLAYROOM_LED 203
#define LAST_PLAYROOM_LED 216
#define FIRST_TOP_FLOOR_LED 185
#define FIRST_BATHROOM_LED 144
#define LAST_BATHROOM_LED 158
#define FIRST_BEDROOM_LED 161
#define LAST_BEDROOM_LED 175

#define BRIGHTNESS  40
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define VOLUME_MUSIC 20

#define LED_PIN     7

#define NUM_NUIDS 89

// All those LEDs
CRGB leds[NUM_LEDS];

#define DISP_COLOR_BLACK             0
#define DISP_COLOR_BLUE              1
#define DISP_COLOR_ORANGE            2
#define DISP_COLOR_YELLOW            3
#define DISP_COLOR_RED               4
#define DISP_COLOR_GREEN             5
#define DISP_COLOR_CYAN              6
#define DISP_COLOR_PURPLE            7
#define DISP_COLOR_WHITE             8
#define DISP_COLOR_PINK              9
#define DISP_COLOR_CRIMSON          10
#define DISP_COLOR_CORAL            11
#define DISP_COLOR_DARK_BLUE        12
#define DISP_COLOR_GOLD             13
#define DISP_COLOR_DARK_CYAN        14 
#define DISP_COLOR_DARK_GREEN       15
#define DISP_COLOR_DARK_RED         16
#define DISP_COLOR_DARK_MAGENTA     17
#define DISP_COLOR_DARK_ORANGE      18
#define DISP_COLOR_DARK_VIOLET      19
#define DISP_COLOR_DEEP_PINK        20
#define DISP_COLOR_DARK_TURQ        21
#define DISP_COLOR_GOLDENROD        22
#define DISP_COLOR_DEEP_SKY_BLUE    23
#define DISP_COLOR_FUCHSIA          24
#define DISP_COLOR_LIGHT_SEA_GREEN  25
#define DISP_COLOR_INDIGO           26
#define DISP_COLOR_LAWN_GREEN       27
#define DISP_COLOR_MAGENTA          28
#define DISP_COLOR_LIME             29
#define DISP_COLOR_MAROON           30
#define DISP_COLOR_MED_VIOLET_RED   31
#define DISP_COLOR_MED_BLUE         32
#define DISP_COLOR_MED_SPRING_GREEN 33
#define DISP_COLOR_ORANGE_RED       34
#define DISP_COLOR_NAVY             35
#define DISP_COLOR_OLIVE            36
#define DISP_COLOR_SALMON           37
#define DISP_COLOR_PERU             38
#define DISP_COLOR_ROYAL_BLUE       39
#define DISP_COLOR_TEAL             40
#define DISP_COLOR_SEA_GREEN        41
#define DISP_COLOR_SPRING_GREEN     42
#define DISP_COLOR_FAIRY_LIGHT      43
#define DISP_COLOR_VIOLET           44
#define DISP_COLOR_MINT             45
#define DISP_COLOR_BROWN            46
#define NUM_DISP_COLORS             47

const CRGB numToColor[NUM_DISP_COLORS] = 
{CRGB::Black, CRGB::Blue, CRGB::Orange, CRGB::Yellow, CRGB::Red, CRGB::Green, CRGB::Cyan, CRGB::Purple,
 CRGB::White, 0xFF1493, CRGB::Crimson, CRGB::Coral, CRGB::DarkBlue, CRGB::Gold, CRGB::DarkCyan, CRGB::DarkGreen,
 CRGB::DarkRed, CRGB::DarkMagenta, CRGB::DarkOrange, CRGB::DarkViolet, CRGB::DeepPink, CRGB::DarkTurquoise, CRGB::Goldenrod, CRGB::DeepSkyBlue, 
 CRGB::Fuchsia, CRGB::LightSeaGreen, CRGB::Indigo, CRGB::LawnGreen, CRGB::Magenta, CRGB::Lime, CRGB::Maroon, CRGB::MediumVioletRed, 
 CRGB::MediumBlue, CRGB::MediumSpringGreen, CRGB::OrangeRed, CRGB::Navy, CRGB::Olive, CRGB::Salmon, CRGB::Peru, CRGB::RoyalBlue,
 CRGB::Teal, CRGB::SeaGreen, CRGB::SpringGreen, CRGB::FairyLight, CRGB::Violet, 0x28a828, CRGB::Brown};


const unsigned int PROGMEM character_nuid_one[NUM_NUIDS] =
{224, // Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81 // 1
 192, // Cinderella 2 (Blue) - In hex:  C0 C9 B7 51 In dec:  192 201 183 81 // 2
 192, // Prince Charming - In hex:  C0 8A B7 51 In dec:  192 138 183 81 // 3
 54, // Pocahontas - In hex:  36 C0 41 33 In dec:  54 192 65 51 // 4
 192, // Belle 1 (Yellow) - In hex:  C0 9A B7 51 In dec:  192 154 183 81 // 5
 192, // Fairy Godmother - In hex:  C0 AA B7 51 In dec:  192 170 183 81 // 6
 192, // Merida 1 (Green) - In hex:  C0 BA B7 51 In dec:  192 186 183 81 // 7
 64, // Jasmine 1 (Green) - In hex:  40 D7 B7 51 In dec:  64 215 183 81 // 8
 48, // Mulan 1 (Dress) - In hex:  30 E7 B7 51 In dec:  48 231 183 81 // 9
 224, // Lumiere - In hex:  E0 F6 B7 51 In dec:  224 246 183 81 // 10
 160, // Gus and Jaq - In hex:  A0 04 B8 51 In dec:  160 04 184 81 // 11
 64, // Rapunzel 1 (Purple) - In hex:  40 58 B7 51 In dec:  64 88 183 81 // 12
 64, // Aurora 1 (Pink) - In hex: In hex:  40 66 B7 51 In dec:  64 102 183 81 // 13
 160, // Olaf 1 - In hex:  A0 C4 B7 51 In dec:  160 196 183 81 // 14
 32, // Tiana 1 (Green) - In hex:  20 D2 B7 51 In dec:  32 210 183 81 // 15
 32, // Snow White 1 (Dress) - In hex:  20 E1 B7 51 In dec:  32 225 183 81 // 16
 192, // Hei Hei 1 - In hex:  C0 CD B6 51 In dec:  192 205 182 81 // 17
 240, // Pua 1 - In hex:  F0 BD B6 51 In dec:  240 189 182 81 // 18
 176, // Genie 1 - In hex:  B0 DD B6 51 In dec:  176 221 182 81 // 19
 128, // Mickey - In hex:  80 05 B8 51 In dec:  128 05 184 81 // 20
 144, // Daisy - In hex:  90 05 B8 51 In dec:  144 05 184 81 // 21
 32, // Donald - In hex:  20 F8 B7 51 In dec:  32 248 183 81 // 22
 160, // Goofy - In hex:  A0 E8 B7 51 In dec:  160 232 183 81 // 23
 24, // Minnie - In hex:  E0 DD B7 51 In dec:  224 221 183 81 // 24
 48, // Vanellope (WIR) - In hex:  30 C4 B7 51 In dec:  48 196 183 81 // 25
 48, // Ariel (WIR) - In hex:  30 B4 B7 51 In dec:  48 180 183 81 // 26
 48, // Elsa (WIR) - In hex:  30 A6 B7 51 In dec:  48 166 183 81 // 27
 48, // Snow White (WIR) In hex:  30 96 B7 51 In dec:  48 150 183 81 // 28
 48, // Mulan (WIR) In hex:  30 86 B7 51 In dec:  48 134 183 81 // 29
 80, // Belle (WIR) In hex:  50 76 B7 51 In dec:  80 118 183 81 // 30
 192, // Rapunzel (WIR) In hex:  C0 61 B7 51 In dec:  192 97 183 81 // 31
 208, // Moana (WIR) In hex:  D0 51 B7 51 In dec:  208 81 183 81 // 32
 208, // Jasmine (WIR) In hex:  D0 41 B7 51 In dec:  208 65 183 81 // 33
 208, // Merida (WIR) In hex:  D0 31 B7 51 In dec:  208 49 183 81 // 34
 224, // Anna (WIR) In hex:  E0 21 B7 51 In dec:  224 33 183 81 // 35
 224, // Cinderella (WIR) In hex:  E0 11 B7 51 In dec:  224 17 183 81 // 36
 224, // Tiana (WIR) In hex:  E0 FC B6 51 In dec:  224 252 182 81 // 37
 80, // Aurora (WIR) In hex:  50 ED B6 51 In dec:  80 237 182 81 // 38
 128, // Pocahontas (WIR) In hex:  80 DD B6 51 In dec:  128 221 182 81 // 39
 214, // Flora, Fauna, Merryweather In hex:  D6 49 39 33 In dec:  214 73 57 51 // 40
 06, // Rapunzel 3 (gray, white) In hex:  06 B3 38 33 In dec:  06 179 56 51 // 41
 118, // Maleficent Dragon In hex:  76 1F 38 33 In dec:  118 31 56 51 // 42
 230, // Maleficent In hex:  E6 1A 37 33 In dec:  230 26 55 51 // 43
 166, // Aurora 4 (Pink) In hex:  A6 90 36 33 In dec:  166 144 54 51 // 44
 150, // Prince Philip In hex:  96 16 37 33 In dec:  150 22 55 51 // 45
 150, // Ice Horse In hex:  96 1A 38 33 In dec:  150 26 56 51 // 46
 38, // Queen Anna Frozen 2 In hex:  26 B0 38 33 In dec:  38 176 56 51 // 47
 22, // Kristoff In hex:  16 46 39 33 In dec:  22 70 57 51 // 48
 214, // Olaf 2 In hex:  D6 E4 39 33 In dec:  214 228 57 51 // 49
 198, // Elsa Frozen 2 In hex:  C6 72 3F 33 In dec:  198 114 63 51 // 50
 214, // Christmas In hex:  D6 BC 41 33 In dec:  214 188 65 51 // 51
 54, // PJ Masks Tower - In hex:  36 2A 41 33 In dec:  54 42 65 51 // 52
 198, // Cheeseburger 1 - In hex:  C6 4E 42 33 In dec:  198 78 66 51 // 53
 246, // Cheeseburger 2 - In hex:  F6 DD 42 33 In dec:  246 221 66 51 // 54
 192, // Cake 1 - In hex:  C0 74 B7 51 In dec:  192 116 183 81 // 55
 214, // Cake 2 In hex:  D6 F2 3F 33 In dec:  214 242 63 51 // 56
 38, // Cake 3 In hex:  26 8E 40 33 In dec:  38 142 64 51 // 57
 160, // Ice Cream 1 - In hex:  A0 B4 B7 51 In dec:  160 180 183 81 // 58
 160, // Ice Cream 2 - In hex:  A0 94 B7 51 In dec:  160 148 183 81 // 59
 160,  // Ice Cream 3 - In hex:  A0 A4 B7 51 In dec:  160 164 183 81 // 60
 28, // Fia In hex:  1C 37 72 17 In dec:  28 55 114 23 // 61
 60, // Kaia In hex:  3C 79 71 17 In dec:  60 121 113 23 // 62
 131, // Cinderella New In hex:  83 00 80 61  In dec:  131 00 128 97 // 63
 195, // Jasmine New In hex:  C3 64 80 61  In dec:  195 100 128 97 // 64
 83, // Pocahontas New In hex:  53 B6 80 61  In dec:  83 182 128 97 // 65
 147, // Snow White New In hex:  93 18 81 61  In dec:  147 24 129 97 // 66
 131, // Tiana New In hex:  83 67 81 61  In dec:  131 103 129 97 // 67
 147, // Mulan New In hex:  93 CA 81 61  In dec:  147 202 129 97 // 68
 243, // Aurora New In hex:  F3 F6 5B 61  In dec:  243 246 91 97 // 69
 227, // Belle New In hex:  E3 6E 82 61  In dec:  227 110 130 97 // 70
 51,  // Carlita In hex:  33 C1 7A 61 In dec:  51 193 122 97 // 71
 163, // Pandy In hex:  A3 A7 7B 61 In dec:  163 167 123 97 // 72
 163, // Mercat 1 In hex:  A3 D0 79 61 In dec:  163 208 121 97 // 73
 131, // Mercat 2 In hex:  83 B3 69 61 In dec:  131 179 105 97 // 74
 19,  // Box Cat In hex:  13 61 79 61 In dec:  19 97 121 97 // 75
 211, // DJ Catnip In hex:  D3 C2 6A 61 In dec:  211 194 106 97 // 76
 67,  // Catrat In hex:  43 6E 78 61 In dec:  67 110 120 97 // 77
 67,  // Pillowcat In hex:  43 FF 5A 61 In dec:  67 255 90 97 // 78
 67,  // Cupcake In hex:  43 E0 78 61 In dec:  67 224 120 97 // 79
 35,  // Gabby In hex:  23 4C 7A 61 In dec:  35 76 122 97 // 80
 131, // Kitty Fairy In hex:  83 35 7B 61 In dec:  131 53 123 97 // 81
 51,  // Big Puzzle 1 In hex:  33 88 6E 61 In dec:  51 136 110 97 // 82
 227, // Big Puzzle 2 In hex:  E3 00 6F 61 In dec:  227 00 111 97 // 83
 03,  // Big Puzzle 3 In hex:  03 7C 6F 61 In dec:  03 124 111 97 // 84
 179, // Big Puzzle 4 In hex:  B3 F8 6F 61 In dec:  179 248 111 97 // 85
 67,  // Little Puzzle 1 In hex:  43 74 67 61 In dec:  67 116 103 97 // 86
 99,  // Little Puzzle 2 In hex:  63 E5 66 61 In dec:  99 229 102 97 // 87
 99,  // Little Puzzle 3 In hex:  63 CF 65 61 In dec:  99 207 101 97 // 88
 211  // Little Puzzle 4 In hex:  D3 55 66 61 In dec:  211 85 102 97 // 89
 };

const unsigned int PROGMEM character_nuid_two[NUM_NUIDS] =
{122, // Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81
 201, // Cinderella 2 (Blue) - In hex:  C0 C9 B7 51 In dec:  192 201 183 81
 138, // Prince Charming - In hex:  C0 8A B7 51 In dec:  192 138 183 81
 192, // Pocahontas - In hex:  36 C0 41 33 In dec:  54 192 65 51
 154, // Belle 1 (Yellow) - In hex:  C0 9A B7 51 In dec:  192 154 183 81
 170, // Fairy Godmother - In hex:  C0 AA B7 51 In dec:  192 170 183 81
 186, // Merida 1 (Green) - In hex:  C0 BA B7 51 In dec:  192 186 183 81
 215, // Jasmine 1 (Green) - In hex:  40 D7 B7 51 In dec:  64 215 183 81
 231, // Mulan 1 (Dress) - In hex:  30 E7 B7 51 In dec:  48 231 183 81
 246, // Lumiere - In hex:  E0 F6 B7 51 In dec:  224 246 183 81
 04, // Gus and Jaq - In hex:  A0 04 B8 51 In dec:  160 04 184 81
 88, // Rapunzel 1 (Purple) - In hex:  40 58 B7 51 In dec:  64 88 183 81
 102, // Aurora 1 (Pink) - In hex: In hex:  40 66 B7 51 In dec:  64 102 183 81
 196, // Olaf 1 - In hex:  A0 C4 B7 51 In dec:  160 196 183 81
 210, // Tiana 1 (Green) - In hex:  20 D2 B7 51 In dec:  32 210 183 81
 225, // Snow White 1 (Dress) - In hex:  20 E1 B7 51 In dec:  32 225 183 81
 205, // Hei Hei 1 - In hex:  C0 CD B6 51 In dec:  192 205 182 81
 189, // Pua 1 - In hex:  F0 BD B6 51 In dec:  240 189 182 81
 221, // Genie 1 - In hex:  B0 DD B6 51 In dec:  176 221 182 81
 05, // Mickey - In hex:  80 05 B8 51 In dec:  128 05 184 81
 05, // Daisy - In hex:  90 05 B8 51 In dec:  144 05 184 81
 248, // Donald - In hex:  20 F8 B7 51 In dec:  32 248 183 81
 232, // Goofy - In hex:  A0 E8 B7 51 In dec:  160 232 183 81
 221, // Minnie - In hex:  E0 DD B7 51 In dec:  224 221 183 81
 196, // Vanellope (WIR) - In hex:  30 C4 B7 51 In dec:  48 196 183 81
 180, // Ariel (WIR) - In hex:  30 B4 B7 51 In dec:  48 180 183 81
 166, // Elsa (WIR) - In hex:  30 A6 B7 51 In dec:  48 166 183 81
 150, // Snow White (WIR) In hex:  30 96 B7 51 In dec:  48 150 183 81
 134, // Mulan (WIR) In hex:  30 86 B7 51 In dec:  48 134 183 81
 118, // Belle (WIR) In hex:  50 76 B7 51 In dec:  80 118 183 81
 97, // Rapunzel (WIR) In hex:  C0 61 B7 51 In dec:  192 97 183 81
 81, // Moana (WIR) In hex:  D0 51 B7 51 In dec:  208 81 183 81
 65, // Jasmine (WIR) In hex:  D0 41 B7 51 In dec:  208 65 183 81
 49, // Merida (WIR) In hex:  D0 31 B7 51 In dec:  208 49 183 81
 33, // Anna (WIR) In hex:  E0 21 B7 51 In dec:  224 33 183 81
 17, // Cinderella (WIR) In hex:  E0 11 B7 51 In dec:  224 17 183 81
 252, // Tiana (WIR) In hex:  E0 FC B6 51 In dec:  224 252 182 81
 237, // Aurora (WIR) In hex:  50 ED B6 51 In dec:  80 237 182 81
 221, // Pocahontas (WIR) In hex:  80 DD B6 51 In dec:  128 221 182 81
 73, // Flora, Fauna, Merryweather In hex:  D6 49 39 33 In dec:  214 73 57 51
 179, // Rapunzel 3 (gray, white) In hex:  06 B3 38 33 In dec:  06 179 56 51
 31, // Maleficent Dragon In hex:  76 1F 38 33 In dec:  118 31 56 51
 26, // Maleficent In hex:  E6 1A 37 33 In dec:  230 26 55 51
 144, // Aurora 4 (Pink) In hex:  A6 90 36 33 In dec:  166 144 54 51
 22, // Prince Philip In hex:  96 16 37 33 In dec:  150 22 55 51
 26, // Ice Horse In hex:  96 1A 38 33 In dec:  150 26 56 51
 176, // Queen Anna Frozen 2 In hex:  26 B0 38 33 In dec:  38 176 56 51
 70, // Kristoff In hex:  16 46 39 33 In dec:  22 70 57 51
 228, // Olaf 2 In hex:  D6 E4 39 33 In dec:  214 228 57 51
 114, // Elsa Frozen 2 In hex:  C6 72 3F 33 In dec:  198 114 63 51
 188, // Christmas In hex:  D6 BC 41 33 In dec:  214 188 65 51
 42, // PJ Masks Tower - In hex:  36 2A 41 33 In dec:  54 42 65 51
 78, // Cheeseburger 1 - In hex:  C6 4E 42 33 In dec:  198 78 66 51
 221, // Cheeseburger 2 - In hex:  F6 DD 42 33 In dec:  246 221 66 51
 116, // Cake 1 - In hex:  C0 74 B7 51 In dec:  192 116 183 81
 242, // Cake 2 In hex:  D6 F2 3F 33 In dec:  214 242 63 51
 142, // Cake 3 In hex:  26 8E 40 33 In dec:  38 142 64 51
 180, // Ice Cream 1 - In hex:  A0 B4 B7 51 In dec:  160 180 183 81
 148, // Ice Cream 2 - In hex:  A0 94 B7 51 In dec:  160 148 183 81
 164,  // Ice Cream 3 - In hex:  A0 A4 B7 51 In dec:  160 164 183 81
 55, // Fia In hex:  1C 37 72 17 In dec:  28 55 114 23
 121, // Kaia In hex:  3C 79 71 17 In dec:  60 121 113 23
 0, // Cinderella New In hex:  83 00 80 61  In dec:  131 00 128 97
 100, // Jasmine New In hex:  C3 64 80 61  In dec:  195 100 128 97
 182, // Pocahontas New In hex:  53 B6 80 61  In dec:  83 182 128 97
 24, // Snow White New In hex:  93 18 81 61  In dec:  147 24 129 97
 103, // Tiana New In hex:  83 67 81 61  In dec:  131 103 129 97
 202, // Mulan New In hex:  93 CA 81 61  In dec:  147 202 129 97
 246, // Aurora New In hex:  F3 F6 5B 61  In dec:  243 246 91 97
 110, // Belle New In hex:  E3 6E 82 61  In dec:  227 110 130 97
 193, // Carlita In hex:  33 C1 7A 61 In dec:  51 193 122 97 // 71
 167, // Pandy In hex:  A3 A7 7B 61 In dec:  163 167 123 97 // 72
 208, // Mercat 1 In hex:  A3 D0 79 61 In dec:  163 208 121 97 // 73
 179, // Mercat 2 In hex:  83 B3 69 61 In dec:  131 179 105 97 // 74
 97,  // Box Cat In hex:  13 61 79 61 In dec:  19 97 121 97 // 75
 194, // DJ Catnip In hex:  D3 C2 6A 61 In dec:  211 194 106 97 // 76
 110, // Catrat In hex:  43 6E 78 61 In dec:  67 110 120 97 // 77
 255, // Pillowcat In hex:  43 FF 5A 61 In dec:  67 255 90 97 // 78
 224, // Cupcake In hex:  43 E0 78 61 In dec:  67 224 120 97 // 79
 76,  // Gabby In hex:  23 4C 7A 61 In dec:  35 76 122 97 // 80
 53,  // Kitty Fairy In hex:  83 35 7B 61 In dec:  131 53 123 97 // 81
 136, // Big Puzzle 1 In hex:  33 88 6E 61 In dec:  51 136 110 97 // 82
 00,  // Big Puzzle 2 In hex:  E3 00 6F 61 In dec:  227 00 111 97 // 83
 124, // Big Puzzle 3 In hex:  03 7C 6F 61 In dec:  03 124 111 97 // 84
 248, // Big Puzzle 4 In hex:  B3 F8 6F 61 In dec:  179 248 111 97 // 85
 116, // Little Puzzle 1 In hex:  43 74 67 61 In dec:  67 116 103 97 // 86
 229, // Little Puzzle 2 In hex:  63 E5 66 61 In dec:  99 229 102 97 // 87
 207, // Little Puzzle 3 In hex:  63 CF 65 61 In dec:  99 207 101 97 // 88
 85   // Little Puzzle 4 In hex:  D3 55 66 61 In dec:  211 85 102 97 // 89
 };

const unsigned int PROGMEM character_nuid_three[NUM_NUIDS] =
{183, // Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81
 183, // Cinderella 2 (Blue) - In hex:  C0 C9 B7 51 In dec:  192 201 183 81
 183, // Prince Charming - In hex:  C0 8A B7 51 In dec:  192 138 183 81
 65, // Pocahontas - In hex:  36 C0 41 33 In dec:  54 192 65 51
 183, // Belle 1 (Yellow) - In hex:  C0 9A B7 51 In dec:  192 154 183 81
 183, // Fairy Godmother - In hex:  C0 AA B7 51 In dec:  192 170 183 81
 183, // Merida 1 (Green) - In hex:  C0 BA B7 51 In dec:  192 186 183 81
 183, // Jasmine 1 (Green) - In hex:  40 D7 B7 51 In dec:  64 215 183 81
 183, // Mulan 1 (Dress) - In hex:  30 E7 B7 51 In dec:  48 231 183 81
 183, // Lumiere - In hex:  E0 F6 B7 51 In dec:  224 246 183 81
 184, // Gus and Jaq - In hex:  A0 04 B8 51 In dec:  160 04 184 81
 183, // Rapunzel 1 (Purple) - In hex:  40 58 B7 51 In dec:  64 88 183 81
 183, // Aurora 1 (Pink) - In hex: In hex:  40 66 B7 51 In dec:  64 102 183 81
 183, // Olaf 1 - In hex:  A0 C4 B7 51 In dec:  160 196 183 81
 183, // Tiana 1 (Green) - In hex:  20 D2 B7 51 In dec:  32 210 183 81
 183, // Snow White 1 (Dress) - In hex:  20 E1 B7 51 In dec:  32 225 183 81
 182, // Hei Hei 1 - In hex:  C0 CD B6 51 In dec:  192 205 182 81
 182, // Pua 1 - In hex:  F0 BD B6 51 In dec:  240 189 182 81
 182, // Genie 1 - In hex:  B0 DD B6 51 In dec:  176 221 182 81
 184, // Mickey - In hex:  80 05 B8 51 In dec:  128 05 184 81
 184, // Daisy - In hex:  90 05 B8 51 In dec:  144 05 184 81
 183, // Donald - In hex:  20 F8 B7 51 In dec:  32 248 183 81
 183, // Goofy - In hex:  A0 E8 B7 51 In dec:  160 232 183 81
 183, // Minnie - In hex:  E0 DD B7 51 In dec:  224 221 183 81
 183, // Vanellope (WIR) - In hex:  30 C4 B7 51 In dec:  48 196 183 81
 183, // Ariel (WIR) - In hex:  30 B4 B7 51 In dec:  48 180 183 81
 183, // Elsa (WIR) - In hex:  30 A6 B7 51 In dec:  48 166 183 81
 183, // Snow White (WIR) In hex:  30 96 B7 51 In dec:  48 150 183 81
 183, // Mulan (WIR) In hex:  30 86 B7 51 In dec:  48 134 183 81
 183, // Belle (WIR) In hex:  50 76 B7 51 In dec:  80 118 183 81
 183, // Rapunzel (WIR) In hex:  C0 61 B7 51 In dec:  192 97 183 81
 183, // Moana (WIR) In hex:  D0 51 B7 51 In dec:  208 81 183 81
 183, // Jasmine (WIR) In hex:  D0 41 B7 51 In dec:  208 65 183 81
 183, // Merida (WIR) In hex:  D0 31 B7 51 In dec:  208 49 183 81
 183, // Anna (WIR) In hex:  E0 21 B7 51 In dec:  224 33 183 81
 183, // Cinderella (WIR) In hex:  E0 11 B7 51 In dec:  224 17 183 81
 182, // Tiana (WIR) In hex:  E0 FC B6 51 In dec:  224 252 182 81
 182, // Aurora (WIR) In hex:  50 ED B6 51 In dec:  80 237 182 81
 182, // Pocahontas (WIR) In hex:  80 DD B6 51 In dec:  128 221 182 81
 57, // Flora, Fauna, Merryweather In hex:  D6 49 39 33 In dec:  214 73 57 51
 56, // Rapunzel 3 (gray, white) In hex:  06 B3 38 33 In dec:  06 179 56 51
 56, // Maleficent Dragon In hex:  76 1F 38 33 In dec:  118 31 56 51
 55, // Maleficent In hex:  E6 1A 37 33 In dec:  230 26 55 51
 54, // Aurora 4 (Pink) In hex:  A6 90 36 33 In dec:  166 144 54 51
 55, // Prince Philip In hex:  96 16 37 33 In dec:  150 22 55 51
 56, // Ice Horse In hex:  96 1A 38 33 In dec:  150 26 56 51
 56, // Queen Anna Frozen 2 In hex:  26 B0 38 33 In dec:  38 176 56 51
 57, // Kristoff In hex:  16 46 39 33 In dec:  22 70 57 51
 57, // Olaf 2 In hex:  D6 E4 39 33 In dec:  214 228 57 51
 63, // Elsa Frozen 2 In hex:  C6 72 3F 33 In dec:  198 114 63 51
 65, // Christmas In hex:  D6 BC 41 33 In dec:  214 188 65 51
 65, // PJ Masks Tower - In hex:  36 2A 41 33 In dec:  54 42 65 51
 66, // Cheeseburger 1 - In hex:  C6 4E 42 33 In dec:  198 78 66 51
 66, // Cheeseburger 2 - In hex:  F6 DD 42 33 In dec:  246 221 66 51
 183, // Cake 1 - In hex:  C0 74 B7 51 In dec:  192 116 183 81
 63, // Cake 2 In hex:  D6 F2 3F 33 In dec:  214 242 63 51
 64, // Cake 3 In hex:  26 8E 40 33 In dec:  38 142 64 51
 183, // Ice Cream 1 - In hex:  A0 B4 B7 51 In dec:  160 180 183 81
 183, // Ice Cream 2 - In hex:  A0 94 B7 51 In dec:  160 148 183 81
 183,  // Ice Cream 3 - In hex:  A0 A4 B7 51 In dec:  160 164 183 81
 114, // Fia In hex:  1C 37 72 17 In dec:  28 55 114 23
 113, // Kaia In hex:  3C 79 71 17 In dec:  60 121 113 23
 128, // Cinderella New In hex:  83 00 80 61  In dec:  131 00 128 97
 128, // Jasmine New In hex:  C3 64 80 61  In dec:  195 100 128 97
 128, // Pocahontas New In hex:  53 B6 80 61  In dec:  83 182 128 97
 129, // Snow White New In hex:  93 18 81 61  In dec:  147 24 129 97
 129, // Tiana New In hex:  83 67 81 61  In dec:  131 103 129 97
 129, // Mulan New In hex:  93 CA 81 61  In dec:  147 202 129 97
 91,  // Aurora New In hex:  F3 F6 5B 61  In dec:  243 246 91 97
 130, // Belle New In hex:  E3 6E 82 61  In dec:  227 110 130 97 // 70
 122, // Carlita In hex:  33 C1 7A 61 In dec:  51 193 122 97 // 71
 123, // Pandy In hex:  A3 A7 7B 61 In dec:  163 167 123 97 // 72
 121, // Mercat 1 In hex:  A3 D0 79 61 In dec:  163 208 121 97 // 73
 105, // Mercat 2 In hex:  83 B3 69 61 In dec:  131 179 105 97 // 74
 121, // Box Cat In hex:  13 61 79 61 In dec:  19 97 121 97 // 75
 106, // DJ Catnip In hex:  D3 C2 6A 61 In dec:  211 194 106 97 // 76
 120, // Catrat In hex:  43 6E 78 61 In dec:  67 110 120 97 // 77
 90,  // Pillowcat In hex:  43 FF 5A 61 In dec:  67 255 90 97 // 78
 120, // Cupcake In hex:  43 E0 78 61 In dec:  67 224 120 97 // 79
 122, // Gabby In hex:  23 4C 7A 61 In dec:  35 76 122 97 // 80
 123, // Kitty Fairy In hex:  83 35 7B 61 In dec:  131 53 123 97 // 81
 110, // Big Puzzle 1 In hex:  33 88 6E 61 In dec:  51 136 110 97 // 82
 111, // Big Puzzle 2 In hex:  E3 00 6F 61 In dec:  227 00 111 97 // 83
 111, // Big Puzzle 3 In hex:  03 7C 6F 61 In dec:  03 124 111 97 // 84
 111, // Big Puzzle 4 In hex:  B3 F8 6F 61 In dec:  179 248 111 97 // 85
 103, // Little Puzzle 1 In hex:  43 74 67 61 In dec:  67 116 103 97 // 86
 102, // Little Puzzle 2 In hex:  63 E5 66 61 In dec:  99 229 102 97 // 87
 101, // Little Puzzle 3 In hex:  63 CF 65 61 In dec:  99 207 101 97 // 88
 102  // Little Puzzle 4 In hex:  D3 55 66 61 In dec:  211 85 102 97 // 89
 };

const unsigned int PROGMEM character_nuid_four[NUM_NUIDS] =
{81, // Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81
 81, // Cinderella 2 (Blue) - In hex:  C0 C9 B7 51 In dec:  192 201 183 81
 81, // Prince Charming - In hex:  C0 8A B7 51 In dec:  192 138 183 81
 51, // Pocahontas - In hex:  36 C0 41 33 In dec:  54 192 65 51
 81, // Belle 1 (Yellow) - In hex:  C0 9A B7 51 In dec:  192 154 183 81
 81, // Fairy Godmother - In hex:  C0 AA B7 51 In dec:  192 170 183 81
 81, // Merida 1 (Green) - In hex:  C0 BA B7 51 In dec:  192 186 183 81
 81, // Jasmine 1 (Green) - In hex:  40 D7 B7 51 In dec:  64 215 183 81
 81, // Mulan 1 (Dress) - In hex:  30 E7 B7 51 In dec:  48 231 183 81
 81, // Lumiere - In hex:  E0 F6 B7 51 In dec:  224 246 183 81
 81, // Gus and Jaq - In hex:  A0 04 B8 51 In dec:  160 04 184 81
 81, // Rapunzel 1 (Purple) - In hex:  40 58 B7 51 In dec:  64 88 183 81
 81, // Aurora 1 (Pink) - In hex: In hex:  40 66 B7 51 In dec:  64 102 183 81
 81, // Olaf 1 - In hex:  A0 C4 B7 51 In dec:  160 196 183 81
 81, // Tiana 1 (Green) - In hex:  20 D2 B7 51 In dec:  32 210 183 81
 81, // Snow White 1 (Dress) - In hex:  20 E1 B7 51 In dec:  32 225 183 81
 81, // Hei Hei 1 - In hex:  C0 CD B6 51 In dec:  192 205 182 81
 81, // Pua 1 - In hex:  F0 BD B6 51 In dec:  240 189 182 81
 81, // Genie 1 - In hex:  B0 DD B6 51 In dec:  176 221 182 81
 81, // Mickey - In hex:  80 05 B8 51 In dec:  128 05 184 81
 81, // Daisy - In hex:  90 05 B8 51 In dec:  144 05 184 81
 81, // Donald - In hex:  20 F8 B7 51 In dec:  32 248 183 81
 81, // Goofy - In hex:  A0 E8 B7 51 In dec:  160 232 183 81
 81, // Minnie - In hex:  E0 DD B7 51 In dec:  224 221 183 81
 81, // Vanellope (WIR) - In hex:  30 C4 B7 51 In dec:  48 196 183 81
 81, // Ariel (WIR) - In hex:  30 B4 B7 51 In dec:  48 180 183 81
 81, // Elsa (WIR) - In hex:  30 A6 B7 51 In dec:  48 166 183 81
 81, // Snow White (WIR) In hex:  30 96 B7 51 In dec:  48 150 183 81
 81, // Mulan (WIR) In hex:  30 86 B7 51 In dec:  48 134 183 81
 81, // Belle (WIR) In hex:  50 76 B7 51 In dec:  80 118 183 81
 81, // Rapunzel (WIR) In hex:  C0 61 B7 51 In dec:  192 97 183 81
 81, // Moana (WIR) In hex:  D0 51 B7 51 In dec:  208 81 183 81
 81, // Jasmine (WIR) In hex:  D0 41 B7 51 In dec:  208 65 183 81
 81, // Merida (WIR) In hex:  D0 31 B7 51 In dec:  208 49 183 81
 81, // Anna (WIR) In hex:  E0 21 B7 51 In dec:  224 33 183 81
 81, // Cinderella (WIR) In hex:  E0 11 B7 51 In dec:  224 17 183 81
 81, // Tiana (WIR) In hex:  E0 FC B6 51 In dec:  224 252 182 81
 81, // Aurora (WIR) In hex:  50 ED B6 51 In dec:  80 237 182 81
 81, // Pocahontas (WIR) In hex:  80 DD B6 51 In dec:  128 221 182 81
 51, // Flora, Fauna, Merryweather In hex:  D6 49 39 33 In dec:  214 73 57 51
 51, // Rapunzel 3 (gray, white) In hex:  06 B3 38 33 In dec:  06 179 56 51
 51, // Maleficent Dragon In hex:  76 1F 38 33 In dec:  118 31 56 51
 51, // Maleficent In hex:  E6 1A 37 33 In dec:  230 26 55 51
 51, // Aurora 4 (Pink) In hex:  A6 90 36 33 In dec:  166 144 54 51
 51, // Prince Philip In hex:  96 16 37 33 In dec:  150 22 55 51
 51, // Ice Horse In hex:  96 1A 38 33 In dec:  150 26 56 51
 51, // Queen Anna Frozen 2 In hex:  26 B0 38 33 In dec:  38 176 56 51
 51, // Kristoff In hex:  16 46 39 33 In dec:  22 70 57 51
 51, // Olaf 2 In hex:  D6 E4 39 33 In dec:  214 228 57 51
 51, // Elsa Frozen 2 In hex:  C6 72 3F 33 In dec:  198 114 63 51
 51, // Christmas In hex:  D6 BC 41 33 In dec:  214 188 65 51
 51, // PJ Masks Tower - In hex:  36 2A 41 33 In dec:  54 42 65 51
 51, // Cheeseburger 1 - In hex:  C6 4E 42 33 In dec:  198 78 66 51
 51, // Cheeseburger 2 - In hex:  F6 DD 42 33 In dec:  246 221 66 51
 81, // Cake 1 - In hex:  C0 74 B7 51 In dec:  192 116 183 81
 51, // Cake 2 In hex:  D6 F2 3F 33 In dec:  214 242 63 51
 51, // Cake 3 In hex:  26 8E 40 33 In dec:  38 142 64 51
 81, // Ice Cream 1 - In hex:  A0 B4 B7 51 In dec:  160 180 183 81
 81, // Ice Cream 2 - In hex:  A0 94 B7 51 In dec:  160 148 183 81
 81,  // Ice Cream 3 - In hex:  A0 A4 B7 51 In dec:  160 164 183 81
 23, // Fia In hex:  1C 37 72 17 In dec:  28 55 114 23
 23, // Kaia In hex:  3C 79 71 17 In dec:  60 121 113 23
 97, // Cinderella New In hex:  83 00 80 61  In dec:  131 00 128 97
 97, // Jasmine New In hex:  C3 64 80 61  In dec:  195 100 128 97
 97, // Pocahontas New In hex:  53 B6 80 61  In dec:  83 182 128 97
 97, // Snow White New In hex:  93 18 81 61  In dec:  147 24 129 97
 97, // Tiana New In hex:  83 67 81 61  In dec:  131 103 129 97
 97, // Mulan New In hex:  93 CA 81 61  In dec:  147 202 129 97
 97, // Aurora New In hex:  F3 F6 5B 61  In dec:  243 246 91 97
 97, // Belle New In hex:  E3 6E 82 61  In dec:  227 110 130 97
 97, // Carlita In hex:  33 C1 7A 61 In dec:  51 193 122 97 // 71
 97, // Pandy In hex:  A3 A7 7B 61 In dec:  163 167 123 97 // 72
 97, // Mercat 1 In hex:  A3 D0 79 61 In dec:  163 208 121 97 // 73
 97, // Mercat 2 In hex:  83 B3 69 61 In dec:  131 179 105 97 // 74
 97, // Box Cat In hex:  13 61 79 61 In dec:  19 97 121 97 // 75
 97, // DJ Catnip In hex:  D3 C2 6A 61 In dec:  211 194 106 97 // 76
 97, // Catrat In hex:  43 6E 78 61 In dec:  67 110 120 97 // 77
 97, // Pillowcat In hex:  43 FF 5A 61 In dec:  67 255 90 97 // 78
 97, // Cupcake In hex:  43 E0 78 61 In dec:  67 224 120 97 // 79
 97, // Gabby In hex:  23 4C 7A 61 In dec:  35 76 122 97 // 80
 97, // Kitty Fairy In hex:  83 35 7B 61 In dec:  131 53 123 97 // 81
 97, // Big Puzzle 1 In hex:  33 88 6E 61 In dec:  51 136 110 97 // 82
 97, // Big Puzzle 2 In hex:  E3 00 6F 61 In dec:  227 00 111 97 // 83
 97, // Big Puzzle 3 In hex:  03 7C 6F 61 In dec:  03 124 111 97 // 84
 97, // Big Puzzle 4 In hex:  B3 F8 6F 61 In dec:  179 248 111 97 // 85
 97, // Little Puzzle 1 In hex:  43 74 67 61 In dec:  67 116 103 97 // 86
 97, // Little Puzzle 2 In hex:  63 E5 66 61 In dec:  99 229 102 97 // 87
 97, // Little Puzzle 3 In hex:  63 CF 65 61 In dec:  99 207 101 97 // 88
 97  // Little Puzzle 4In hex:  D3 55 66 61 In dec:  211 85 102 97 // 89
 };

 const unsigned int PROGMEM character_colors[NUM_NUIDS][3] =
{{DISP_COLOR_BLUE, DISP_COLOR_YELLOW, DISP_COLOR_BLUE},                      // aa Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81           // 1
 {DISP_COLOR_BLUE, DISP_COLOR_YELLOW, DISP_COLOR_BLUE},                      // ab Cinderella 2 (Blue) - In hex:  C0 C9 B7 51 In dec:  192 201 183 81    // 2
 {DISP_COLOR_DARK_RED, DISP_COLOR_WHITE, DISP_COLOR_YELLOW},                 // ac Prince Charming - In hex:  C0 8A B7 51 In dec:  192 138 183 81        // 3
 {DISP_COLOR_FAIRY_LIGHT, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},     // ad Pocahontas - In hex:  36 C0 41 33 In dec:  54 192 65 51               // 4
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // ae Belle 1 (Yellow) - In hex:  C0 9A B7 51 In dec:  192 154 183 81       // 5
 {DISP_COLOR_MED_BLUE, DISP_COLOR_PINK, DISP_COLOR_WHITE},                   // af Fairy Godmother - In hex:  C0 AA B7 51 In dec:  192 170 183 81        // 6
 {DISP_COLOR_SPRING_GREEN, DISP_COLOR_SPRING_GREEN, DISP_COLOR_ORANGE_RED},  // ag Merida 1 (Green) - In hex:  C0 BA B7 51 In dec:  192 186 183 81       // 7
 {DISP_COLOR_TEAL, DISP_COLOR_TEAL, DISP_COLOR_TEAL},                        // ah Jasmine 1 (Green) - In hex:  40 D7 B7 51 In dec:  64 215 183 81       // 8
 {DISP_COLOR_DEEP_SKY_BLUE, DISP_COLOR_DARK_BLUE, DISP_COLOR_RED},           // ai Mulan 1 (Dress) - In hex:  30 E7 B7 51 In dec:  48 231 183 81         // 9
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_WHITE},                   // aj Lumiere - In hex:  E0 F6 B7 51 In dec:  224 246 183 81                // 10
 {DISP_COLOR_DARK_RED, DISP_COLOR_YELLOW, DISP_COLOR_GREEN},                 // ak Gus and Jaq - In hex:  A0 04 B8 51 In dec:  160 04 184 81             // 11
 {DISP_COLOR_PURPLE, DISP_COLOR_YELLOW, DISP_COLOR_PURPLE},                  // al Rapunzel 1 (Purple) - In hex:  40 58 B7 51 In dec:  64 88 183 81      // 12
 {DISP_COLOR_PINK, DISP_COLOR_YELLOW, DISP_COLOR_PINK},                      // am Aurora 1 (Pink) - In hex: In hex:  40 66 B7 51 In dec:  64 102 183 81 // 13
 {DISP_COLOR_WHITE, DISP_COLOR_ORANGE, DISP_COLOR_WHITE},                    // an Olaf 1 - In hex:  A0 C4 B7 51 In dec:  160 196 183 81                 // 14
 {DISP_COLOR_MINT, DISP_COLOR_MINT, DISP_COLOR_SEA_GREEN},                   // ao Tiana 1 (Green) - In hex:  20 D2 B7 51 In dec:  32 210 183 81         // 15
 {DISP_COLOR_DARK_RED, DISP_COLOR_BLUE, DISP_COLOR_YELLOW},                  // ap Snow White 1 (Dress) - In hex:  20 E1 B7 51 In dec:  32 225 183 81    // 16
 {DISP_COLOR_DARK_GREEN, DISP_COLOR_YELLOW, DISP_COLOR_DARK_RED},            // aq Hei Hei 1 - In hex:  C0 CD B6 51 In dec:  192 205 182 81              // 17
 {DISP_COLOR_WHITE, DISP_COLOR_PINK, DISP_COLOR_WHITE},                      // ar Pua 1 - In hex:  F0 BD B6 51 In dec:  240 189 182 81                  // 18
 {DISP_COLOR_BLUE, DISP_COLOR_BLUE, DISP_COLOR_BLUE},                        // as Genie 1 - In hex:  B0 DD B6 51 In dec:  176 221 182 81                // 19
 {DISP_COLOR_YELLOW, DISP_COLOR_RED, DISP_COLOR_WHITE},                      // at Mickey - In hex:  80 05 B8 51 In dec:  128 05 184 81                  // 20
 {DISP_COLOR_PURPLE, DISP_COLOR_YELLOW, DISP_COLOR_PINK},                    // au Daisy - In hex:  90 05 B8 51 In dec:  144 05 184 81                   // 21
 {DISP_COLOR_YELLOW, DISP_COLOR_WHITE, DISP_COLOR_BLUE},                     // av Donald - In hex:  20 F8 B7 51 In dec:  32 248 183 81                  // 22
 {DISP_COLOR_DARK_BLUE,DISP_COLOR_ORANGE, DISP_COLOR_DARK_GREEN},            // aw Goofy - In hex:  A0 E8 B7 51 In dec:  160 232 183 81                  // 23
 {DISP_COLOR_PINK, DISP_COLOR_WHITE, DISP_COLOR_PINK},                       // ax Minnie - In hex:  E0 DD B7 51 In dec:  224 221 183 81                 // 24
 {DISP_COLOR_MINT, DISP_COLOR_WHITE, DISP_COLOR_PINK},                       // ay Vanellope (WIR) - In hex:  30 C4 B7 51 In dec:  48 196 183 81
 {DISP_COLOR_GREEN, DISP_COLOR_PURPLE, DISP_COLOR_DARK_RED},                 // az Ariel (WIR) - In hex:  30 B4 B7 51 In dec:  48 180 183 81
 {DISP_COLOR_DARK_BLUE, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},       // ba Elsa (WIR) - In hex:  30 A6 B7 51 In dec:  48 166 183 81
 {DISP_COLOR_DARK_RED, DISP_COLOR_BLUE, DISP_COLOR_YELLOW},                  // bb Snow White (WIR) In hex:  30 96 B7 51 In dec:  48 150 183 81
 {DISP_COLOR_DEEP_SKY_BLUE, DISP_COLOR_DARK_BLUE, DISP_COLOR_RED},           // bc Mulan (WIR) In hex:  30 86 B7 51 In dec:  48 134 183 81
 {DISP_COLOR_BLUE, DISP_COLOR_WHITE, DISP_COLOR_BLUE},                       // bd Belle (WIR) In hex:  50 76 B7 51 In dec:  80 118 183 81
 {DISP_COLOR_PURPLE, DISP_COLOR_YELLOW, DISP_COLOR_PURPLE},                  // be Rapunzel (WIR) In hex:  C0 61 B7 51 In dec:  192 97 183 81
 {DISP_COLOR_FAIRY_LIGHT, DISP_COLOR_ORANGE_RED, DISP_COLOR_TEAL},           // bf Moana (WIR) In hex:  D0 51 B7 51 In dec:  208 81 183 81
 {DISP_COLOR_TEAL, DISP_COLOR_TEAL, DISP_COLOR_TEAL},                        // bg Jasmine (WIR) In hex:  D0 41 B7 51 In dec:  208 65 183 81
 {DISP_COLOR_SPRING_GREEN, DISP_COLOR_SPRING_GREEN, DISP_COLOR_ORANGE_RED},  // bh Merida (WIR) In hex:  D0 31 B7 51 In dec:  208 49 183 81
 {DISP_COLOR_DARK_BLUE, DISP_COLOR_DARK_CYAN, DISP_COLOR_DARK_VIOLET},       // bi Anna (WIR) In hex:  E0 21 B7 51 In dec:  224 33 183 81
 {DISP_COLOR_BLUE, DISP_COLOR_YELLOW, DISP_COLOR_BLUE},                      // bj Cinderella (WIR) In hex:  E0 11 B7 51 In dec:  224 17 183 81
 {DISP_COLOR_MINT, DISP_COLOR_MINT, DISP_COLOR_SEA_GREEN},                   // bk Tiana (WIR) In hex:  E0 FC B6 51 In dec:  224 252 182 81
 {DISP_COLOR_PINK, DISP_COLOR_YELLOW, DISP_COLOR_PINK},                      // bl Aurora 2 (WIR) In hex:  50 ED B6 51 In dec:  80 237 182 81
 {DISP_COLOR_FAIRY_LIGHT, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},     // bm Pocahontas (WIR) In hex:  80 DD B6 51 In dec:  128 221 182 81
 {DISP_COLOR_RED, DISP_COLOR_GREEN, DISP_COLOR_BLUE},                        // bn Flora, Fauna, Merryweather In hex:  D6 49 39 33 In dec:  214 73 57 51
 {DISP_COLOR_DARK_VIOLET, DISP_COLOR_WHITE, DISP_COLOR_YELLOW},              // bo Aurora 3 (gray, white) In hex:  06 B3 38 33 In dec:  06 179 56 51
 {DISP_COLOR_PURPLE, DISP_COLOR_MINT, DISP_COLOR_PURPLE},                    // bp Maleficent Dragon In hex:  76 1F 38 33 In dec:  118 31 56 51
 {DISP_COLOR_PURPLE, DISP_COLOR_MINT, DISP_COLOR_PURPLE},                    // bq Maleficent In hex:  E6 1A 37 33 In dec:  230 26 55 51
 {DISP_COLOR_PINK, DISP_COLOR_YELLOW, DISP_COLOR_PINK},                      // br Aurora 4 (Pink) In hex:  A6 90 36 33 In dec:  166 144 54 51
 {DISP_COLOR_BROWN, DISP_COLOR_YELLOW, DISP_COLOR_RED},                      // bs Prince Philip In hex:  96 16 37 33 In dec:  150 22 55 51
 {DISP_COLOR_DARK_BLUE, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},       // bt Ice Horse In hex:  96 1A 38 33 In dec:  150 26 56 51
 {DISP_COLOR_DARK_BLUE, DISP_COLOR_DARK_CYAN, DISP_COLOR_DARK_VIOLET},       // bu Queen Anna Frozen 2 In hex:  26 B0 38 33 In dec:  38 176 56 51
 {DISP_COLOR_TEAL, DISP_COLOR_BLACK, DISP_COLOR_FAIRY_LIGHT},                // bv Kristoff In hex:  16 46 39 33 In dec:  22 70 57 51
 {DISP_COLOR_WHITE, DISP_COLOR_ORANGE, DISP_COLOR_WHITE},                    // bw Olaf 2 In hex:  D6 E4 39 33 In dec:  214 228 57 51
 {DISP_COLOR_DARK_BLUE, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},       // bx Elsa Frozen 2 (49) In hex:  C6 72 3F 33 In dec:  198 114 63 51
 {DISP_COLOR_RED, DISP_COLOR_GREEN, DISP_COLOR_RED},                         // by Christmas In hex:  D6 BC 41 33 In dec:  214 188 65 51
 {DISP_COLOR_RED, DISP_COLOR_GREEN, DISP_COLOR_BLUE},                        // bz PJ Masks Tower - In hex:  36 2A 41 33 In dec:  54 42 65 51
 {DISP_COLOR_GREEN, DISP_COLOR_GREEN, DISP_COLOR_GREEN},                     // ca Cheeseburger 1 - In hex:  C6 4E 42 33 In dec:  198 78 66 51
 {DISP_COLOR_GREEN, DISP_COLOR_GREEN, DISP_COLOR_GREEN},                     // cb Cheeseburger 2 - In hex:  F6 DD 42 33 In dec:  246 221 66 51
 {DISP_COLOR_BLUE, DISP_COLOR_WHITE, DISP_COLOR_RED},                        // cc Cake 1 - In hex:  C0 74 B7 51 In dec:  192 116 183 81
 {DISP_COLOR_BLUE, DISP_COLOR_WHITE, DISP_COLOR_RED},                        // cd Cake 2 In hex:  D6 F2 3F 33 In dec:  214 242 63 51
 {DISP_COLOR_BLUE, DISP_COLOR_WHITE, DISP_COLOR_RED},                        // ce Cake 3 In hex:  26 8E 40 33 In dec:  38 142 64 51
 {DISP_COLOR_PINK, DISP_COLOR_BLUE, DISP_COLOR_GREEN},                       // cf Ice Cream 1 - In hex:  A0 B4 B7 51 In dec:  160 180 183 81
 {DISP_COLOR_PINK, DISP_COLOR_BLUE, DISP_COLOR_GREEN},                       // cg Ice Cream 2 - In hex:  A0 94 B7 51 In dec:  160 148 183 81
 {DISP_COLOR_PINK, DISP_COLOR_BLUE, DISP_COLOR_GREEN},                       // ch Ice Cream 3 - In hex:  A0 A4 B7 51 In dec:  160 164 183 81
 {DISP_COLOR_WHITE, DISP_COLOR_RED, DISP_COLOR_WHITE},                       // ci Fia 1 In hex:  1C 37 72 17 In dec:  28 55 114 23
 {DISP_COLOR_PURPLE, DISP_COLOR_PURPLE, DISP_COLOR_PINK},                    // cj Kaia 1 In hex:  3C 79 71 17 In dec:  60 121 113 23
 {DISP_COLOR_BLUE, DISP_COLOR_YELLOW, DISP_COLOR_BLUE},                      // ck Cinderella 1 (Blue) - Hex: E0 7A B7 51 Dec:  224 122 183 81
 {DISP_COLOR_TEAL, DISP_COLOR_TEAL, DISP_COLOR_TEAL},                        // cl Jasmine New (Green) - In hex:  C3 64 80 61  In dec:  195 100 128 97
 {DISP_COLOR_FAIRY_LIGHT, DISP_COLOR_DARK_TURQ, DISP_COLOR_FAIRY_LIGHT},     // cm Pocahontas New - In hex:  53 B6 80 61  In dec:  83 182 128 97
 {DISP_COLOR_DARK_RED, DISP_COLOR_BLUE, DISP_COLOR_YELLOW},                  // cn Snow White New (Dress) - In hex:  93 18 81 61  In dec:  147 24 129 97
 {DISP_COLOR_MINT, DISP_COLOR_MINT, DISP_COLOR_SEA_GREEN},                   // co Tiana New (Green) - In hex:  83 67 81 61  In dec:  131 103 129 97
 {DISP_COLOR_DEEP_SKY_BLUE, DISP_COLOR_DARK_BLUE, DISP_COLOR_RED},           // cp Mulan New (Dress) - In hex:  93 CA 81 61  In dec:  147 202 129 97
 {DISP_COLOR_PINK, DISP_COLOR_YELLOW, DISP_COLOR_PINK},                      // cq Aurora New (Pink) - In hex:  F3 F6 5B 61  In dec:  243 246 91 97
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // cr Belle New (Yellow) - In hex:  E3 6E 82 61  In dec:  227 110 130 97 // 70
 {DISP_COLOR_PURPLE, DISP_COLOR_TEAL, DISP_COLOR_PURPLE},                    // Carlita In hex:  33 C1 7A 61 In dec:  51 193 122 97 // 71
 {DISP_COLOR_WHITE, DISP_COLOR_BLACK, DISP_COLOR_WHITE},                     // Pandy In hex:  A3 A7 7B 61 In dec:  163 167 123 97 // 72
 {DISP_COLOR_PURPLE, DISP_COLOR_TEAL, DISP_COLOR_PINK},                      // Mercat 1 In hex:  A3 D0 79 61 In dec:  163 208 121 97 // 73
 {DISP_COLOR_PURPLE, DISP_COLOR_TEAL, DISP_COLOR_PINK},                      // Mercat 2 In hex:  83 B3 69 61 In dec:  131 179 105 97 // 74
 {DISP_COLOR_YELLOW, DISP_COLOR_TEAL, DISP_COLOR_PINK},                      // Box Cat In hex:  13 61 79 61 In dec:  19 97 121 97 // 75
 {DISP_COLOR_PURPLE, DISP_COLOR_RED, DISP_COLOR_PURPLE},                     // DJ Catnip In hex:  D3 C2 6A 61 In dec:  211 194 106 97 // 76
 {DISP_COLOR_TEAL, DISP_COLOR_PURPLE, DISP_COLOR_TEAL},                      // Catrat In hex:  43 6E 78 61 In dec:  67 110 120 97 // 77
 {DISP_COLOR_YELLOW, DISP_COLOR_GREEN, DISP_COLOR_RED},                      // Pillowcat In hex:  43 FF 5A 61 In dec:  67 255 90 97 // 78
 {DISP_COLOR_TEAL, DISP_COLOR_PURPLE, DISP_COLOR_WHITE},                     // Cupcake In hex:  43 E0 78 61 In dec:  67 224 120 97 // 79
 {DISP_COLOR_BLUE, DISP_COLOR_WHITE, DISP_COLOR_PINK},                       // Gabby In hex:  23 4C 7A 61 In dec:  35 76 122 97 // 80
 {DISP_COLOR_GREEN, DISP_COLOR_PINK, DISP_COLOR_WHITE},                      // Kitty Fairy In hex:  83 35 7B 61 In dec:  131 53 123 97 // 81
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // Big Puzzle 1 In hex:  33 88 6E 61 In dec:  51 136 110 97 // 82
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // Big Puzzle 2 In hex:  E3 00 6F 61 In dec:  227 00 111 97 // 83
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // Big Puzzle 3 In hex:  03 7C 6F 61 In dec:  03 124 111 97 // 84
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // Big Puzzle 4 In hex:  B3 F8 6F 61 In dec:  179 248 111 97 // 85
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // Little Puzzle 1 In hex:  43 74 67 61 In dec:  67 116 103 97 // 86
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // Little Puzzle 2 In hex:  63 E5 66 61 In dec:  99 229 102 97 // 87
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // Little Puzzle 3 In hex:  63 CF 65 61 In dec:  99 207 101 97 // 88
 {DISP_COLOR_YELLOW, DISP_COLOR_YELLOW, DISP_COLOR_YELLOW},                  // Little Puzzle 4 In hex:  D3 55 66 61 In dec:  211 85 102 97 // 89
 };

 

#define TIME_STEP 150

#define SS_PIN 10
#define RST_PIN 9


SoftwareSerial mySoftwareSerial(5, 6); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

byte readCard[4];
String My_ID = "********";  
String ID = "";

unsigned int track_num_req = 0;
unsigned int track_num_current = 0;

// data to send over USB
char dataString[5] = {0};

MFRC522 rfid(SS_PIN, RST_PIN); // Create MFRC522 instance

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];





void display_leds_three(unsigned int color_num_1, unsigned int color_num_2, unsigned int color_num_3)
{
   unsigned int i = 0;

  for(i = 0; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;
  FastLED.show();
  delay(35);
  
  for(i = FIRST_HOUSE_LED; i <= LAST_MUSIC_ROOM_LED; i++)
  {
    leds[i] = numToColor[color_num_1];
    FastLED.show();
    delay(35);
  }

  for(i = FIRST_BATHROOM_LED; i <= LAST_BEDROOM_LED; i++)
  {
    leds[i] = numToColor[color_num_2];
    FastLED.show();
    delay(35);
  }

  for(i = LAST_PLAYROOM_LED; i >= FIRST_TOP_FLOOR_LED; i--)
  {
    leds[i] = numToColor[color_num_3];
    FastLED.show();
    delay(35);
  }
  
}

void clear_leds()
{
  unsigned int i = 0;

  for(i = 0; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;
}

void display_leds_handle()
{
   unsigned int i = 0;
  
  for(i = 0; i < 19; i++)
  {
    leds[i] = numToColor[DISP_COLOR_TEAL];
    FastLED.show();
  }
  
}

void display_leds_house()
{
   unsigned int i = 0;

  for(i = 0; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;
  
  for(i = FIRST_HOUSE_LED; i < NUM_LEDS; i++)
  {
    leds[i] = numToColor[DISP_COLOR_BLUE];
  }
  FastLED.show();
}

void display_leds_house_tri()
{
   unsigned int i = 0;

  for(i = 0; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;
  
  for(i = FIRST_HOUSE_LED; i < NUM_LEDS; i+=3)
  {
    leds[i] = numToColor[DISP_COLOR_TEAL];
    leds[i+1] = numToColor[DISP_COLOR_PINK];
    leds[i+2] = numToColor[DISP_COLOR_PURPLE];
    FastLED.show();
    delay(40);
  }
  
}

void display_leds_house_five()
{
   unsigned int i = 0;

  for(i = 0; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;
  
  for(i = FIRST_HOUSE_LED; i < NUM_LEDS; i+=5)
  {
    leds[i] = numToColor[DISP_COLOR_TEAL];
    leds[i+1] = numToColor[DISP_COLOR_PINK];
    leds[i+2] = numToColor[DISP_COLOR_YELLOW];
    leds[i+3] = numToColor[DISP_COLOR_PURPLE];
    leds[i+4] = numToColor[DISP_COLOR_GREEN];
    FastLED.show();
    delay(40);
  }
  
}

void display_leds_music_room()
{
   unsigned int i = 0;

  for(i = 0; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;
  
  for(i = FIRST_MUSIC_ROOM_LED; i <= LAST_MUSIC_ROOM_LED; i++)
  {
    leds[i] = numToColor[DISP_COLOR_YELLOW];
    FastLED.show();
  }
  
}

void display_leds_playroom()
{
   unsigned int i = 0;

  for(i = 0; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;
  
  for(i = FIRST_PLAYROOM_LED; i <= LAST_PLAYROOM_LED; i++)
  {
    leds[i] = numToColor[DISP_COLOR_YELLOW];
    FastLED.show();
  }
  
}

void display_leds_bathroom()
{
   unsigned int i = 0;

  for(i = 0; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;
  
  for(i = FIRST_BATHROOM_LED; i <= LAST_BATHROOM_LED; i++)
  {
    leds[i] = numToColor[DISP_COLOR_YELLOW];
    FastLED.show();
  }
  
}

void display_leds_bedroom()
{
   unsigned int i = 0;

  for(i = 0; i < NUM_LEDS; i++)
      leds[i] = CRGB::Black;
  
  for(i = FIRST_BEDROOM_LED; i <= LAST_BEDROOM_LED; i++)
  {
    leds[i] = numToColor[DISP_COLOR_YELLOW];
    FastLED.show();
  }
  
}


uint8_t gHue = 0;
void display_rainbow()
{

  fill_rainbow( leds, NUM_LEDS, gHue, 7);  
     
  FastLED.show();
  delay(20);
  gHue+=5; // slowly cycle the "base color" through the rainbow
}







void setup()
{

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  pinMode(LED_PIN, OUTPUT);

  SPI.begin(); // Init SPI bus

 

  Serial.begin(9600);

  rfid.PCD_Init();    // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  rfid.PCD_DumpVersionToSerial();

  mySoftwareSerial.begin(9600);
  delay(2000);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(VOLUME_MUSIC);  //Set volume value. From 0 to 30

  clear_leds();
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}


unsigned int current_phase = 99;



void perform_phase_1()
{
  clear_leds();
  delay(50);
  
  // Play Gabby 1
  sprintf(dataString,"DOUG%02d",1); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(32000);                     // wait until movie ends
  
  // Light up house
  delay(2000);
  myDFPlayer.playMp3Folder(90); // Meow
  delay(100);
  display_leds_house_tri();
  delay(3000);

  // Play Gabby 2
  sprintf(dataString,"DOUG%02d",2); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(12000);                     // wait until movie ends

  // Light up mail box
  delay(2000);
  display_leds_handle();
  myDFPlayer.playMp3Folder(91); // Handle
  delay(3000);

  // Play Gabby 3
  sprintf(dataString,"DOUG%02d",3); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(94000);                     // wait until movie ends

  // Light up playroom
  delay(6000);
  display_leds_playroom();
  myDFPlayer.playMp3Folder(92); // Carlita
  delay(4000);

  // Play Gabby 4
  sprintf(dataString,"DOUG%02d",4); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(21000);                     // wait until movie ends

  // Light up house
  delay(2000);
  display_leds_house_five();
  myDFPlayer.playMp3Folder(93); // Shrink
  delay(100);
  delay(4000);
  display_leds_playroom();

  // Play Gabby 5
  sprintf(dataString,"DOUG%02d",5); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(13000);                     // wait until movie ends

}

void perform_phase_2()
{
  // Play Gabby 6
  sprintf(dataString,"DOUG%02d",6); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(25000);                     // wait until movie ends
}

void perform_phase_3()
{
  // Play Gabby 7
  sprintf(dataString,"DOUG%02d",7); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(52000);                     // wait until movie ends

  // Light up bathroom
  delay(6000);
  display_leds_bathroom();
  myDFPlayer.playMp3Folder(94); // Move Room
  delay(100);
  delay(4000);

  // Play Gabby 8
  sprintf(dataString,"DOUG%02d",8); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(38000);                     // wait until movie ends
}

void perform_phase_4()
{
  // Play Gabby 9
  sprintf(dataString,"DOUG%02d",9); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(51000);                     // wait until movie ends

  // Light up bedroom
  delay(6000);
  display_leds_bedroom();
  myDFPlayer.playMp3Folder(94); // Move Room
  delay(4000);

  // Play Gabby 10
  sprintf(dataString,"DOUG%02d",10); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(86000);                     // wait until movie ends
}

void perform_phase_5()
{
  // Play Gabby 11
  sprintf(dataString,"DOUG%02d",11); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(51000);                     // wait until movie ends

  // Light up music room
  delay(6000);
  display_leds_music_room();
  myDFPlayer.playMp3Folder(94); // Move Room
  delay(4000);

  // Play Gabby 12
  sprintf(dataString,"DOUG%02d",12); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(34000);                     // wait until movie ends
}

void perform_phase_6()
{
  // Play Rainbow Boogie
  myDFPlayer.playMp3Folder(96); // Rainbow Boogie
  display_rainbow();
  delay(30000);
  display_leds_house();
  delay(7500);
  display_rainbow();
  delay(14900);
  display_leds_house();
  delay(7500);
  display_rainbow();
  delay(14900);
  display_leds_house();
  delay(7500);
  display_rainbow();
  delay(14900);
  display_leds_house();
  delay(7500);
  display_rainbow();
  delay(14900);
  display_leds_house();
  delay(8000);
}

void perform_phase_7()
{
  // Play Gabby 13
  sprintf(dataString,"DOUG%02d",13); // Create string for movie 
  Serial.println(dataString);       // send the data
  clear_leds();
  delay(50);
  delay(38000);                     // wait until movie ends
}

void perform_phase_8()
{
  // Play Gabby 14
  sprintf(dataString,"DOUG%02d",14); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(57000);                     // wait until movie ends
}

void perform_phase_9()
{
  // Light up house rainbow
  delay(3000);
  display_leds_house_five();
  myDFPlayer.playMp3Folder(95); // End House
  delay(3000);

  // Play Gabby 15
  sprintf(dataString,"DOUG%02d",15); // Create string for movie 
  Serial.println(dataString);       // send the data
  delay(11000);                     // wait until movie ends
}




void perform_action(int matched_char)
{
  if ((current_phase == 99) && ((matched_char == 22) || (matched_char == 23) || (matched_char == 10)))
  {
    current_phase = 1;
    perform_phase_1();
  }
  else if ((current_phase == 1) && ((matched_char == 72) ||(matched_char == 22) || (matched_char == 23) || (matched_char == 10)))
  {
    current_phase = 2;
    perform_phase_2();
  }
  else if ((current_phase == 2) && ((matched_char == 82) || (matched_char == 86) ||(matched_char == 22) || (matched_char == 23) || (matched_char == 10)))
  {
    current_phase = 3;
    perform_phase_3();
  }
  else if ((current_phase == 3) && ((matched_char == 83) || (matched_char == 87) ||(matched_char == 22) || (matched_char == 23) || (matched_char == 10)))
  {
    current_phase = 4;
    perform_phase_4();
  }
  else if ((current_phase == 4) && ((matched_char == 84) || (matched_char == 88) ||(matched_char == 22) || (matched_char == 23) || (matched_char == 10)))
  {
    current_phase = 5;
    perform_phase_5();
  }
  else if ((current_phase == 5) && ((matched_char == 85) || (matched_char == 89) ||(matched_char == 22) || (matched_char == 23) || (matched_char == 10)))
  {
    current_phase = 6;
    perform_phase_6();
  }
  else if ((current_phase == 6) && (matched_char > 0))
  {
    current_phase = 7;
    perform_phase_7();
  }
  else if ((current_phase == 7) && (matched_char > 0))
  {
    current_phase = 8;
    perform_phase_8();
  }
  else if ((current_phase == 8) && (matched_char > 0))
  {
    current_phase = 98;
    perform_phase_9();
  }
}

int counter = 0;

void loop()
{

  counter = counter + 1;
  if (counter > 30)
  {
    Serial.println("HELLO THERE");
    counter = 0;
  }

  if (current_phase == 98)
     current_phase = 99;

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) 
  {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    int matched_character = -1;
    for(int i = 0; i < NUM_NUIDS; i++)
    {
      if ((((unsigned int) rfid.uid.uidByte[0]) == pgm_read_word_near(&character_nuid_one[i])) &&
          ((unsigned int) (rfid.uid.uidByte[1]) == pgm_read_word_near(&character_nuid_two[i])) &&
          ((unsigned int) (rfid.uid.uidByte[2]) == pgm_read_word_near(&character_nuid_three[i])) &&
          ((unsigned int) (rfid.uid.uidByte[3]) == pgm_read_word_near(&character_nuid_four[i])))
        matched_character = i + 1;
    }
  
  // We have a match
    if (matched_character > 0)
    {
      Serial.print("Matched number: ");
      Serial.print(matched_character);
      Serial.println();


      if (matched_character != track_num_current)
      {
        // perform action if something to perform
        perform_action(matched_character);
        delay(100);

        if ((current_phase == 99) && (matched_character < 82))
        {
           myDFPlayer.playMp3Folder(matched_character);
        
           track_num_current = matched_character;

           display_leds_three(pgm_read_word_near(&character_colors[matched_character - 1][0]), pgm_read_word_near(&character_colors[matched_character - 1][1]), pgm_read_word_near(&character_colors[matched_character - 1][2]));
        }
      }
      
    }
  }
  else Serial.println(F("Card read previously."));



  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  


  delayMicroseconds(100);

  
  delay(1);


  
}



/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

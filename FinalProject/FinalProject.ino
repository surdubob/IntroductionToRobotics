#include <Adafruit_Keypad.h>
#include <FastLED.h>
#include <Encoder.h>
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Eeprom_at24c256.h>
#include "MySequencer.h"
#include "Set.h"

//================ SOME CONSTANTS ===============================

#define BRIGHTNESS 20
#define SPLASH_DURATION 100
#define UPDATES_PER_SECOND 100

#define NOTE_LED 0
#define VELOCITY_LED 9
#define LEGATO_LED 18
#define GATE_LED 27

#define SAVE_LED 36
#define REC_LED 37
#define PLAY_LED 38

#define NOTE_BTN 1
#define VELOCITY_BTN 10
#define LEGATO_BTN 19
#define GATE_BTN 28

#define NOTE_COLOR     CRGB(255,128,213)
#define VELOCITY_COLOR CRGB(51, 204, 51)
#define LEGATO_COLOR   CRGB(255, 30, 30)
#define GATE_COLOR     CRGB(128, 179, 255)

#define KEYBOARD_NOTE_COLOR CRGB(255, 200, 200)
#define STEP_COLOR CRGB(255, 204, 255)

const int lineheight = 8;	//default font size

int midiChannel = 1;

//================ BUTTON MATRIX VARIABLES=======================

// unfortunately, the circuit for the button matrix is a bit inverted and the rows and columns are inverted
// this problem is solved by rearranging the keymap
const byte ROWS = 9; // rows
const byte COLS = 4; // columns

//define the symbols on the buttons of the keypads (the keymap)
byte keys[ROWS][COLS] = {
	{0, 9 , 18, 27},
	{1, 10, 19, 28},
	{2, 11, 20, 29},
	{3, 12, 21, 30},
	{4, 13, 22, 31},
	{5, 14, 23, 32},
	{6, 15, 24, 33},
	{7, 16, 25, 34},
	{8, 17, 26, 35},
};

byte rowPins[ROWS] = {38, 36, 34, 32, 30, 28, 26, 24, 22}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {37, 35, 33, 31}; //connect to the column pinouts of the keypad

//initialize an instance of class Keypad
Adafruit_Keypad buttonMatrix = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


//=============== OTHER BUTTON VARIABLES ========================

const int octPlusBtn = 39;
const int octMinBtn = 40;
const int saveBtn = 42;
const int recBtn = 41;
const int playBtn = 43;
const int shiftBtn = 44;

int octPlusBs = 1;
int octMinBs = 1;
int saveBs = 1;
int recBs = 1;
int playBs = 1;
int shiftBs = 1;

int octPlusLbs = 1;
int octMinLbs = 1;
int saveLbs = 1;
int recLbs = 1;
int playLbs = 1;
int shiftLbs = 1;

int octPlusLdt = 0;
int octMinLdt = 0;
int saveLdt = 0;
int recLdt = 0;
int playLdt = 0;
int shiftLdt = 0;

Encoder encDisplay(2, 3);
Encoder encBpm(18, 19);

const int btnDisplayEncoder = 46;
const int btnEncoderBpm = 45;

const int debounceDelay = 50;

//=================== LED VARIABLES =============================

#define MATRIX_LED_NUMBER 36
#define CONTROL_LED_NUMBER 3

const int DATA_PIN_MATRIX = 5;
const int DATA_PIN_CONTROL = 6;

CRGB leds[MATRIX_LED_NUMBER + CONTROL_LED_NUMBER];



const int bpmLed = 7;

//================== DISPLAY VARIABLES ==========================

const int res = 8;
const int dc = 9;
const int cs = 10;

Adafruit_ST7735 disp = Adafruit_ST7735(cs, dc, res);

// Color definitions
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE color(0,0,255)
#define RED color(255,0,0)
#define GREEN color(0,255,0)

//====================== EEPROM VARIABLES ==========================

Eeprom_at24c256 eeprom(0x50);

//=================== LOGIC-RELATED VARIABLES ========================

#define FS_DEFAULT_TEMPO 120
#define FS_DEFAULT_STEPS 32
#define FS_DEFAULT_MEMORY 1024
#define FS_MIN_TEMPO 40
#define FS_MAX_TEMPO 250
#define FS_MAX_STEPS 32

//the delay necesarry to be between two bpm changes from the encoder to be considered a fast change and change multiple values 
#define FAST_BPM_DELAY 80

// sequencer init
MySequencer *seq;

enum KeyboardModes {
	NOTE, VELOCITY, LEGATO, GATE
} currentKeyboardMode;

enum SequencerState {
	STOPPED, PLAYING, RECORDING
} currentSeqState;

int BPM = 120;
long lastBpmChange = 0;
long lastModeChange = 0;
long lastRecBlink = 0;
long lastScaleChange = 0;
long lastBpmBlink = 0;

Set keySet = Set();

String modeStrings[4] = {"Note", "Velocity", "Legato", "Gate"};

CRGB* modeLeds[4] = {&leds[NOTE_LED], &leds[VELOCITY_LED], &leds[LEGATO_LED], &leds[GATE_LED]};


#define SCALE_NUMBER 10

enum Scales {
	FULL, NATURAL_MINOR, MAJOR, DORIAN, PHRYGIAN, MIXOLYDIAN, MELODIC_MINOR, HARMONIC_MINOR, LYDIAN, BLUES
} currentScale;

String scaleStrings[10] = {
	"FULL", "NATURAL_MINOR", "MAJOR", "DORIAN", "PHRYGIAN", "MIXOLYDIAN", "MELODIC_MINOR", "HARMONIC_MINOR", "LYDIAN", "BLUES"
};


int naturalMinorScale[8] = {36, 38, 39, 41, 43, 44, 46, 48};
int majorScale[8] = {36, 38, 40, 41, 43, 45, 47, 48};
int dorianScale[8] = {36, 38, 39, 41, 43, 45, 46, 48};
int phrygianScale[8] = {36, 37, 39, 41, 43, 44, 46, 48};
int mixolydianScale[8] = {36, 38, 40, 41, 43, 45, 46, 48};
int melodicMinorScale[8] = {36, 38, 39, 41, 43, 45, 47, 48};
int harmonicMinorScale[8] = {36, 38, 39, 41, 43, 44, 47, 48};
int lydianScale[8] = {36, 38, 40, 42, 43, 45, 47, 48};
int bluesScale[8] = {36, 39, 41, 42, 43, 46, 48, 48};

int *scalesVector[SCALE_NUMBER];

int baseNote = 36;

int octaveModifier = 0;
int semitonesPerOctave = 12;

bool changingScale = false;

int stepPage = 1;

void setup() {
	Serial.begin(115200);
	Serial2.begin(31250);

	initVars();

	initLeds();
	initDisplay();
	showSplashScreen();
	showStartupAnimation();
	clearScreen();

	initButtons();
	initSequencer();

	currentKeyboardMode = NOTE;
	currentSeqState = STOPPED;
	currentScale = FULL;

	setOriginalModeColors();
	for (int i = 0; i < 4; i++) {
		if (i != 0)
			modeLeds[i]->fadeLightBy( 200 );
	}
	leds[SAVE_LED] = CRGB::White;
	leds[REC_LED] = CRGB::Red;
	leds[PLAY_LED] = CRGB::White;

	for (int i = 1; i < 18; i++) {
		if (i != 9) {
			leds[i] = CRGB::Black;
		}
	}
	leds[2] = leds[3] = leds[5] = leds[6] = leds[7] = NOTE_COLOR;
	for (int i = 10; i < 18; i++) {
		leds[i] = NOTE_COLOR;
	}
	for (int i = 1; i < 9; i++) {
		leds[2 * 9 + i] = leds[3 * 9 + i] = STEP_COLOR;
	}

	FastLED.show();

	lastBpmChange = millis();
	lastModeChange = millis();
	lastScaleChange = millis();
}

void loop() {
	readEncoders();
	if (millis() - lastBpmChange < 100) {
		displayBpm();
	}
	if (millis() - lastModeChange < 100) {
		displayMode();
	}
	if (millis() - lastScaleChange < 100) {
		displayScale();
	}
	if(millis() - lastBpmBlink > 100) {
		digitalWrite(bpmLed, LOW);
		if (currentSeqState == RECORDING) {
			leds[REC_LED] = CRGB::Black;
			FastLED.show();
		}
	}
	debounceButtons();


	switch (currentKeyboardMode) {
		case NOTE:

			break;
		case VELOCITY:

			break;
		case LEGATO:

			break;
		case GATE:

			break;
	}

	while (buttonMatrix.available()) {
		keypadEvent e = buttonMatrix.read();
		//Serial.print((int)e.bit.KEY);
		if (e.bit.EVENT == KEY_JUST_PRESSED) {
			//Serial.println(" pressed");
			keySet.insert((int)e.bit.KEY);
		}
		else if (e.bit.EVENT == KEY_JUST_RELEASED) {
			//Serial.println(" released");
			keySet.remove((int)e.bit.KEY);
		}
		keyEvent(e);
	}

	buttonMatrix.tick();
	seq->run();
}


//======================== INIT FUNCTIONS ==========================

void initButtons() {
	pinMode(octPlusBtn, INPUT_PULLUP);
	pinMode(octMinBtn, INPUT_PULLUP);
	pinMode(saveBtn, INPUT_PULLUP);
	pinMode(recBtn, INPUT_PULLUP);
	pinMode(playBtn, INPUT_PULLUP);
	pinMode(shiftBtn, INPUT_PULLUP);
	pinMode(btnDisplayEncoder, INPUT_PULLUP);
	pinMode(btnEncoderBpm, INPUT_PULLUP);

	buttonMatrix.begin();
}

void initLeds() {
	FastLED.addLeds<NEOPIXEL, DATA_PIN_MATRIX>(leds, 0, MATRIX_LED_NUMBER);
	FastLED.addLeds<NEOPIXEL, DATA_PIN_CONTROL>(leds, MATRIX_LED_NUMBER, CONTROL_LED_NUMBER);
	FastLED.setBrightness(BRIGHTNESS);

	pinMode(bpmLed, OUTPUT);
}

void initDisplay() {
	disp.initR(INITR_GREENTAB);
	disp.setRotation(3);
	disp.fillScreen(ST77XX_BLACK);
	disp.setTextWrap(false);
}

void initSequencer() {
	seq = new MySequencer(120);
	seq->setStepNumber(8);
	seq->setMidiHandler(sendMidi);
	seq->setStepHandler(seqStep);
	seq->setBeatHandler(beatCallback);

	Serial.println("inainte");
	seq->getSequence()->addNote(new Note(60));
	seq->getSequence()->addNote(new Note(36));
	seq->getSequence()->addNote(new Note(36));
	seq->getSequence()->addNote(new Note(36));
	seq->getSequence()->addNote(new Note(48));
	seq->getSequence()->addNote(new Note(36));
	seq->getSequence()->addNote(new Note(36));
	seq->getSequence()->addNote(new Note(36));

	seq->getSequence()->printNotes();
}

void initVars() {
	scalesVector[0] = NULL;
	scalesVector[1] = naturalMinorScale;
	scalesVector[2] = dorianScale;
	scalesVector[3] = phrygianScale;
	scalesVector[4] = mixolydianScale;
	scalesVector[5] = melodicMinorScale;
	scalesVector[6] = harmonicMinorScale;
	scalesVector[7] = lydianScale;
	scalesVector[8] = bluesScale;
}

//======================= EVENT CALLBACKS ===================================

void buttonEventOctPlus() {
	Serial.println("Oct+");
	octaveModifier++;
}

void buttonEventOctMinus() {
	Serial.println("Oct-");
	octaveModifier--;
}

void buttonEventPlayPause() {
	if (currentSeqState == STOPPED) {
		currentSeqState = PLAYING;
		seq->start();
		leds[PLAY_LED] = CRGB::Green;
	} else {
		currentSeqState = STOPPED;
		seq->pause();
		leds[PLAY_LED] = CRGB::Yellow;
	}
	FastLED.show();
}

void buttonEventRecord() {
	Serial.println("Record");
	if (currentSeqState == RECORDING) {
		currentSeqState = PLAYING;
	} else {
		currentSeqState = RECORDING;
	}

}

void buttonEventSave() {
	Serial.println(F("Save/Stop"));
	if(currentSeqState = PLAYING) {
		seq->stop();
		leds[PLAY_LED] = CRGB::White;
		currentSeqState = STOPPED;
		FastLED.show();
	}
}

void buttonEventShift() {
	changingScale = !changingScale;
}

// Debounce all the buttons that are not in the matrix
void debounceButtons() {
	int r_octPlusBs = digitalRead(octPlusBtn);
	int r_octMinBs = digitalRead(octMinBtn);
	int r_saveBs = digitalRead(saveBtn);
	int r_recBs = digitalRead(recBtn);
	int r_playBs = digitalRead(playBtn);
	int r_shiftBs = digitalRead(shiftBtn);

	if (r_octPlusBs != octPlusLbs) {
		octPlusLdt = millis();
	}
	if (r_octMinBs != octMinLbs) {
		octMinLdt = millis();
	}
	if (r_saveBs != saveLbs) {
		saveLdt = millis();
	}
	if (r_recBs != recLbs) {
		recLdt = millis();
	}
	if (r_playBs != playLbs) {
		playLdt = millis();
	}
	if (r_shiftBs != shiftLbs) {
		shiftLdt = millis();
	}

	if (millis() - octPlusLdt > debounceDelay) {
		if (r_octPlusBs != octPlusBs) {
			octPlusBs = r_octPlusBs;
			if (octPlusBs == HIGH) {
				buttonEventOctPlus();
			}
		}
	}

	if (millis() - octMinLdt > debounceDelay) {
		if (r_octMinBs != octMinBs) {
			octMinBs = r_octMinBs;
			if (octMinBs == HIGH) {
				buttonEventOctMinus();
			}
		}
	}

	if (millis() - saveLdt > debounceDelay) {
		if (r_saveBs != saveBs) {
			saveBs = r_saveBs;
			if (saveBs == HIGH) {
				buttonEventSave();
			}
		}
	}

	if (millis() - recLdt > debounceDelay) {
		if (r_recBs != recBs) {
			recBs = r_recBs;
			if (recBs == HIGH) {
				buttonEventRecord();
			}
		}
	}

	if (millis() - playLdt > debounceDelay) {
		if (r_playBs != playBs) {
			playBs = r_playBs;
			if (playBs == HIGH) {
				buttonEventPlayPause();
			}
		}
	}

	if (millis() - shiftLdt > debounceDelay) {
		if (r_shiftBs != shiftBs) {
			shiftBs = r_shiftBs;
			if (shiftBs == HIGH) {
				buttonEventShift();
			}
		}
	}

	octPlusLbs = r_octPlusBs;
	octMinLbs = r_octMinBs;
	saveLbs = r_saveBs;
	recLbs = r_recBs;
	playLbs = r_playBs;
	shiftLbs = r_shiftBs;
}

void keyEvent(keypadEvent e) {
	switch ((int)e.bit.KEY) {
		case 0:	// Note key ==============================================================================
			if (e.bit.EVENT == KEY_JUST_PRESSED) {
				currentKeyboardMode = NOTE;
				lastModeChange = millis();
				setOriginalModeColors();
				for (int i = 0; i < 4; i++) {
					if (i != 0)
						modeLeds[i]->fadeLightBy( 200 );
				}
				updateScaleLeds();
				for (int i = 1; i < 9; i++) {
					leds[2 * 9 + i] = leds[3 * 9 + i] = STEP_COLOR;
				}

				FastLED.show();
			}
			break;
		case 1:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][0] + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][0] + (octaveModifier * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 2:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(37 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(37 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][1] + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][1] + (octaveModifier * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 3:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(39 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(39 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][2] + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][2] + (octaveModifier * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 4:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:

						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][3] + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][3] + (octaveModifier * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 5:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(42 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(42 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][4] + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][4] + (octaveModifier * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 6:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(44 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(44 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][5] + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][5] + (octaveModifier * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 7:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(46 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(46 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][6] + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][6] + (octaveModifier * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 8:
			switch (currentScale) {
				case FULL:

					break;
				case NATURAL_MINOR:
				case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
					if (!keySet.existsOtherThan(e.bit.KEY)) {
						if (e.bit.EVENT == KEY_JUST_PRESSED) {
							sendNoteOn(scalesVector[currentScale][7] + (octaveModifier * semitonesPerOctave), 80);
						} else {
							sendNoteOff(scalesVector[currentScale][7] + (octaveModifier * semitonesPerOctave));
						}

					}
					break;
			}
			break;
		case 9: // Velocity key =============================================================================
			if (e.bit.EVENT == KEY_JUST_PRESSED) {
				currentKeyboardMode = VELOCITY;
				lastModeChange = millis();
				setOriginalModeColors();
				for (int i = 0; i < 4; i++) {
					if (i != 1)
						modeLeds[i]->fadeLightBy( 200 );
				}
				for (int i = 1; i < 9; i++) {
					leds[i] = leds[1 * 9 + i] = VELOCITY_COLOR;
					leds[2 * 9 + i] = leds[3 * 9 + i] = STEP_COLOR;
				}
				FastLED.show();
			}
			break;
		case 10:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(36 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(36 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][0] + ((octaveModifier - 1) * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][0] + ((octaveModifier - 1) * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 11:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(38 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(38 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][1] + ((octaveModifier - 1) * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][1] + ((octaveModifier - 1) * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 12:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(40 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(40 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][2] + ((octaveModifier - 1) * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][2] + ((octaveModifier - 1) * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 13:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(41 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(41 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][3] + ((octaveModifier - 1) * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][3] + ((octaveModifier - 1) * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 14:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(43 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(43 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][4] + ((octaveModifier - 1) * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][4] + ((octaveModifier - 1) * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 15:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(45 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(45 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][5] + ((octaveModifier - 1) * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][5] + ((octaveModifier - 1) * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 16:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(47 + (octaveModifier * semitonesPerOctave), 80);
							} else {
								sendNoteOff(47 + (octaveModifier * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][6] + ((octaveModifier - 1) * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][6] + ((octaveModifier - 1) * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 17:
			if (currentKeyboardMode == NOTE) {
				switch (currentScale) {
					case FULL:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(36 + ((octaveModifier + 1) * semitonesPerOctave), 80);
							} else {
								sendNoteOff(36 + ((octaveModifier + 1) * semitonesPerOctave));
							}
						}
						break;
					case NATURAL_MINOR:
					case MAJOR:
					case DORIAN:
					case PHRYGIAN:
					case MIXOLYDIAN:
					case MELODIC_MINOR:
					case HARMONIC_MINOR:
					case LYDIAN:
					case BLUES:
						if (!keySet.existsOtherThan(e.bit.KEY)) {
							if (e.bit.EVENT == KEY_JUST_PRESSED) {
								sendNoteOn(scalesVector[currentScale][7] + ((octaveModifier - 1) * semitonesPerOctave), 80);
							} else {
								sendNoteOff(scalesVector[currentScale][7] + ((octaveModifier - 1) * semitonesPerOctave));
							}

						}
						break;
				}
			}
			break;
		case 18: // Legato key =============================================================================
			if (e.bit.EVENT == KEY_JUST_PRESSED) {
				currentKeyboardMode = LEGATO;
				lastModeChange = millis();
				setOriginalModeColors();
				for (int i = 0; i < 4; i++) {
					if (i != 2)
						modeLeds[i]->fadeLightBy( 200 );
				}
				for (int i = 1; i < 9; i++) {
					leds[i] = leds[1 * 9 + i] = LEGATO_COLOR;
					leds[2 * 9 + i] = leds[3 * 9 + i] = STEP_COLOR;
				}
				FastLED.show();
			}
			break;
		case 19:
			
			break;
		case 20:

			break;
		case 21:

			break;
		case 22:

			break;
		case 23:

			break;
		case 24:

			break;
		case 25:

			break;
		case 26:

			break;
		case 27: //Gate key =============================================================================
			if (e.bit.EVENT == KEY_JUST_PRESSED) {
				currentKeyboardMode = GATE;
				lastModeChange = millis();
				setOriginalModeColors();
				for (int i = 0; i < 4; i++) {
					if (i != 3)
						modeLeds[i]->fadeLightBy( 200 );
				}
				for (int i = 1; i < 9; i++) {
					leds[i] = leds[1 * 9 + i] = GATE_COLOR;
					leds[2 * 9 + i] = leds[3 * 9 + i] = STEP_COLOR;
				}
				FastLED.show();
			}
			break;
		case 28:

			break;
		case 29:

			break;
		case 30:

			break;
		case 31:

			break;
		case 32:

			break;
		case 33:

			break;
		case 34:

			break;
		case 35:

			break;
		case 36:

			break;
	}
}

void encoderDisplay(bool dir) {
	if (changingScale) {
		if (dir) {
			if (currentScale < SCALE_NUMBER - 1) {
				currentScale = currentScale + 1;
				lastScaleChange = millis();
				updateScaleLeds();
			}
		} else {
			if (currentScale > 0) {
				currentScale = currentScale - 1;
				lastScaleChange = millis();
				updateScaleLeds();
			}
		}
	}
}

void encoderBpm(bool dir) {
	if (dir) {
		if (BPM < FS_MAX_TEMPO) {
			if (millis() - lastBpmChange < FAST_BPM_DELAY) {
				BPM += 5;
			} else {
				BPM++;
			}
			lastBpmChange = millis();
		}
	} else {
		if (BPM > FS_MIN_TEMPO) {
			if (millis() - lastBpmChange < FAST_BPM_DELAY) {
				BPM -= 5;
			} else {
				BPM--;
			}
			lastBpmChange = millis();
		}
	}
	seq->setTempo(BPM);
}


//=================== MIDI SEQUENCER CALLBACKS ==============================

// called when the step position changes.
void seqStep(int current, int last) {

}

// the callback that will be called by the sequencer when it needs to send midi commands.
void sendMidi(byte channel, byte command, byte arg1, byte arg2) {
	if (command < 128) {
		// shift over command
		command <<= 4;
		// add channel to the command
		command |= channel;
	}

	// send MIDI data
	Serial2.write(command);
	Serial2.write(arg1);
	Serial2.write(arg2);
}

bool blinkBeat = true;

void beatCallback() {
	// blink on even beats
	if (blinkBeat) {	
		analogWrite(bpmLed, 5);
		leds[REC_LED] = CRGB::Red;
		FastLED.show();
		lastBpmBlink = millis();
	}

	blinkBeat = !blinkBeat;
}

//=================== ENCODER UTILITIES ==================================

long oldDisplayEncoderPosition = 0;
long oldBpmEncoderPosition = 0;

void readEncoders() {
	long newDisplayEncoderPosition = encDisplay.read();
	long newBpmEncoderPosition = encBpm.read();

	if (newDisplayEncoderPosition != oldDisplayEncoderPosition && newDisplayEncoderPosition % 4 == 0) {
		if (newDisplayEncoderPosition > oldDisplayEncoderPosition) {
			encoderDisplay(false);
		} else {
			encoderDisplay(true);
		}
		oldDisplayEncoderPosition = newDisplayEncoderPosition;
	}
	if (newBpmEncoderPosition != oldBpmEncoderPosition && newBpmEncoderPosition % 4 == 0) {
		if (newBpmEncoderPosition > oldBpmEncoderPosition) {
			encoderBpm(true);
		} else {
			encoderBpm(false);
		}
		oldBpmEncoderPosition = newBpmEncoderPosition;
	}
}

//==================== UTILITY FUNCTIONS =================================

uint16_t color(uint16_t R, uint16_t G, uint16_t B) {
	return (((B & 0xF8) << 8) | ((G & 0xFC) << 3) | (R >> 3));
}

void displayBpm() {
	disp.setTextColor(WHITE, BLACK);
	disp.setTextSize(1);
	disp.setCursor(10, lineheight * 1);
	disp.print(F("BPM:"));
	disp.print(BPM);
	disp.print("  ");
}

void displayMode() {
	disp.setTextColor(WHITE, BLACK);
	disp.setTextSize(1);
	disp.setCursor(10, lineheight * 2);
	disp.print(F("MODE:"));
	disp.print(modeStrings[currentKeyboardMode]);
	disp.print(F("      "));
}

void displayScale() {
	disp.setTextColor(WHITE, BLACK);
	disp.setTextSize(1);
	disp.setCursor(10, lineheight * 3);
	disp.print("Scale:");
	disp.print(scaleStrings[currentScale]);
	disp.print("           ");
}

void showSplashScreen() {
	disp.setTextSize(3);
	disp.setTextColor(color(255, 51, 0));
	disp.setCursor(15, 20);
	disp.print("Welcome!");
	disp.setTextSize(1);
	disp.setTextColor(WHITE);
	disp.setCursor(10, 60);
	disp.print(F("Final project for"));
	disp.setCursor(20, 72);
	disp.print(F("@UnibucRobotics"));
	disp.setCursor(10, 95);
	disp.print(F("by Alexandru Surdu Bob"));
}

void clearScreen() {
	disp.fillScreen(BLACK);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex) {
	uint8_t brightness = 255;

	for ( int i = 0; i < MATRIX_LED_NUMBER + CONTROL_LED_NUMBER; i++) {
		leds[i] = ColorFromPalette( RainbowColors_p, colorIndex, BRIGHTNESS + 50, NOBLEND);
		colorIndex += 3;
	}
}

void showStartupAnimation() {
	long animStartTime = millis();
	while (millis() - animStartTime < SPLASH_DURATION) {
		static uint8_t startIndex = 0;
		startIndex = startIndex + 1; /* motion speed */

		FillLEDsFromPaletteColors( startIndex);

		FastLED.show();
		FastLED.delay(1000 / UPDATES_PER_SECOND);
	}
	FastLED.clear();
	FastLED.show();
}

void setOriginalModeColors() {
	leds[NOTE_LED] = NOTE_COLOR;
	leds[VELOCITY_LED] = VELOCITY_COLOR;
	leds[LEGATO_LED] = LEGATO_COLOR;
	leds[GATE_LED] = GATE_COLOR;
}

void sendNoteOn(int note, int velocity) {
	sendMidi(0x0, 0x9, note, velocity);
}

void sendNoteOff(int note) {
	sendMidi(0x0, 0x8, note, 0x0);
}

void allNotesOff() {
	sendMidi(0x0, 123, 0x0, 0x0);
}

void updateScaleLeds() {
	switch (currentScale) {
		case FULL:
			for (int i = 1; i < 18; i++) {
				if (i != 9) {
					leds[i] = CRGB::Black;
				}
			}
			leds[2] = leds[3] = leds[5] = leds[6] = leds[7] = NOTE_COLOR;
			for (int i = 10; i < 18; i++) {
				leds[i] = NOTE_COLOR;
			}
			break;
		case NATURAL_MINOR:
		case MAJOR:
		case DORIAN:
		case PHRYGIAN:
		case MIXOLYDIAN:
		case MELODIC_MINOR:
		case HARMONIC_MINOR:
		case LYDIAN:
			for (int i = 1; i < 18; i++) {
				if (i != 9) {
					leds[i] = NOTE_COLOR;
				}
			}
			leds[1] = leds[8] = leds[10] = leds[17] = NOTE_COLOR - CRGB(0, 60, 60);
			break;
		case BLUES:
			break;

	}
	FastLED.show();
}

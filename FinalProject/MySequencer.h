#ifndef MYSEQUENCER_H
#define MYSEQUENCER_H

//#include <MIDI.h>

// MIDI standard clock ticks per beat
#define CLOCKS_PER_BEAT 24


// MIDIcallback
//
// This defines the MIDI callback function format that is required by the
// sequencer.
//
// Most of the time these arguments will represent the following:
//
// channel: midi channel
// command: note on or off (0x9 or 0x8)
// arg1: pitch value
// arg1: velocity value
//
// It's possible that there will be other types of MIDI messages sent
// to this callback in the future, so please check the command sent if
// you are doing something other than passing on the MIDI messages to
// a MIDI library.
//
typedef void (*MidiCallback) (byte channel, byte command, byte arg1, byte arg2);

// StepCallback
//
// This defines the format of the step callback function that will be used
// by the sequencer. This callback will be called with the current
// step position and last step position whenever the step changes.
//
typedef void (*StepCallback) (int current, int last);

typedef void (*BeatCallback) ();

class Note {
	public:

		Note() {
			_pitch = 36;
			_velocity = 0;
			_legato = false;
			_gate = 4;
		}

		Note(int pitch) {
			_pitch = pitch;
			_velocity = 127;
			_legato = false;
			_gate = 4;
		}

		Note(int pitch, int velocity) {
			_pitch = pitch;
			_velocity = velocity;
			_legato = false;
			_gate = 4;
		}
		
		Note(int pitch, int velocity, int gate) {
			_pitch = pitch;
			_velocity = velocity;
			_legato = false;
			_gate = gate;
		}

		Note(int pitch, int velocity, int gate, bool legato) {
			_pitch = pitch;
			_velocity = velocity;
			_legato = legato;
			_gate = gate;
		}

		int getPitch() {
			return _pitch;
		}

		void setPitch(int pitch) {
			_pitch = pitch;
		}

		void setVelocity(int velocity) {
			_velocity = velocity;
		}

		int getVelocity() {
			return _velocity;
		}

		void setLegato(bool legato) {
			_legato = legato;
		}

		bool getLegato() {
			return _legato;
		}

		void setGate(int gate) {
			_gate = gate;
		}

		int getGate() {
			return _gate;
		}

	private:
		int _pitch;
		int _velocity;
		bool _legato;
		int _gate;	//values between 0 and 12
};

class Sequence {
	public:

		Sequence() {
			Sequence(16);
		}
	
		Sequence(int stepNumber) {
			_stepNumber = stepNumber;
			_setStepNumber = 0;
		}
	
		void setStepNumber(int sn) {
			_stepNumber = sn;
		}
	
		int getStepNumber() {
			return _stepNumber;
		}

		void setStep(int pitch, int index) {
			_notes[index]->setPitch(pitch);
		}
		
		void setStep(Note *note, int index) {
			_notes[index] = note;
			if(index > _setStepNumber) {
				_setStepNumber = index;
			}
		}

		void addNote(Note *newNote) {
			if(_setStepNumber < _stepNumber) {
				_notes[_setStepNumber] = newNote;
				_setStepNumber++;
			}
		}

		Note* getNote(int index) {
			return _notes[index];
		}

		void printNotes() {
			for(int i = 0; i < _stepNumber; i++) {
				Serial.print(getNote(i)->getPitch());
				Serial.print("  ");
			}
			Serial.println();
		}

		int _setStepNumber;
	private:
		int _stepNumber;		
		Note *_notes[32];
		
};

class MySequencer {
	public:

		MySequencer() {
			MySequencer(120);
		}
	
		MySequencer(int tempo) {
			_sequence = new Sequence();
			_running = false;
			setTempo(tempo);
			_currentStep = _lastStep = 0;
			_lastTempoTick = micros();
			_tickTimeInterval = 20833;
			_sequence->_setStepNumber = 0;
		}

		void run() {
			if(micros() - _lastTempoTick > _tickTimeInterval) {
				_sendClockSignal();
				_clockTickNumber %= (CLOCKS_PER_BEAT / 2);

				if(_running) {
					if(_clockTickNumber == 0) {
						if(_firstTime) {
							_sendNoteOff(_sequence->getNote(0)->getPitch());
							_firstTime = false;
						}
						if(_sequence->getNote(_lastStep)->getLegato()) {
							_sendNoteOff(_sequence->getNote(_lastStep)->getPitch());
							_sendNoteOff(_sequence->getNote(_currentStep)->getPitch());
						}
						_lastStep = _currentStep;
						_currentStep++;
						if(_currentStep >= (_sequence->getStepNumber())) {
							_currentStep = 0;
						}
						_sendNoteOn(_sequence->getNote(_currentStep));
						//Serial.println(_sequence->getNote(_currentStep)->getPitch());
						_step_cb(_currentStep, _lastStep);
					} 
					if(_sequence->getNote(_currentStep)->getGate() == _clockTickNumber - _noteOnTick && !_sequence->getNote(_currentStep)->getLegato()) {
						_sendNoteOff(_sequence->getNote(_currentStep)->getPitch());
					}
				} else if(!_sentNoteOffLast) {
					_sendNoteOff(_sequence->getNote(_lastStep)->getPitch());
					_sendNoteOff(_sequence->getNote(_currentStep)->getPitch());
					_sentNoteOffLast = true;
					if(_stp) {
						_currentStep = 0;
						_lastStep = 0;
						_stp = false;
					}
				}
				if(_clockTickNumber == 0){
					_beat_cb();
				}
			}
		}

		void start() {
			_running = true;
			if(_currentStep == 0) {
				_sendNoteOn(_sequence->getNote(_currentStep));
				//_sendClockSignal();
				_step_cb(_currentStep, _lastStep);
				_firstTime = true;
			}
			_lastTempoTick = micros();
		}

		void stop() {
			_running = false;
			_sentNoteOffLast = false;
			_stp = true;
			_firstTime = true;
		}

		void pause() {
			_running = false;
			_sentNoteOffLast = false;
		}
		void resume() {
			_running = true;
		}

		void setTempo(int tempo) {
			_tempo = tempo;
			_setTickTimeInterval(_calculateIntervalMicroSecs(tempo));
		}

		int getTempo() {
			return _tempo;
		}

		void setMidiHandler(MidiCallback cb) {
			_midi_cb = cb;
		}

		void setStepHandler(StepCallback cb) {
			_step_cb = cb;
		}

		void setBeatHandler(BeatCallback cb) {
			_beat_cb = cb;
		}

		void setStepNumber(int steps) {
			if(steps > 0 && steps <= 32) {
				_sequence->setStepNumber(steps);
			}
		}

		Sequence* getSequence() {
			return _sequence;
		}

		int getCurrentStep() {
			return _currentStep;
		}

	private:
		Sequence *_sequence;
	
		int _tempo;
		long _tickTimeInterval;
		long _lastTempoTick;
		long _clockTickNumber;
		long _noteOnTick;
		
		int _currentStep;
		int _lastStep;

		bool _running;

		bool _sentNoteOffLast = false;

		bool _stp = false;
		bool _firstTime = true;

		MidiCallback _midi_cb;
		StepCallback _step_cb;
		BeatCallback _beat_cb;

		void _sendClockSignal() {
			
			// bail if the midi callback isn't set
			if (! _midi_cb) 
				return;			
			
			// tick
			//_midi_cb(0x0, 0xF8, 0x0, 0x0);
			Serial2.write(0xF8);
			
			_lastTempoTick = micros();
			_clockTickNumber++;
		}

		long _calculateIntervalMicroSecs(int bpm) {
			// Take care about overflows!
			float interm = 60.0 / bpm / CLOCKS_PER_BEAT;
			long val = interm * 1000000L;
			return val;
		}

		void _setTickTimeInterval(long ti) {
			_tickTimeInterval = ti;
		}


		void _sendNoteOn(Note *note) {
			_midi_cb(0x0, 0x9, note->getPitch(), note->getVelocity());
			_noteOnTick = _clockTickNumber;
		}

		void _sendNoteOff(Note *note) {
			_midi_cb(0x0, 0x8, note->getPitch(), 0x0);
		}

		void _sendNoteOff(int pitch) {
			_midi_cb(0x0, 0x8, pitch, 0x0);
		}
};

#endif

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
			_velocity = 127;
		}

		Note(int pitch) {
			_pitch = pitch;
		}

		Note(int pitch, int velocity) {
			_pitch = pitch;
			_velocity = velocity;
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

	private:
		int _pitch;
		int _velocity;
};

class Sequence {
	public:
	Sequence(int stepNumber) {
		_length = stepNumber;
	}

	private:
		int _length;
		Note _notes[32];
		
};

class MySequencer {
	public:

		MySequencer() {
			MySequencer(120);
		}
	
		MySequencer(int tempo) {
			_running = false;
			setTempo(tempo);
			_currentStep = _lastStep = 1;
			_lastTempoTick = micros();
			_tickTimeInterval = 20833;
		}

		void run() {
			if(micros() - _lastTempoTick > _tickTimeInterval) {
				_sendClockSignal();
				_clockTickNumber %= CLOCKS_PER_BEAT;
				//Serial.println("Beat");
				if(_clockTickNumber == 0){
					_beat_cb();
					
				}
			}
		}

		void setTempo(int tempo) {
			_tempo = tempo;
			//_setTickTimeInterval(_calculateIntervalMicroSecs(tempo));
		}

		void getTempo() {
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
				_stepNumber = steps;
			}
		}

	private:
		int _tempo;
		int _tickTimeInterval;
		long _lastTempoTick;
		long _clockTickNumber;
		
		int _stepNumber;
		int _currentStep;
		int _lastStep;

		bool _running;

		MidiCallback _midi_cb;
		StepCallback _step_cb;
		BeatCallback _beat_cb;

		void _sendClockSignal() {
			
			// bail if the midi callback isn't set
			if (! _midi_cb) 
				return;			
			
			// tick
			_midi_cb(0x0, 0xF8, 0x0, 0x0);
			
			_lastTempoTick = micros();
			_clockTickNumber++;
		}

		long _calculateIntervalMicroSecs(int bpm) {
			// Take care about overflows!
			float interm = 60.0 / (float)bpm / CLOCKS_PER_BEAT;
			long val = interm * 1000000L;
			return val;
		}

		void _setTickTimeInterval(long ti) {
			_tickTimeInterval = ti;
		}


		void _sendNoteOn(Note note) {
			_midi_cb(0x0, 0x9, note.getPitch(), note.getVelocity());
		}

		void sendNoteOff(Note note) {
			_midi_cb(0x0, 0x8, note.getPitch(), 0x0);
		}

		void sendNoteOff(int pitch) {
			_midi_cb(0x0, 0x8, pitch, 0x0);
		}
};

#endif

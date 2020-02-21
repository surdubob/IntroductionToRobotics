#ifndef SET_H
#define SET_H

class Set {
	public:
		Set() {
			for (int i = 0; i < 4; i++) {
				values[i] = -1;
			}
		}

		void insert(int val) {
			for (int i = 0; i < 4; i++) {
				if (values[i] == -1) {
					values[i] = val;
					break;
				}
			}
		}

		void remove(int val) {
			for (int i = 0; i < 4; i++) {
				if (values[i] == val) {
					values[i] = -1;
				}
			}
		}

		bool exists(int val) {
			for (int i = 0; i < 4; i++) {
				if (values[i] == val) {
					return true;
				}
			}
			return false;
		}

		int size() {
			int nr = 0;
			for (int i = 0; i < 4; i++) {
				if (values[i] != -1) {
					nr++;
				}
			}
			return nr;
		}

		bool existsOtherThan(int a) {
			int nr = 0;
			for (int i = 0; i < 4; i++) {
				if (values[i] != -1 && values[i] != a) {
					nr++;
				}
			}
			return nr != 0;
		}
	private:
		int values[4];
};

#endif

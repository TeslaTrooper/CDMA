#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct sat {
	const int field1;
	const int field2;
};

const sat sats[24] = {
	{ 1, 5 },
	{ 2, 6 },
	{ 3, 7 },
	{ 4, 8 },
	{ 0, 8 },
	{ 1, 9 },
	{ 0, 7 },
	{ 1, 8 },
	{ 2, 9 },
	{ 1, 2 },
	{ 2, 3 },
	{ 4, 5 },
	{ 5, 6 },
	{ 6, 7 },
	{ 7, 8 },
	{ 8, 9 },
	{ 0, 3 },
	{ 1, 4 },
	{ 2, 5 },
	{ 3, 6 },
	{ 4, 7 },
	{ 5, 8 },
	{ 0, 2 },
	{ 3, 5 },
}; 

const int n = 10;
// 1023 - anzahl störender satelliten * minimum
// minimum siehe 598 gerade registerlänge
const int HIGH_PEEK = 1023 + (3 * (-(pow(2, (n+2)/2))-1));
const int LOW_PEEK = -1023 + (3 * (pow(2, (n + 2) / 2) - 1));

int const * const read(char const * const file) {
	int* const numbers = new int[1023];

	fstream fp;
	fp.open(file, ios::in);
	if (!fp.is_open()) {
		cout << "File not found!";
		exit(1);
	}

	int i = 0;
	int number;

	while (fp >> number) {
		numbers[i] = number;
		i++;
	}

	fp.close();
	
	return numbers;
}

/*
	Schiebt zwei Register um eine Stelle nach rechts.
*/
void shift(int* const reg1, int* const reg2, const int valReg1, const int valReg2) {
	for (int j = 8; j >= 0; j--) {
		reg1[j + 1] = reg1[j];
		reg2[j + 1] = reg2[j];
	}

	reg1[0] = valReg1;
	reg2[0] = valReg2;
}

int* const generateChipSeq(sat sat) {
	int* const seq = new int[1023];

	int* const reg1 = new int[10] { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	int* const reg2 = new int[10] { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	int xor1, xor2, xor3;

	for (int i = 0; i < 1023; i++) {
		xor1 = reg2[sat.field1] ^ reg2[sat.field2];
		seq[i] = xor1 ^ reg1[9] == 0 ? -1 : xor1 ^ reg1[9];

		xor2 = reg2[1] ^ reg2[2] ^ reg2[5] ^ reg2[7] ^ reg2[8] ^ reg2[9];
		xor3 = reg1[2] ^ reg1[9];

		shift(reg1, reg2, xor3, xor2);
	}

	return seq;
}

/*
	Schiebt ein Register um digits Stellen nach rechts.
*/
void shift(int* const reg, const int digits) {
	for (int i = 0; i < digits; i++) {
		int last = reg[1022];

		for (int a = 1021; a >= 0; a--) {
			reg[a + 1] = reg[a];
		}

		reg[0] = last;
	}
}

void cross(const int sat, int const * const sum, int * const chipSeq) {
	for (int delta = 0; delta < 1023; delta++) {
		int product = 0;

		for (int i = 0; i < 1023; i++) {
			product += sum[i] * chipSeq[i];
		}

		if (product >= HIGH_PEEK) {
			cout << "Satellite " << sat << " has sent Bit 1 " << "(delta = " << delta << ")" << endl;
			return;
		}
		if (product <= LOW_PEEK) {
			cout << "Satellite " << sat << " has sent Bit 0 " << "(delta = " << delta << ")" << endl;
			return;
		}

		shift(chipSeq, 1);
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Missing argument!";
		return 1;
	}

	int const * const data = read(argv[1]);

	for (int sat = 0; sat < 24; sat++) {
		int * const seq = generateChipSeq(sats[sat]);

		cross(sat + 1, data, seq);
	}

	cout << "Press to exit..."; cin.get();
	
	return 0;
}
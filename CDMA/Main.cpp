#include <iostream>
#include <fstream>

using namespace std;

struct sat {
	const int field1;
	const int field2;
};

struct peek {
	int value;
	int index;
};

sat sats[24] = {
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

int* const read(char const * const file) {
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

/*
	Kreuzkorreliert das Summensignal mit der Chipsequenz und gibt die peeks an den 
	entsprechenden Stellen zurück.
*/
peek const * const cross(int const * const sum, int * const chipSeq) {
	peek* peeks = new peek[2];
	peeks[0] = { 0,0 };
	peeks[1] = { 0,0 };
	int currentSum;

	for (int k = 0; k < 1023; k++) {
		currentSum = 0;

		for (int i = 0; i < 1023; i++) {
			currentSum += sum[i] * chipSeq[i];
		}

		if (currentSum > peeks[0].value) {
			peeks[0].value = currentSum;
			peeks[0].index = k;
		}
		if (currentSum < peeks[1].value) {
			peeks[1].value = currentSum;
			peeks[1].index = k;
		}

		shift(chipSeq, 1);
	}

	return peeks;
}

/*
	Bildet das Skalarprodukt des Summensignals und der Chipsequenz. Hierbei wird
	vor der Berechnung die Chipsequenz um delta verschoben.
*/
int getSignal(int const * const sum, int * const chipSeq, const int delta) {
	int result = 0;

	shift(chipSeq, delta);

	for (int i = 0; i < 1023; i++) {
		result += sum[i] * chipSeq[i];
	}

	result = (int) (result / 1023);

	return result == -1 ? 0 : result;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Missing argument!";
		return 1;
	}
	int const * const data = read(argv[1]);

	for (int i = 0; i < 24; i++) {
		int * const seq = generateChipSeq(sats[i]);

		peek const * const peeks = cross(data, seq);

		if (peeks[0].value < 1023 - (3*63) && peeks[1].value > -1023 - (3*-65)) {
			continue;
		}

		float signal;
		if (peeks[0].value > 1023) {
			signal = getSignal(data, seq, peeks[0].index);
			cout << "Satellite " << i + 1 << " has sent Bit " << signal << " (delta = " << peeks[0].index << ")" << endl;
		} else {
			signal = getSignal(data, seq, peeks[1].index);
			cout << "Satellite " << i + 1 << " has sent Bit " << signal << " (delta = " << peeks[1].index << ")" << endl;
		}
		
	}

	while (1);
	
	return 0;
}
#include <algorithm>
#include <ctime>
#include <iostream>

int main() {
    const unsigned arraySize = 32768;
    int data[arraySize];
    for (unsigned c = 0; c < arraySize; ++c) {
        data[c] = std::rand() % 256;
	}
    std::sort(data, data + arraySize);

    long sum = 0;
    for (unsigned i = 0; i < 1000000000; i += 10) {
		sum += i;
		sum += i + 1;
		sum += i + 2;
		sum += i + 3;
		sum += i + 4;
		sum += i + 5;
		sum += i + 6;
		sum += i + 7;
		sum += i + 8;
		sum += i + 9;
		/*
        for (unsigned c = 0; c < arraySize; ++c) {
            if (data[c] >= 128)
                sum += data[c];
        }
		*/
    }
    std::cout << "sum = " << sum << std::endl;
}

#include <iostream>
#include <fstream>
#include <stdint.h>
#include "Main.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(){

	ifstream data("example_1_1.aghconf", ios_base::binary);
	Config konfiguracja(data);
	konfiguracja.print();
	
	/*uint16_t number = 2;
	ofstream data_to_save("saved_data.bin", ios_base::binary);
	data_to_save.write(reinterpret_cast<char*>(&number),sizeof(number));
	data_to_save.close();

	uint16_t numbers_16;
	ifstream data("saved_data.bin", ios_base::binary);
	data.seekg(0);
	data.read(reinterpret_cast<char*>(&numbers_16), sizeof(numbers_16));
	cout << numbers_16 << "\n";

	
  	data.close();*/
	return 0;
}





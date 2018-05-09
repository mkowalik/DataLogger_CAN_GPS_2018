#include <iostream>
#include <fstream>
#include <stdint.h>
#include "ReadingClass.h"
#include "ConfigChannel.h"
#include "Config.h"
#include "ConfigFrame.h"
#include "ValueType.h"

using namespace std;

//Operations on binary files:
// - reading binary files
// - printing content of binary files
// - writing to file in binary

int main(){
	string name_of_file;
	cout << "Write name of file: ";
	cin >> name_of_file;
	Config configuration(name_of_file);
	configuration.print();
	
	string answear;
	cout << "Do you want to save to file? (y/n) ";
	cin >> answear;
	if (answear == "y"){
		cout << "Write name of file you want to save: ";
		cin >> name_of_file;
		ofstream data_to_save(name_of_file.c_str(), ios_base::binary);
		configuration.writeToBinary(data_to_save);
		data_to_save.close();
	}

	return 0;
}

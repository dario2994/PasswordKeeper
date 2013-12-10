//TODO: Incoerenza nell'import export non criptato. Attualmente se faccio un export e lo importo potrebbe non funzionare
//		se decido di esportare senza spazi. (ma forse questo è anche normale)
//TODO: Implementare backup automatici?
//TODO: Implementare nei backup criptati la presenza di un hash della password stessa.

#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>

#include "header.hpp"
using namespace std; 

string privateKey;
vector <entry*> Entries;

void savePrivateKeyHash() {
	ofstream passOut(privateKeyHashPath.c_str());
	assert( passOut!=NULL );
	passOut << hashPassword( privateKey );
	passOut.close();
}

void save() {
	ofstream out(passwordListPath.c_str());
	out << Entries.size() << "\n";
	for(int i=0;i<(int)Entries.size();i++) {
		entry* ee=new entry();
		*ee=*(Entries[i]);
		ee->enc( privateKey );
		out << ee->place << " " << ee->user << " " << ee->pass << "\n";
	}
	out.close();
}

void setPrivateKey( string s ) {
	privateKey=s;
	savePrivateKeyHash();
	save();
}

void init () {
	
	destroy();
	
	cout << "This is the first time you execute password-keeper.\n\n";
	
	system(("mkdir "+mainFolderPath).c_str());
	
	changePrivateKey();
	
	ofstream listOut(passwordListPath.c_str());
	listOut << "0\n";
	listOut.close();
	
	cout <<  "Password-keeper initialized\n" ;
}

void readAllPass( ) {
	ifstream in(passwordListPath.c_str());
	int entryNumber;
	in >> entryNumber;
	for(int i=0;i<entryNumber;i++) {
		entry* newOne=new entry();
		in >> newOne->place >> newOne->user >> newOne->pass;
		newOne->dec( privateKey );
		Entries.push_back(newOne);
	}
	in.close();
}

bool login () {
	privateKey=hiddenQuestionForm("Password:");
	
	//DEBUG
	if( privateKey == "debugdestroy" ) {
		destroy();
		cout << "Debug destroy.\n";
	}
	
	ifstream in(privateKeyHashPath.c_str());
	string correctHash;
	in >> correctHash;
	in.close();
	string hashedKey = hashPassword( privateKey );
	if ( hashedKey != correctHash ) {
		cout << "Password is incorrect, try again.\n";
		return 0;
	}
	
	cout << "Login successful.\n";
	return 1;
}

int main(){
	
	initPath();
	
	if( access(mainFolderPath.c_str() , F_OK) != 0 ) init();
	else {
		while( !login() );
	}
	
	readAllPass();
	
	while(1) {
		string command = questionForm(">");
		
		if( command == "add" ) add();
		
		else if( command == "remove" ) remove();
		
		else if( command == "get" ) retrieve();
		
		else if( command == "getAll" ) retrieveAll();
		
		else if( command == "quit" ) {
			save();
			return 0;
		}
		
		else if( command == "import" ) importPasswords(); 
		
		else if( command == "export" ) exportPasswords();
		
		else if( command == "changeKey" ) changePrivateKey();
		
		else if( command == "destroy" ) {
			bool desBool=decisionForm("Are you sure you want to delete all your saved password and configuration?");
			if( desBool ) {
				destroy();
				cout << "Everything has been deleted.";
				return 0;
			}
		}
		
		else if( command == "help" ) {
			cout << "This are all the possible commands:\n";
			cout << "\tadd\t: add an entry to the password list\n";
			cout << "\tremove\t: remove an entry from the password list\n";
			cout << "\tget\t: get an entry from the password list\n";
			cout << "\tgetAll\t: get all entries from the password list\n";
			cout << "\tquit\t: quit the program\n";
			cout << "\timport\t: import the list of passwords\n";
			cout << "\texport\t: export the list of passwords\n";
			cout << "\tchangeKey\t: change the private key used for the encryption\n";
			cout << "\tdestroy\t: destroy the password list and any configuration\n";
			cout << "\thelp\t: gives help about the usage of password-keeper\n";
		}
		
		else {
			cout << "Command doesn't exist, for help about the usage type help.\n";
		}
	}
}

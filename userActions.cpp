#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <vector>

#include "header.hpp"
using namespace std;

//Search in Entries for for place. Return the indexes.
vector <int> searchEntry( string place ) {
	vector <int> found;
	for( int i=0;i<(int)Entries.size();i++) if( Entries[i]->place == place ) found.push_back(i);
	return found;
}

//Search in Entries for the exact same pair (place,user) of val. Return true if present, false otherwise.
bool searchCollisions( entry* val ) {
	for(int i=0;i<(int)Entries.size();i++) {
		if( Entries[i]->place==val->place and Entries[i]->user==val->user ){
			return true;
		}
	}
	return false;
}

//Add an entry.
void add( ) {
	entry* val=new entry();
	val->place = questionForm("Place:");
	val->user = questionForm("User:");
	while(1) {
		val->pass = hiddenQuestionForm("Password:");
		string pswCheck = hiddenQuestionForm("Insert again password:");
		if( val->pass == pswCheck ) break;
		else cout << "The second password doesn't correspond with the first. Try again.\n";
	}
	
	vector <int> found=searchEntry(val->place);
	int cc=found.size();
	if ( cc == 0 ) { //If there isn't another entry with the same place
		Entries.push_back(val);
		cout << "Entry has been registered.\n";
	}
	else{ //Else gives the opportunity to update an existing entry or to add a new
		cout << val->place << " has already some entries:\n";
		for(int i=0;i<cc;i++){ //list of entries with the same place
			entry* x=Entries[found[i]];
			cout << "\t" << i+1 << "- User: " << x->user << " ; Pass: " << strToAst(x->pass) << "\n";
		}
		string answer=questionForm("Do you want to add a new entry (write new) or \
to update one of the entry (write its number)?");
		if( answer == "new" ) { //Add a new if it doesn't collide
			if( !searchCollisions(val) ) {
				Entries.push_back(val);
				cout << "Entry has been registered.\n";
			}
			else {
				cout << "The pair (place, user) = (" << val->place << ", " << val->user << ") is already present.\n";
			}
		}
		else { //Update if doesn't collide
			int nn=atoi(answer.c_str());
			if ( 1<=nn and nn<=cc ) {
				entry* x=Entries[found[nn-1]];
				if( x->user == val->user or !searchCollisions(val) ) {
					Entries[found[nn-1]]=val;
					cout << "Entry has been updated.\n";
				}
				else {
					cout << "The pair (place, user) = (" << val->place << ", " << val->user << ") is already present.\n";
				}
			}
			else{ //If the value given by the user is wrong
				cout << "You entered an invalid value.\n";
			}
		}
	}
	
}

//Remove an entry.
void remove() {
	string place = questionForm("Place:");
	
	vector <int> found=searchEntry(place);
	int cc=found.size();
	if( cc==0 ) cout << "The place " << place << " isn't in the password list.\n";
	else if( cc==1 ) {
		Entries.erase(Entries.begin()+found[0]);
		cout << "Entry has been deleted.\n";
	}
	else {
		cout << "The place " << place << " is present in more than one entry:\n";
		for(int i=0;i<cc;i++){
			entry* x=Entries[found[i]];
			cout << "\t" << i+1 << "- User: " << x->user << " ; Pass: " << strToAst(x->pass) << "\n";
		}
		
		string answer=questionForm("Which (* for all) should I delete?");
		
		if( answer == "*" ) {
			for(int i=cc-1; i>=0 ;i--) { //cancello dall'ultimo per non rompere i puntatori
				assert( Entries[found[i]]->place == place );
				Entries.erase(Entries.begin()+found[i]);
			}
			cout << "All entries relative to " << place << " have been deleted.\n";
		}
		else {
			int nn=atoi(answer.c_str());
			if ( 1<=nn and nn<=cc ) {
				Entries.erase(Entries.begin()+found[nn-1]);
				cout << "Entry has been deleted.\n";
			}
			else{
				cout << "You entered an invalid value.\n";
			}
		}
	}
}

//Retrieve an entry.
void retrieve( ) {
	string place = questionForm("Place:");
	
	vector <int> found=searchEntry(place);
	int cc=found.size();
	if( cc==0 ) cout << "The place " << place << " isn't in the password list.\n";
	else {
		for(int i=0;i<cc;i++) {
			entry* x=Entries[found[i]];
			cout << "\t" << i+1 << "- User: " << x->user << " ; Pass : " << x->pass << "\n";
		}
	}
}

//Retrieve (and show) all entries.
void retrieveAll( ) {
	for(int i=0;i<(int)Entries.size();i++) {
		entry* x=Entries[i];
		cout << "Place: " << x->place << " ; User: " << x->user << " ; Pass : " << x->pass << "\n";
	}
}

void importPasswords() {
	bool encBool=decisionForm("Are the passwords in the file encrypted?");
	string encKey;
	if( encBool ) {
		while (1) {
			encKey=hiddenQuestionForm("Insert the key used for decryption:");
			string pswCheck=hiddenQuestionForm("Insert again the key:");
			if( encKey == pswCheck) break;
			else cout << "The second key doesn't correspond with the first. Try again.\n";
		}
	}
	
	bool sepBool=false;
	if( !encBool ) sepBool=decisionForm("Is the separator between place,user,password used (otherwise space will be used)?");
	string separator;
	if(sepBool) separator=SEPARATOR;
	else separator=" ";

	string sourcePath;
	while( 1 ) {
		sourcePath=questionForm("Write the absolute file path from which to import: ");
		if( access( sourcePath.c_str() , R_OK ) == 0 ) break;
		cout << "The path entered doesn't correspond to a readable file.\n";
	}
	
	ifstream source( sourcePath.c_str() );
	
	int nn, collNum=0, errLine=0;
	
	source >> nn;
	string stupidNewLine;//To avoid errors with getline
	getline(source,stupidNewLine);
	
	for( int i=0;i<nn;i++){
		entry* newOne=new entry();
		string line;
		getline(source,line);
		vector <string> pieces=splitString(line,separator);
		if( pieces.size() != 3 ) errLine++;
		else {
			newOne->place=pieces[0];
			newOne->user=pieces[1];
			newOne->pass=pieces[2];
			if( encBool ) newOne->dec(encKey);
			if( !searchCollisions(newOne) ) Entries.push_back(newOne);
			else collNum++;
		}
	}
	
	save();
	cout << "\nAll passwords have been imported (" << collNum << " collisions and " << errLine << " errors)\n";
}

void exportPasswords() {
	bool encBool=decisionForm("Do you want the passwords to be encrypted (otherwise just plain-text)?");
	string encKey;
	if( encBool ) {
		while (1) {
			encKey=hiddenQuestionForm("Insert the key to use for encryption:");
			string pswCheck=hiddenQuestionForm("Insert again the key:");
			if( encKey == pswCheck) break;
			else cout << "The second key doesn't correspond with the first. Try again.\n";
		}
	}
	
	bool sepBool=false;
	if( !encBool ) sepBool=decisionForm("Do you want a separator between place,user,password (suggested if some spaces are used)?");
	string separator;
	if(sepBool) separator=SEPARATOR;
	else separator=" ";
	
	bool fileBool=decisionForm("Do you want to export in a file?");
	
	if(fileBool) {
		ofstream dest( questionForm("Write the absolute path:").c_str() );
		dest << Entries.size() << "\n";
		for( int i=0;i<(int)Entries.size();i++){
			entry* x=Entries[i];
			x->enc(encKey);
			dest << x->place << separator << x->user << separator << x->pass << "\n";
			x->dec(encKey);
		}
		dest.close();
		cout << "\nAll passwords have been saved in the specified file.\n";
	}
	
	else {
		cout << "\n";
		cout << Entries.size() << "\n";
		for( int i=0;i<(int)Entries.size();i++){
			entry* x=Entries[i];
			cout << x->place << separator << x->user << separator << x->pass << "\n";
		}
		cout << "\nAll passwords have been printed.\n";
	}
}

void changePrivateKey() {
	string privateKey;
	while (1) {
		privateKey = hiddenQuestionForm("Insert the private key (used for encryption):");
		string pswCheck   = hiddenQuestionForm("Insert the private key again:");
		if( privateKey == pswCheck ) break;
		else cout << "The second key doesn't correspond with the first. Try again.\n";
	}
	setPrivateKey( privateKey );
}

void destroy () { //Deletes the folder if present
	system( ("rm -fr "+mainFolderPath).c_str() );
}

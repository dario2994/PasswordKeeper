#include <string>
#include <vector>
#include <iostream>
#include "header.hpp"
using namespace std;

typedef unsigned int uint;
typedef unsigned short int suint;

inline uint rightShift ( uint x, uint y ) {
	return (x >> y);
}
inline uint rightRotate( uint x, uint y ) {
	return ( (x >> y) | (x << (32-y)) );
}

uint k[64]={0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
char base16[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

suint inverse16(char x){
	for(int i=0;i<16;i++)if(base16[i]==x) return i;
	cout << "Err: the char " << x << " is not in [0..9] or [a..f].\n";
	return 16;
}

string sha256( string S ) {
	uint h[8]= {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	uint tt[8];
	uint w[64];
	uint s0,s1;
	uint S0,S1,ch,maj,temp1,temp2;
	
	vector <uint> bits;
	int l=S.size();
	bits.resize( (l/4)+2 );
	for(int i=0;i<l;i++) {
		bits[i/4]|= ((uint)S[i])<<(8*(i%4));
	}
	while(bits.size()%16 != 15 )bits.push_back(0); //non standard... manca il bit 1
	bits.push_back(S.size());
	
	for(int chunk=0;chunk<(int)(bits.size()/16);chunk++) {
		for(int i=0;i<16;i++)w[i]=bits[chunk*16+i];
		for(int i=16;i<64;i++) {
			s0 = ( rightRotate(w[i-15],7) )^( rightRotate(w[i-15],18) )^( rightShift(w[i-15],3) );
			s1 = ( rightRotate(w[i-2],17) )^( rightRotate(w[i-2], 19) )^( rightShift(w[i-2],10) );
			w[i] = w[i-16] + s0 + w[i-7] + s1;
		}
		
		for(int i=0;i<8;i++) tt[i]=h[i];
		
		for(int i=0;i<64;i++){
			S1 = rightRotate(tt[4],6)^rightRotate(tt[4],11)^rightRotate(tt[4],25);
			ch = (tt[4]&tt[5])^((!tt[4])&tt[6]);
			temp1 = tt[7] + S1 + ch + k[i] + w[i];
			S0 = rightRotate(tt[0],2)^rightRotate(tt[0],13)^rightRotate(tt[0],22);
			maj = (tt[0]&tt[1])^(tt[0]&tt[2])^(tt[1]&tt[2]);
			temp2 = S0 + maj;
			
			for(int j=7;j>=1;j--) {
				tt[j]=tt[j-1];
				if (j == 4) tt[j]+=temp1;
			}
			tt[0]=temp1+temp2;
		}
		
		for(int i=0;i<8;i++)h[i]+=tt[i];
	}
	
	string res;
	for(int i=0;i<8;i++) { //non standard output
		for(int j=0;j<8;j++)res+=base16[h[i]%16], h[i]>>=4;
	}
	return res;
}

string removeSpaces ( string text ) {
	string res;
	for(int i=0;i<(int)text.size();i++) {
		if( text[i]!=' ' ) res+=text[i];
		else {
			res+=SPACESEP;
		}
	}
	return res;
}

string addSpaces ( string text ) {
	vector <string> pieces=splitString( text , SPACESEP );
	string res=pieces[0];
	for(int i=1;i<(int)pieces.size();i++) res+=" "+pieces[i];
	return res; 
}

string hashPassword ( string pass ) {
	string res=sha256(sha256(pass)+"Giada");
	res=res.substr(0,16);
	return res;
}

//Encripta usando un classico xor ma mischiato con sha della chiave e somma delle entrate del testo.
string encrypt ( string text , string key ) {
	vector <suint> T;
	T.resize(2*text.size());
	for(int i=0;i<(int)text.size();i++) {
		T[2*i]=(suint)text[i]%16;
		T[2*i+1]=((suint)text[i])>>4;
	}
	for(int i=1;i<(int)T.size();i++)T[i]=(T[i]+T[i-1])%16;
	
	string hashedKey=sha256(key+"Cucciola ti voglio bene");
	vector <suint> H;
	H.resize(hashedKey.size());
	for(int i=0;i<(int)hashedKey.size();i++) H[i]=inverse16(hashedKey[i]);
	
	string res;
	res.resize(T.size());
	for(int i=0;i<(int)T.size();i++)res[i]=base16[ T[i]^H[i%(H.size())] ];

	return res;
}

//Inverso di encrypt
string decrypt ( string text , string key ) {
	string hashedKey=sha256(key+"Cucciola ti voglio bene");
	vector <suint> H;
	H.resize(hashedKey.size());
	for(int i=0;i<(int)hashedKey.size();i++) H[i]=inverse16(hashedKey[i]);

	vector <suint> T;
	T.resize(text.size());
	for(int i=0;i<(int)text.size();i++) T[i]=inverse16(text[i])^H[i%(H.size())];
	for(int i=(int)T.size()-1;i>0;i--) T[i]=(16+T[i]-T[i-1])%16;

	string res;
	res.resize(T.size()/2);
	for(int i=0;i<(int)T.size()/2;i++){
		res[i]=(char)(T[2*i]+ (T[2*i+1]<<4) );
	}
	return res;
}

void entry::enc( string key ){
	pass=encrypt( pass, key+place+user );
	place=removeSpaces( place );
	user=removeSpaces( user );
}

void entry::dec( string key ){
	place=addSpaces( place );
	user=addSpaces( user );
	pass=decrypt( pass , key+place+user );
}

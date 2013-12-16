#include <iostream>
#include <string>
using namespace std;

int** D;
string S, T;
int rmcost;
int letterDist[128][128];
int letterCost[128];

int editDistance(int a, int b){
	if( D[a][b]!=-1 ) return D[a][b];
	if( a==0 and b==0 ) D[a][b]=0;
	else if( a==0 ) D[a][b]=editDistance(a,b-1)+letterCost[(int)T[b-1]];
	else if( b==0 ) D[a][b]=editDistance(a-1,b)+letterCost[(int)S[a-1]];
	else {
		int x=S[a-1], y=T[b-1];
		D[a][b]=editDistance(a-1,b-1)+letterDist[x][y];
		D[a][b]=min( D[a][b], editDistance(a,b-1)+letterCost[y] );
		D[a][b]=min( D[a][b], editDistance(a-1,b)+letterCost[x] );
	}
	return D[a][b];
}

double stringSimilarity(string SS, string TT) {
	S=SS,T=TT;
	int s=S.size(), t=T.size();
	D=new int*[s+1];
	for(int i=0;i<=s;i++) D[i]=new int[t+1];
	for(int i=0;i<=s;i++)for(int j=0;j<=t;j++)D[i][j]=-1;
	double ed=editDistance(s,t);

	for(int i=0;i<=s;i++) delete [] D[i];
	delete [] D;
	return ed/(double)(S.size()+T.size());
}

void initDist(int* A, int qq, int dx) {
	for(int i=0;i<qq;i++)for(int j=0;j<qq;j++) letterDist[A[i]][A[j]]=letterDist[A[j]][A[i]]=dx;
}

void initCost(int* A, int qq, int dx) {
	for(int i=0;i<qq;i++)letterCost[A[i]]=dx;
}

void initStrSim() {

	int all[128];
	for(int i=0;i<128;i++)all[i]=i;
	int space[4]={' ','-','_','\t'};
	int punct[4]={'.',';',',',':'};
	int num[10]={'0','1','2','3','4','5','6','7','8','9'};
	int alpha[52];
	for(int i=0;i<26;i++)alpha[i]='a'+(char)i;
	for(int i=0;i<26;i++)alpha[26+i]='A'+(char)i;

	initDist(	all,	128,	10	);
	initDist(	space,	4,		2	);
	initDist(	punct,	4,		4	);
	initDist(	num,	10,		4	);
	initDist(	alpha,	52,		6	);
	for(int c='a';c<'z';c++) letterDist[c]['A'+(c-'a')]=letterDist['A'+(c-'a')][c]=1; //Costo dei casi
	for(int i=0;i<128;i++) letterDist[i][i]=0;
	
	initCost(	all,	128,	6	);
	initCost(	space,	4,		2	);
	initCost(	punct,	4,		4	);
}

void ddd(string A, string B) {
	cout << A << "\t" << B << "\t\t: " << stringSimilarity(A,B) << "\n";
}

int main() {
	initStrSim();
	ddd( "Ciao bella!" , "Ciaobbello" );
	ddd( "facebook" , "Facebook.com" );
	ddd( "facebook" , "gmail");
	ddd( "facebook.com", "gmail.com");
	ddd( "google" , "youtube");
	ddd( "arturo" , "antani" );
	ddd( "google calendar", "calendar" );
	ddd( "Errorino", "Errorone" );
	//~ cout << letterDist[S[3]]['o'] << "\n";
	//~ cout << D[4][4] << "\n";
}
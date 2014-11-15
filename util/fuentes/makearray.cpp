#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <iomanip>
using namespace std;

#define WIDTH 16
#define HEIGHT 16

int bits[27136];

int main(int argc, char *argv[]){
	if(argc!=2){
		cout << "Usage: makearray rawimage" << endl;
		return 1;
	}
	FILE *f=fopen(argv[1], "rb");
	char c;
	int q=0, lines=0;
	for(int i=0; c=fgetc(f), !feof(f); i++){
		if((i%4)==3){
			if(c) bits[lines]|= 1<<(WIDTH-1-q%WIDTH);
			q++;
			if((q%WIDTH)==0) lines++;
		}
	}
	fclose(f);
	lines-=lines%HEIGHT;
	
	//Readable version
	//~ for(int i=0; i<lines; i++) if(isprint(i/HEIGHT)){
		//~ if(i%HEIGHT==0) cout << "=== " << (char)(i/HEIGHT) <<  " (" << i/HEIGHT << ")" << endl;
		//~ for(int j=0; j<WIDTH; j++)
			//~ cout << (bits[i]&(1<<(WIDTH-1-j))? '*':' ');
		//~ cout << endl;
	//~ }
	//~ cout << lines << " lines, "<< q << " characters, " << lines/HEIGHT << " letters." << endl;
	
	//C array version
	cout << hex << "uint16_t nesfont_raw[] = {\n";
	for(int i=0; i<lines; i++){
		if(i && i%HEIGHT==0) cout << endl;
		cout << "0x" << bits[i] << (i==lines-1?"};\n":",");
	}
	return 0;
}


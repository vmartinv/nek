#include <iostream>
using namespace std;



int main(){
	for(int c=32; c<256; c++){
		if(isprint(c)) cout << (char)c << endl;
		else cout << ' ' << endl;
	}
	return 0;
}


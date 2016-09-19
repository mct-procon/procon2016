//“ü—ÍŒ`®‚Ì•ÏŠ· (ŒÃ¨V)

#include <iostream>
#include <vector>
using namespace std;

void io_poly(int n) {
	vector<int> x;
	vector<int> y;
	x.resize(n);
	y.resize(n);
	
	for (int i = 0; i < n; i++) {
		cin >> x[i];
	}
	for (int i = 0; i < n; i++) {
		cin >> y[i];
	}
	
	for (int i = 0; i < n; i++) {
		cout << x[i] << " " << y[i] << endl;
	}
}

void io() {
	int numOfPoly;
	
	cin >> numOfPoly;
	cout << numOfPoly << endl;
	
	while (numOfPoly--) {
		int n;
		cin >> n;
		cout << n << endl;
		io_poly(n);
	}
}

int main() {
	io();
	io();
	return 0;
}
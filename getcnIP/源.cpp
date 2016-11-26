#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;
typedef long long LL;
//http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest
LL StringToNum(string s){
	LL num;
	stringstream ss(s);
	ss >> num;
	return num;
}
string NumToString(LL i){
	stringstream ss;
	ss << i;
	return ss.str();
}
string getSubnetMask(LL num){
	string mask[4];
	LL n[4] = { 255,255,255,0 };
	//int t=num >> 8;
	if (num > (1 << 24)) {
		n[0] -= (num >> 24) - 1;
		n[1] = n[2]= 0;
	}	
	else if (num > (1 << 16)) {
		n[1] -= (num >> 16) - 1;
		n[2] = 0;
	}
	else if(num > (1 << 8))
		n[2] -= (num >> 8) - 1;
	for (int i = 0; i < 4; ++i)
		mask[i] = NumToString(n[i]);
	return mask[0] + "." + mask[1] + "." + mask[2] + "." + mask[3];
}
string getIP(string line,string a="Mask") {
	string temp(line,14,line.length()-14);
	size_t pos1=temp.find("|");
	string str1(temp, 0,pos1);
	size_t pos2 = temp.find("|",pos1+1);
	string str2(temp, pos1+1, pos2-pos1-1);
	str2=getSubnetMask(StringToNum(str2));
	if (a == "IP")
		return str1;
	return str2;
}
bool isCNipv4(string line) {
	string temp(line, 0, 14);
	if (temp == "apnic|CN|ipv4|")
		return 1;
	return 0;
}
void main() {
	ifstream rawdata("D:\\ip.txt");
	ofstream outfile,add,del;
	outfile.open("D:\\cnIP.txt", ios::trunc);
	add.open("D:\\add.txt", ios::trunc);
	del.open("D:\\del.txt", ios::trunc);
	string tempStr,IP,mask;
	size_t t = 0;
	if (rawdata) {
		cout << "��������·�ɱ�..." << endl;
		while (getline(rawdata, tempStr, '\n')) {
			if (isCNipv4(tempStr)) {
				if (t) {
					outfile << endl;
					add << endl;
					del << endl;
				}
				IP = getIP(tempStr, "IP");
				mask = getIP(tempStr);
				outfile << IP+" "+ mask;
				add << "add "+IP+" mask "+mask+" default METRIC default IF default";
				del<< "delete " + IP + " mask " + mask + " default METRIC default IF default";
				++t;
			}	
		}
		outfile.close();
		cout << "·�ɱ����ɳɹ�������"<<t<<"��"<< endl;	
	}
	else {
		cout << "ERROR!" << endl;
	}

	system("pause");
}
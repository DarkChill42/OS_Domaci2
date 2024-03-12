/*
    Uz zadatke ide fajl koji se zove ulaz.txt
    U njemu se nalaze bodovi studenata u formi:
    Ime,Prezime,V=70&P=30,Beleska
    Napraviti program koji na osnovu fajla sračuna prosek za V (vežbe) i prosek za 
    P(ispit). 
*/

#include <string>
#include <iostream>
#include <regex>
#include <fstream>

using namespace std;

int main(){

    ifstream ulaz("ulaz.txt");

    string str = "";
    auto const regex1 = regex("\\d+");

    
    smatch s;
    float prosekVezbe = 0.0;
    float prosekIspit = 0.0;
    vector<int> bodovi;
    while(getline(ulaz,str)) {
        while(regex_search(str,s,regex1)) {
            bodovi.push_back(stoi(s[0]));
            str = s.suffix();
        }
    }
    for(int i = 0; i < bodovi.size(); i++) {
        if(i%2==0) prosekVezbe+=bodovi[i];
        else prosekIspit+=bodovi[i];
    }
    cout << "Prosek na vezbama: " << (prosekVezbe / (bodovi.size() / 2)) << ". Prosek na ispitu: " << (prosekIspit / (bodovi.size() / 2)) << endl;
    ulaz.close();

}
/**
 * Napisati C++ program koji iz ulazne datoteke cita podatke o temperaturi u toku vikenda sa ski 
 * staza na Kopaoniku, Zlatiboru i Jahorini i odredjuje koji dani i na kojoj planini su idealni 
 * za skijanje a koji nisu. Neki idealni opseg temperature za skijanje je od -5 do 3 stepena.
 * 
 * Za svaku od planina postoji posebna datoteka cije ime se sastoji od imena planine i prosirenja
 * (ekstenzije) ".txt". U svakoj pojedinacnoj datoteci se u jednom redu nalaze podaci za jedan dan. 
 * Jedan red sadrzi redom ime_dana, datum, i potom izmerene temperature. Temperatura se meri na 
 * svakih sat vremena, pocevsi od 8 ujutru, do 8 uvece. Svi podaci su odvojeni razmakom.
 * 
 * Izgled jednog reda iz ulaznih datoteka "Kopaonik.txt" "Zlatibor.txt" "Jahorina.txt"
 * 
 *     subota 01.02.  -15 -13 -10 -8 -3 0 -2 -3 2 2 -5 -7 -3
 * 
 * NAPOMENA: ukoliko ne postoji neka od ulaznih datoteka, treba samo napisati poruku da datoteka
 * ne postoji i nastaviti dalje sa izvrsavanjem programa uz oslonac na podatke iz preostalih
 * datoteka
 * 
 * Treba za svaki dan pronaci najnizu i najvisu dnevnu temperaturu. Ukoliko minimalna i maksimalna
 * temperatura upadaju u navedeni opseg, treba informacije za taj dan ispisati u datoteku 
 * "idealno.txt", u suprotnom u datoteku "lose.txt".
 *
 * Ispis u izlaznu datoteku treba da prati format:
 *     <ime_planine> [<ime_dana> <datum>] <min. temp.> >> <maks. temp.>
 * 
 * Primer ispisa u bilo kojoj od izlaznih datoteka "idealno.txt", "lose.txt":
 * 
 *     Kopaonik [subota 01.02.] -15 >> 2
 *
 * Treba napraviti jednu nit koja ce samo citati podatke iz ulaznih datoteka, jednu nit koja ce 
 * samo pisati spremljene podatke u izlazne datoteke i 4 niti radnika koji ce na osnovu podataka iz
 * ulaznih datoteka generisati sve neophodno za ispis u izlazne datoteke.
*/
#include<iostream>
#include<functional>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<string>
#include<regex>
#include<vector>
#include<queue>
#include<ostream>
#include<fstream>

#define BR_RADNIKA 4
#define BR_MERENJA_TEMPERATURE 13
#define MINIMALNA_IDEALNA_TEMPERATURA -5
#define MAKSIMALNA_IDEALNA_TEMPERATURA 3

using namespace std;

mutex mx;

class StanjeSkijalistaNaDan {
private:
    // TODO dodati polja po potrebi
    string dan,datum,ime;
    vector<int> temperature;
    bool idealan;

    int minIdeal,maxIdeal;
public:
    // TODO dodati metode po potrebi
    StanjeSkijalistaNaDan(string i,string d, string dat, vector<int> t) : ime(i), dan(d), datum(dat), temperature(t){}
    StanjeSkijalistaNaDan() {}

    string const getDan() {return dan;}
    string const getDatum() {return datum;}
    string const getIme() {return ime;}

    bool const jeIdealan() {return idealan;}
    void setJeIdealan() {idealan = proveraTemperatura();}

    int const getMinIdeal() {return minIdeal;}
    int const getMaxIdeal() {return maxIdeal;}

    vector<int> const getTemperature() {return temperature;}
private:
    // TODO dodati metode po potrebi
    bool proveraTemperatura() {
        minIdeal=1e9,maxIdeal = -1e9;
        for(int i = 0; i < temperature.size(); i++) {
            if(temperature[i] > maxIdeal) maxIdeal = temperature[i];
            if(temperature[i] < minIdeal) minIdeal = temperature[i];
        }
        if(minIdeal < MINIMALNA_IDEALNA_TEMPERATURA || minIdeal > MAKSIMALNA_IDEALNA_TEMPERATURA ||
           maxIdeal < MINIMALNA_IDEALNA_TEMPERATURA || maxIdeal > MAKSIMALNA_IDEALNA_TEMPERATURA) {
            return false;
        }
        return true;
    }
};

/** Klasa koja modeluje "postansko sanduce" izmedju citaca i radnika.
*/
template<typename T>
class RedoviIzDatoteke {
private:
    mutex podaci_mx;                       // propusnica za sinhronizaciju nad svim poljima klase
    // TODO dodati polja po potrebi
    bool kraj;
    condition_variable con_v;
    queue<T> data;
public:
    RedoviIzDatoteke(): kraj(false) {}

    void dodaj(T redIzDatoteke) {
        // TODO
        unique_lock<mutex> lock(podaci_mx);
        data.push(redIzDatoteke);
        con_v.notify_one();
    }

    bool preuzmi(T &redIzDatoteke) {
        // TODO
        unique_lock<mutex> lock(podaci_mx);
        while(daLiCekamPodatke()) {
            con_v.wait(lock);
        }
        if(jeLiKraj()) return false;

        redIzDatoteke = data.front();
        data.pop();
        return true;
    }

    // TODO dodati metode po potrebi

    void krajCitanja() {
        kraj = true;
        con_v.notify_all();
    }

private:
    /**
     * Provera da li treba da cekamo podatke. Vraca istinu samo onda kada u kolekciji
     * nema podataka ali istovremeno i nije objavljen kraj stvaranja podataka.
    */
    bool daLiCekamPodatke() {
        // TODO
        return data.empty() && !kraj;
    }

    /**
     * Provera da li smo zavrsili sa citanjem podataka. Vraca istinu samo onda kada nema vise podataka
     * u kolekciji i sve niti stvaraoci podataka su se odjavili.
    */
    bool jeLiKraj() {
        // TODO
        return data.empty() && kraj;
    }
};


/** Klasa koja modeluje "postansko sanduce" izmedju radnika i pisaca.
*/
template<typename T>
class PodaciZaIspis {
private:
    mutex podaci_mx;                       // propusnica za sinhronizaciju nad svim poljima klase
    // TODO dodati polja po potrebi
    bool kraj;
    condition_variable con_v;
    queue<T> data;
    int br_stvaralaca_podataka;
public:
    PodaciZaIspis(): kraj(false), br_stvaralaca_podataka(0) {}  // na pocetku nije kraj i nema radnika

    void dodaj(T stanjeSkijalista) {
        // TODO
        unique_lock<mutex> lock(podaci_mx);
        data.push(stanjeSkijalista);
        con_v.notify_one();

    }

    bool preuzmi(T &stanjeSkijalista) {
        // TODO
        unique_lock<mutex> lock(podaci_mx);
        while(daLiCekamPodatke()) {
            con_v.wait(lock);
        }

        if(jeLiKraj()) return false;

        stanjeSkijalista = data.front();
        data.pop();
        return true;
    }

    // TODO dodati metode po potrebi

    void dodajRadnika() {
        unique_lock<mutex> lock(podaci_mx);
        br_stvaralaca_podataka++;
    }

    void odjaviRadnika() {
        unique_lock<mutex> lock(podaci_mx);
        br_stvaralaca_podataka--;
        if(br_stvaralaca_podataka == 0) {
            kraj = true;
            con_v.notify_all();
        }
    }

private:
    /**
     * Provera da li treba da cekamo podatke. Vraca istinu samo onda kada u kolekciji
     * nema podataka ali istovremeno i nije objavljen kraj stvaranja podataka.
    */
    bool daLiCekamPodatke() {
        // TODO
        return data.empty() && !kraj;
    }

    /**
     * Provera da li smo zavrsili sa citanjem podataka. Vraca istinu samo onda kada nema vise podataka
     * u kolekciji i sve niti stvaraoci podataka su se odjavili.
    */
    bool jeLiKraj() {
        // TODO
        return data.empty() && kraj;
    }
};

void citac(vector<string> imena_ulaznih_datoteka, RedoviIzDatoteke<string>& redovi_iz_ulaznih_datoteka) {
    // TODO
    for(int i = 0; i < imena_ulaznih_datoteka.size(); i++) {
        ifstream ulazna(imena_ulaznih_datoteka[i]);
        if(ulazna.is_open()) {
            string line;
            while(getline(ulazna,line)) {
                redovi_iz_ulaznih_datoteka.dodaj(imena_ulaznih_datoteka[i].substr(0,imena_ulaznih_datoteka[i].length() - 4) + " " + line);
            }
            ulazna.close();
        }
    }
    redovi_iz_ulaznih_datoteka.krajCitanja();
}

void radnik(RedoviIzDatoteke<string>& redovi_iz_ulaznih_datoteka, PodaciZaIspis<StanjeSkijalistaNaDan>& pripremljeni_podaci){
    // TODO
    unique_lock<mutex> lock(mx);
    pripremljeni_podaci.dodajRadnika();
    string line;
    while(redovi_iz_ulaznih_datoteka.preuzmi(line)) {
        istringstream stream(line);
        string ime,dan,datum,temperatura;
        vector<int> temperature;

        if(!getline(stream,ime,' ')) continue;
        if(!getline(stream,dan,' ')) continue;
        if(!getline(stream,datum,' ')) continue;
        while(getline(stream,temperatura,' ')) {
            temperature.push_back(stoi(temperatura));
        }
        StanjeSkijalistaNaDan ski(ime,dan,datum,temperature);
        ski.setJeIdealan();

        pripremljeni_podaci.dodaj(ski);
    }
    pripremljeni_podaci.odjaviRadnika();
}

void pisac(PodaciZaIspis<StanjeSkijalistaNaDan>& pripremljeni_podaci) {
    // TODO
    ofstream idealno("idealno.txt");
    ofstream lose("lose.txt");

    StanjeSkijalistaNaDan ski;
    while(pripremljeni_podaci.preuzmi(ski)) {
        if(ski.jeIdealan()) {
            idealno << ski.getIme() << " " << "[" << ski.getDan() << " " << ski.getDatum() << "]" << " " << ski.getMinIdeal() << " >> " << ski.getMaxIdeal() << endl;
        } else {
            lose << ski.getIme() << " " << "[" << ski.getDan() << " " << ski.getDatum() << "]" << " " << ski.getMinIdeal() << " >> " << ski.getMaxIdeal() << endl;
        }
    }
    idealno.close();
    lose.close();
}

int main() {
    RedoviIzDatoteke<string> redovi_iz_ulaznih_datoteka;
    PodaciZaIspis<StanjeSkijalistaNaDan> pripremljeni_podaci;
    vector<string> imena_ulaznih_datoteka = {"Kopaonik.txt", "Zlatibor.txt", "Jahorina.txt"};

    thread th_reader(citac, imena_ulaznih_datoteka, ref(redovi_iz_ulaznih_datoteka));
    thread th_writer(pisac, ref(pripremljeni_podaci));
    thread th_workers[BR_RADNIKA];

    for(auto &th: th_workers){
        th = thread(radnik, ref(redovi_iz_ulaznih_datoteka), ref(pripremljeni_podaci));
    }

    th_reader.join();
    for(auto &th: th_workers) {
        th.join();
    }
    th_writer.join();
    
    return 0;
}
    /**
 * Napisati C++ program koji cita podatke o uniformnim distribucijama iz ulazne datoteke i potom za
 * svaku distribuciju generise po 10 brojeva, racuna njihov prosek, i nalazi najmanji i najveci broj. 
 * 
 * U ulaznoj datoteci "distribucije.txt" se u svakom redu nalaze informacije o donjoj i gornjoj granici
 * intervala u kojem treba generisati brojeve a potom vrednost koju treba iskoristiti kao jezgro (seed)
 * za generisanje nasumicnih brojeva. Navedene vrednosti su odvojene dvotackom. Granice su razlomljeni
 * brojevi a jezgro je veoma velika celobrojna vrednost.
 * 
 * U izlaznoj datoteci "brojevi.csv" treba da se nalaze u jednom redu odvojeni zarezom prvo 10 
 * izgenerisanih brojeva a potom i prosek, najmanji element i najveci element.
 * 
 * Treba napraviti jednu nit koja ce samo citati ulazne podatke, jednu nit koja ce samo pisati gotove
 * brojeve u datoteku i 6 niti
student@student-VirtualBox:~/Documents/OS/08.05_slucajni-brojevi$  radnika koji ce na osnovu podataka iz ulazne datoteke generisati sve
 * neophodno za ispis u izlaznu datoteku.
*/
#include<iostream>
#include<thread>
#include<mutex>
#include<vector>
#include<chrono>
#include <random>
#include <functional>
#include <fstream>
#include <ostream>
#include <regex>
#include <string>
#include <condition_variable>
#include <queue>

#define BROJ_RADNIKA 6

using namespace std;

typedef chrono::high_resolution_clock hrc_t;
hrc_t::time_point start = hrc_t::now();

struct trio {
    double donji,gornji;
    long long seed;
};

/** Klasa koja modeluje "postansko sanduce" izmedju citaca i radnika.
*/
template<typename T>
class UlazniPodaci {
private:
    mutex podaci_mx;                       // propusnica za sinhronizaciju nad svim poljima klase
    // TODO dodati polja ako je potrebno
    bool kraj;
    queue<T> data;
    condition_variable con_v;
public:
    UlazniPodaci(): kraj(false) {}  // na pocetku nije kraj i nema radnika

    void dodaj(T par_granica) {
        // TODO
        unique_lock<mutex> lock(podaci_mx);
        data.push(par_granica);
        con_v.notify_one();
    }

    bool preuzmi(T &par_granica) {
        // TODO
        unique_lock<mutex> lock(podaci_mx);
        while(daLiCekamPodatke()) {
            con_v.wait(lock);
        }
        if(jeLiKraj()) return false;
        par_granica = data.front();
        data.pop();
        return true;
    }

    void obavestiKraj() {
        kraj = true;
        con_v.notify_all();
    }

    // TODO dodati jos neku metodu ako je potrebno

private:
    /**
     * Provera da li treba da cekamo podatke. Vraca istinu samo onda kada u kolekciji
     * nema podataka ali istovremeno i nije objavljen kraj stvaranja podataka.
    */
    bool daLiCekamPodatke() {
        // TODO
        return !kraj && data.empty();
    }

    /**
     * Provera da li smo zavrsili sa citanjem podataka. Vraca istinu samo onda kada nema vise podataka
     * u kolekciji i sve niti stvaraoci podataka su se odjavili.
    */
    bool jeLiKraj() {
        // TODO
        return kraj && data.empty();
    }
};


/** Klasa koja modeluje "postansko sanduce" izmedju radnika i pisaca.
*/
template<typename T>
class IzlazniPodaci {
private:
    mutex podaci_mx;                       // propusnica za sinhronizaciju nad svim poljima klase
    // TODO dodati polja ako je potrebno
    bool kraj;
    int br_stvaralaca_podataka;
    queue<T> data;
    condition_variable con_v;
public:
    IzlazniPodaci(): kraj(false), br_stvaralaca_podataka(0) {}  // na pocetku nije kraj i nema radnika

    void dodaj(T brojevi) {
        // TODO
        unique_lock<mutex> lock(podaci_mx);
        data.push(brojevi);
        con_v.notify_one();
    }

    bool preuzmi(T &brojevi) {
        // TODO 
        unique_lock<mutex> lock(podaci_mx);
        while(daLiCekamPodatke()) {
            con_v.wait(lock);
        }
        if(jeLiKraj()) return false;
        brojevi = data.front();
        data.pop();
        return true;
    }

    void uvecaj() {
        unique_lock<mutex> lock(podaci_mx);
        br_stvaralaca_podataka++;
    }

    void smanji() {
        unique_lock<mutex> lock(podaci_mx);
        br_stvaralaca_podataka--;
        if(br_stvaralaca_podataka == 0) {
            kraj = true;
            con_v.notify_all();
        }
    }

    // TODO dodati jos neku metodu ako je potrebno

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


/**
 * Logika radnika - niti koje vrse transformaciju ulaznih podataka u izlazne podatke spremne za ispis.
*/
void radnik(UlazniPodaci<struct trio> &ulaz, IzlazniPodaci<vector<double>> &izlaz) {
    // TODO
    //izlaz.uvecaj();
    trio tempData;
    izlaz.uvecaj();
    while(ulaz.preuzmi(tempData)) {
        default_random_engine eng(tempData.seed);
        uniform_real_distribution<double> dist(tempData.donji, tempData.gornji);

        vector<double> v;
        for(int i = 0; i < 10; i++) {
            v.push_back(dist(eng));
        }
        izlaz.dodaj(v);
    }
    izlaz.smanji();
}

/**
 * Logika citaca_iz_datoteke - nit koja radi citanje iz ulazne datoteke i salje u ulaznu kolekciju za radnike
*/
void citacf(string ime_ulazne_dat, UlazniPodaci<struct trio> &ulaz) {
    // TODO 
    ifstream ulazna(ime_ulazne_dat);

    if(ulazna.is_open()) {
        string red_datoteke;
        while(getline(ulazna, red_datoteke)) {  
            string donji,gornji,seed;
            istringstream tok(red_datoteke);      

            if(!getline(tok,donji,':')) {
                continue;
            }
            if(!getline(tok,gornji,':')) {
                continue;
            }
            if(!getline(tok,seed,':')) {
                continue;
            }
            ulaz.dodaj({stod(donji),stod(gornji),stol(seed)});
        }
    }
    ulaz.obavestiKraj();
    ulazna.close();
}

/**
 * Logika pisaca_u_datoteku - nit koja radi pisanje u izlaznu datoteku podataka dobijenih od radnika
*/
void pisacf(IzlazniPodaci<vector<double>> &izlaz, string ime_izlazne_dat) {
    // TODO
    vector<double> v;
    ofstream izlazna(ime_izlazne_dat);
    while(izlaz.preuzmi(v)) {
        double max,min = v[0], prosek = 0.0;
        for(int i = 0; i < v.size(); i++) {
            izlazna << v[i] << ",";
            if(v[i] > max) max = v[i];
            if(v[i] < min) min = v[i];
            prosek+=v[i];
        }
        prosek/=10.0;
        izlazna << prosek << "," << min << "," << max << endl;
    }
  
}

int main() {;
    UlazniPodaci<struct trio> ulazni_podaci;  // bafer podataka koje salje citac_iz_datoteke a obradjuju radnici
    IzlazniPodaci<vector<double>> izlazni_podaci;  // bafer podataka koje pripremaju radnici a ispisuju se u datoteku u pisacu_u_datoteku
    thread citac{citacf, "distribucije.txt", ref(ulazni_podaci)},  // stvaranje niti citaca_iz_datoteke
           pisac{pisacf, ref(izlazni_podaci), "brojevi.csv"},      // stvaranje niti pisaca_u_datoteku
           radnici[BROJ_RADNIKA];

    for (auto &nit: radnici)
        nit = thread(radnik, ref(ulazni_podaci), ref(izlazni_podaci));  // stvaranje niti radnika

    for (auto &nit: radnici)
        nit.join();
    citac.join();
    pisac.join();

    return 0;
}

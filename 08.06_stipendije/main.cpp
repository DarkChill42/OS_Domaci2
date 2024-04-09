/**
 * Napisati C++ program koji cita podatke o studentima iz ulazne datoteke i potom 
 * za svakog studenta racuna prosek
 * 
 * U ulaznoj datoteci "studenti.csv" se u svakom redu nalaze informacije o studentu: 
 *     Ime,Prezime,Broj indeksa,Ocene 
 * Pritom su ocene odvojene medjusobno zarezima.
 * 
 * Prilikom obrade podataka o studentima, mora se proveriti format indeksa da li je validan. Ako nije, zanemariti taj unos.
 * Format indeksa je:
 *     [[:alpha:]][[:alnum:]]{1,2}\s[[:digit:]]{1,3}\/[[:digit:]]{4}
 * 
 * U tri izlazne datoteke rasporediti studente u zavisnosti od proseka.
 * Ukoliko je prosek > 9.00 potrebno je upisati studenta u datoteku "kandidati_stipendija.csv".
 * Ukoliko je prosek > 8.00 i prosek <= 9.00 potrebno je upisati studenta u datoteku "kandidati_kredit.csv".
 * U ostalim slucajevima upisati studenta u datoteku "ostali.csv".
 * Format u izlaznoj datoteci treba da odgovara sledecem: Ime,Prezime,Broj_indeksa,prosek
 * 
 * Treba napraviti jednu nit koja ce samo citati redove ulazne podatke, jednu nit 
 * koja ce samo pisati gotove podatke u izlazne datoteke i 10 niti radnika koji ce na osnovu redova
 * iz ulazne datoteke generisati sve neophodno za ispis u izlaznu datoteku.
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


using namespace std;

mutex mx;


class Student {
private:
    string ime,prezime,indeks;
    vector<int> ocene;
public:
    Student(string i, string p, string in, vector<int> o) : ime(i), prezime(p), indeks(in), ocene(o) {}
    Student() {}

    vector<int> const getOcene() {return ocene;}

    string const getIme() {return ime;}
    string const getPrezime() {return prezime;}
    string const getIndex() {return indeks;}
};

template<typename T>
class InputData {
private:
    mutex data_mtx;
    // TODO dodati polja po potrebi
    bool end;
    condition_variable con_v;
    queue<T> data;

public:
    InputData(): end(false) {
        // TODO
    }
    void add_data(T data_element) {
        // TODO
        unique_lock<mutex> lock(data_mtx);
        data.push(data_element);
        con_v.notify_one();
    }
    bool remove_data(T &data_element) {
        // TODO
        unique_lock<mutex> lock(data_mtx);
        while(data.empty() && !end) {
            con_v.wait(lock);
        }
        if(data.empty() && end) return false;
        data_element = data.front();
        data.pop();
        return true;
    }

    // TODO dodati metode po potrebi
    void notifyEnd() {
        unique_lock<mutex> lock(data_mtx);
        end = true;
        con_v.notify_all();
    }
};

template<typename T>
class OutputData {
private:
    mutex data_mtx;
    // TODO dodati polja po potrebi
    bool end;
    int data_producers_num;
    condition_variable con_v;
    queue<T> data;
public:
    OutputData(): end(false), data_producers_num(0) {}
    void add_data(T data_element) {
        unique_lock<mutex> lock(data_mtx);
        data.push(data_element);
        con_v.notify_one();
    }
    bool remove_data(T &data_element) {
        unique_lock<mutex> lock(data_mtx);
        while(data.empty() && !end) {
            con_v.wait(lock);
        }
        if(the_end()) return false;
        data_element = data.front();
        data.pop();
        return true;
    }

    bool the_end() {
        return data.empty() && end;
    }

    // TODO dodati metode po potrebi

    void workerInc() {
        unique_lock<mutex> lock(data_mtx);
        data_producers_num++;
    }

    void workerDec() {
        unique_lock<mutex> lock(data_mtx);
        data_producers_num--;
        if(data_producers_num == 0) {
            end = true;
            con_v.notify_all();
        }
    }
    
};

void reader(string input_file_name, InputData<string>& raw_data) {
    // TODO
    ifstream ulazna(input_file_name);

    if(ulazna.is_open()) {
        string red_datoteke;
        while(getline(ulazna, red_datoteke)) {  
            raw_data.add_data(red_datoteke);
        }
    }
    raw_data.notifyEnd();
    ulazna.close();
}

bool checkIndexFormat(string index) {
    regex indexRegex("[[:alpha:]][[:alnum:]]{1,2}\\s[[:digit:]]{1,3}\\/[[:digit:]]{4}");
    return regex_match(index,indexRegex);
}

void proccessing_data(InputData<string>& raw_data, OutputData<Student>& proccessed_data){
    // TODO
    //unique_lock<mutex> lock(mx);
    string tempData;
    proccessed_data.workerInc();
    while(raw_data.remove_data(tempData)) {
        string ime,prezime,index;
        vector<int> ocene;
        istringstream tok(tempData);

        if(!getline(tok,ime,',')) continue;
        if(!getline(tok,prezime,',')) continue;
        if(!getline(tok,index,',')) continue;

        string ocena;
        while(getline(tok,ocena,',')) {
            ocene.push_back(stoi(ocena));
        }

        if(checkIndexFormat(index)) {
            Student s(ime,prezime,index,ocene);
            proccessed_data.add_data(s);
        }
    }
    proccessed_data.workerDec();
}

/*
 * U tri izlazne datoteke rasporediti studente u zavisnosti od proseka.
 * Ukoliko je prosek > 9.00 potrebno je upisati studenta u datoteku "kandidati_stipendija.csv".
 * Ukoliko je prosek > 8.00 i prosek <= 9.00 potrebno je upisati studenta u datoteku "kandidati_kredit.csv".
 * U ostalim slucajevima upisati studenta u datoteku "ostali.csv".
 * Format u izlaznoj datoteci treba da odgovara sledecem: Ime,Prezime,Broj_indeksa,prosek


*/

void writer(OutputData<Student>& proccessed_data) {
    // TODO
    Student s;
    ofstream stipendija("kandidati_stipendija.csv");
    ofstream kredit("kandidati_kredit.csv");
    ofstream ostali("ostali.csv");
    while(proccessed_data.remove_data(s)) {
        double prosek = 0.0;
        for(int i = 0; i < s.getOcene().size(); i++) prosek+= s.getOcene()[i];
        prosek/=s.getOcene().size();

        if(prosek > 9.00) {
            stipendija << s.getIme() << "," << s.getPrezime() << "," << s.getIndex() << "," << prosek << endl;
        } else if(prosek > 8.00 && prosek <= 9.00) {
            kredit << s.getIme() << "," << s.getPrezime() << "," << s.getIndex() << "," << prosek << endl;
        } else {
            ostali << s.getIme() << "," << s.getPrezime() << "," << s.getIndex() << "," << prosek << endl;
        }
    }
    stipendija.close();
    kredit.close();
    ostali.close();
}


int main() {
    InputData<string> raw_data;
    OutputData<Student> proccessed_data;

    thread th_reader(reader, "studenti.csv", ref(raw_data));
    thread th_writer(writer, ref(proccessed_data));
    thread th_workers[10];

    for(auto &th: th_workers){
        th = thread(proccessing_data, ref(raw_data), ref(proccessed_data));
    }

    th_reader.join();
    for(auto &th: th_workers) {
        th.join();
    }
    th_writer.join();
    
    return 0;
}
/*
    Ako zamislite jedinični kvadrat centriran oko koordinatnog početka i krug upisan
    u taj kvadrat. Verovatnoća da će nasumično odabrana tačka u okviru kvadrata
    biti i u krugu, pod uslovom da je distribucija ravnomerna, je ravna odnosu njihovih
    površina, tj, ravna je PI/4. Iskoristiti ovaj podataka da kroz Monte Karlo simulaciju
    izračunate vrednost broja PI tako što probate ovaj eksperiment veliki broj puta i vodite
    računa o tome koliko puta dobijete tačku u krugu, a koliko puta van kruga, te na kraju
    simulacije izračunate izmerenu verovatnoću i na osnovu nje, sračunate vrednost broja pi.
*/

#include <iostream>
#include <random>
#include <functional>
#include <chrono>
#include <cmath>

using namespace std;

typedef chrono::high_resolution_clock hrc_t;

hrc_t::time_point start = hrc_t::now();

//10^6 default
#define BROJ_TACAKA 1e6

//Racuna udaljenost od centra.
float udaljenost(float x, float y) {
    return sqrt(pow(x,2) + pow(y,2));
}

int main() {
    default_random_engine generator;
    hrc_t::duration d = hrc_t::now() - start;
    generator.seed(d.count());

    uniform_real_distribution<double> rdist(-0.5, 0.5);
    auto dr = bind(rdist, generator);

    int uKrugu = 0;
    for(int i = 0; i < BROJ_TACAKA; i++) {
        if(udaljenost(dr(), dr()) < 0.5) {
            uKrugu++;
        }
    }
    float pi = 4 * uKrugu / BROJ_TACAKA;
    float absDiff = abs(M_PI - pi);
    cout << "Broj generisanih nasumicnih tacaka: " << BROJ_TACAKA << ". Procenjena vrednost za PI je: " << pi << endl;
    cout << "Apsolutna razlika tacne i generisane vrednosti: " << absDiff << endl;
}

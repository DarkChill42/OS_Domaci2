/*
    Koristeći konzolu i nasumično generisane slučajne brojeve
    po normalnoj distribuciji, vizuelizovati Gausovu krivu
    koristeći zvezdice. Gausova kriva po X osi ima moguće
    vrednosti koje su generisane, a po Y osi koliko su vrednosti
    u tom delu X ose zastupljene, procentualno, u svim
    generisanim vrednostima. Za opseg Y uzmite od 0 do
    najveće izmerene vrednosti od svih Y vrednosti,
    dok za X uzmite plus-minus 2.5 standardne devijacije
    od srednje vrednosti. Veličina uzorka slučajnih vrednosti,
    tj. koliko da generišete pre nego probate neka vam je
    8192.

    Gausova kriva za srednju vrednost 5
    i standardnu devijaciju 2 izlgeda otprilike ovako ako se
    posmatra rotirano za 90 stepeni, kao u zadatku pod A:
    0-1: *
    1-2: ****
    2-3: *********
    3-4: ***************
    4-5: ******************
    5-6: *******************
    6-7: ***************
    7-8: ********
    8-9: ****
    9-10: *
    Gausova kriva za srednju vrednost 0 i SD 1, nacrtana
    uspravno izgleda otprilike ovako:
                                        * *
                                     *  ***
                                     * ***** *
                                  *  **********
                                  *  ********** *
                              ***************** *
                            * *******************
                            *********************
                           ************************ **
                          ****************************
                         *******************************
                       * *******************************
                      ********************************** *
                    * ********************************** *
                    ****************************************
                    ******************************************
                 * *******************************************
                ************************************************
                **************************************************
            *******************************************************
            **********************************************************
       * * ************************************************************
   *  ********************************************************************
   **************************************************************************
********************************************************************************
    Odabrati jedan:
    A) (Lakše) Prikazati Gausovu krivu rotiranu za 90 stepeni.
    B) (Teže) Prikazati je normalno
*/

#include <iostream>
#include <random>
#include <functional>
#include <chrono>

using namespace std;

typedef chrono::high_resolution_clock hrc_t;
hrc_t::time_point start = hrc_t::now();

#define BROJ_ITERACIJA 1e5



int main(){
    default_random_engine generator;
    hrc_t::duration d = hrc_t::now() - start;
    generator.seed(d.count());

    normal_distribution<double> gaus(5.0, 2.0);
    auto gausBroj = bind(gaus, generator);
    int brojPojavljivanja[10] = {};

    for(int i = 0; i < BROJ_ITERACIJA; i++){
      float gb = gausBroj();
      if(gb >= 0.0 && gb <= 1.0) brojPojavljivanja[0]++;
      else if(gb > 1.0 && gb <= 2.0) brojPojavljivanja[1]++;
      else if(gb > 2.0 && gb <= 3.0) brojPojavljivanja[2]++;
      else if(gb > 3.0 && gb <= 4.0) brojPojavljivanja[3]++;
      else if(gb > 4.0 && gb <= 5.0) brojPojavljivanja[4]++;
      else if(gb > 5.0 && gb <= 6.0) brojPojavljivanja[5]++;
      else if(gb > 6.0 && gb <= 7.0) brojPojavljivanja[6]++;
      else if(gb > 7.0 && gb <= 8.0) brojPojavljivanja[7]++;
      else if(gb > 8.0 && gb <= 9.0) brojPojavljivanja[8]++;
      else if(gb > 9.0 && gb <= 10.0) brojPojavljivanja[9]++;
    }

    for(int i = 0; i < 10; i++) {
      cout << (i) << "-" << i+1 << ": ";
      for(int j = 0; j < brojPojavljivanja[i] / 500; j++) {
        cout << "*";
      }
      cout << endl;
    }
}

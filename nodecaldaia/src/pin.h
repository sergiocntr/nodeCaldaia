#ifndef pincaldaia_h
#define pincaldaia_h
static const uint8_t D11   = 9;
static const uint8_t D12  = 10;
const int caldaiaPin= D4;  //pin collegato all'ex termostato
const int acquaPin= D12;    //pin collegato al micro dell'acqua sanitaria
                            //questo pin viene attivato dal programma e da il consenso
                            //all acqua sanitaria.Si attiva basso perche la scheda ex sfinge
                            //lavora con positivo comune
const int acquaIntPin= D11; //questo pin e' collegato al micro e viene portato basso con una resistenza di
                            // 660 ohm quando si chiama acqua dalla casa.
                            //Con interrupt e ritardo di 5 secondi attiva il rele caldaia
const int resetPin= D3;     //pin pulsante reset blocco
const int alarmPin=D5;      //pin luce blocco caldaia
const int valvePin=A0;    //lettura voltaggio valvola gas
#endif

#ifndef pincaldaia_h
#define pincaldaia_h
static const uint8_t D11   = 9;
static const uint8_t D12  = 10;
const uint8_t caldaiaPin= D4;  //pin collegato all'ex termostato
const uint8_t acquaPin= D12;    //pin collegato al micro dell'acqua sanitaria
                            //questo pin viene attivato dal programma e da il consenso
                            //all acqua sanitaria.Si attiva basso perche la scheda ex sfinge
                            //lavora con positivo comune
const uint8_t acquaIntPin= D11; //questo pin e' collegato al micro e viene portato basso con una resistenza di
                            // 660 ohm quando si chiama acqua dalla casa.
                            //Con interrupt e ritardo di 5 secondi attiva il rele caldaia
const uint8_t resetPin= D3;     //pin pulsante reset blocco
const uint8_t alarmPin=D5;      //pin luce blocco caldaia
const uint8_t valvePin=A0;    //lettura voltaggio valvola gas
const uint8_t enableDisplay=D6; //abilitazione 5v x il Display
#endif

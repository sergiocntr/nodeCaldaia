#ifndef pincaldaia_h
#define pincaldaia_h
const uint8_t caldaiaPin= D6;  //pin collegato all'ex termostato//era D4 ma conflitto con TXD e faceva scattare rele
const uint8_t acquaPin= D7;    //pin collegato al micro dell'acqua sanitaria
//const uint8_t resetPin= D3;     //pin pulsante reset blocco
//const uint8_t alarmPin=D5;      //pin luce blocco caldaia
const uint8_t valvePin=A0;    //lettura voltaggio valvola gas
//const uint8_t enableDisplay=D6; //abilitazione 5v x il Display
#endif

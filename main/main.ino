#include "Arduino.h"
#include "ssp.h"

uint8 txframe[dt];
uint8 data[info];
uint8 data2[info];
uint8 rxframe[dt];
uint8 Rx_data[info];
uint8 layerdata[info];

uint8 type2;

uint8 checkcontrol = EMPTY;
uint16 data_length;
uint8 dataflag = EMPTY;
uint8 rxflag = EMPTY;
void receive_frame_here() {
  if (rxflag == EMPTY) {
    if (Serial.available() > 0) {
      Serial.readBytes(rxframe, 236);
      serial_flush_buffer();
      rxflag = FULL;
      Serial1.println("\n Received frame\n");
      Serial1.flush();
    }
  }
}

void serial_flush_buffer()
{
  while (Serial.read() >= 0); // do nothing
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  if (checkcontrol == EMPTY) {
    getdata(data, &data_length, &dataflag);
    checkcontrol = FULL;
  }

}

void loop() {

  static uint8 txflag = EMPTY;
  uint8 i;
  static uint8 crcflag = EMPTY;
  uint8 desti = 0x01;
  uint8 srce;
  uint8 typee = 0x02;
  uint8 desti2;
  static uint16 tx_size = 0;
  uint8 adddest;
  uint8 addsrc;
  static uint8 type = 0;
  static uint16 Rx_length = 0;
  static uint8 layerflag = EMPTY;
  static uint8 deframeflag = EMPTY;
  static uint8 framingflag = EMPTY;




  delay(100);

  if (checkcontrol == EMPTY) {
    getdata(data, &data_length, &dataflag);
    checkcontrol = FULL;
  }

  if ((checkcontrol == FULL && txflag == EMPTY)
      || (checkcontrol == EMPTY && layerflag == EMPTY)) {
    control_layer(data, data_length, desti, &srce, typee, &type2, data2, &desti2,
                  &type, Rx_data, &adddest, Rx_length, &dataflag, &deframeflag,
                  &txflag, layerdata, crcflag, &tx_size, &addsrc, &layerflag, &checkcontrol);

    layerflag = EMPTY;

  }


  if (txflag == FULL) {
    ssp_build_frame(txframe, data2, desti2, srce, type2, tx_size, &txflag);
  }

  receive_frame_here();


  if (rxflag == FULL && deframeflag == EMPTY ) {

    ssp_deframing(rxframe, &adddest, &addsrc, &type, Rx_data, &Rx_length,
                  &rxflag, &crcflag, &deframeflag);

  }



}

byte rs = 9;
byte E = 8;

byte data = 10;
byte clock = 11;
byte latch = 12;

byte line[2]{0,16};
byte descLine[16]{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};;
unsigned long lasmil = 0;

byte trans[2]{3,3};
byte transPoint[14]{1,3,6,8,10,11,13, 18,20,21,23,25,28,30};
byte descTransPoint[8]{15,13,12,10,8,6,5,3};
bool side = 0;
byte wait = 0;

int pot;
byte potMap = 0;

byte lastStation[3]{0,0,0};

byte lb = 0;

byte mode = -1;
bool enable = 0;

byte pay = 0;
byte payAdd = 0;

byte savePos = 0;
byte lastPos = 0;

byte count = 0;
bool enableCount = 0;

bool refreshState = 0;

char station[16][20] = {
  {"Pasar Senen"},
  {"Bekasi"},
  {"Cikampek"},
  {"Jatibarang"},
  {"Cirebon"},
  {"Brebes"},
  {"Tegal"},
  {"Pekalongan"},
  {"Semarang Tawang"},
  {"Gambringan"},
  {"Bojonegoro"},
  {"Pasar Turi"},
  {"Wonokromo"},
  {"Bangil"},
  {"Jember"},
  {"Banyuwangi"}
};


void write4Bit(byte n, byte rs_state){
  digitalWrite(rs, rs_state);
  for(byte i = 0; i < 4; i++){
    digitalWrite(4+i, (n >> 3-i) & 0x01);
  }
  digitalWrite(E, 1);
  delay(1);
  digitalWrite(E, 0);
}

void write_cmd(byte cmd){
  write4Bit(((cmd >> 4) & 0x0f), 0);
  write4Bit((cmd & 0x0f), 0);
}

void write_data(byte n){
  write4Bit(((n >> 4) & 0x0f), 1);
  write4Bit((n & 0x0f), 1);
}

void LCDinit(){
  delay(50);
  byte cmdData[7]{0x33,0x32,0x28,0x08,0x06,0x0c,0x01};
  for(byte i = 0; i < 6; i++){
    write_cmd(cmdData[i]);
  }
  delay(1);
}

void LCDsetCursor(byte x, byte y){
  int col = 0x80;
  switch(y){
    case 0:
      col |= 0x00;
      break;
    case 1:
      col |= 0x40;
      break;
    case 2:
      col |= 0x14;
      break;
    case 3:
      col |= 0x54;
      break;
  }
  col += x;
  write_cmd(col);
}

void LCDclear(){
  write_cmd(0x01);
}

void LCDprint(char *str){
  while(*str)write_data(*str++);
}

void LCDprintNumb(byte x) {
    if(x >= 100) {
        write_data('0' + x/100);
        x %= 100;
    }
    if(x >= 10) {
        write_data('0' + x/10);
        x %= 10;
    }
    write_data('0' + x);
}

void pos(byte x){
  for (int i = 32; i >= 0; i--) {
      digitalWrite(data, (i == (x)) ? 1 : 0);
      digitalWrite(clock, 0);
      digitalWrite(clock, 1);
  }
  digitalWrite(latch, 1);
  digitalWrite(latch, 0);
}

void transit(byte x){
  for(byte i = 0; i < 14; i++){
    if(line[x] == transPoint[i]+1){
      if(trans[x] != 0){
        line[x]--;
        trans[x]--;
      }
    }
  }
  if(trans[x] == 0){
    line[x]++;
    trans[x] = 3;  
  }
}

void refresh1() {
  if (lastStation[0] != line[0]) {
    LCDsetCursor(1, 1);
    LCDprint("                  ");   
    lastStation[0] = line[0];   
  }
  
  if (lastStation[1] != line[1]) {
    LCDsetCursor(1, 3);
    LCDprint("                  "); 
    lastStation[1] = line[1];  
  }
}

void refresh2(){
  if(lastStation[2] != potMap){
    LCDsetCursor(1, 3);
    LCDprint("                  ");
    lastStation[2] = potMap;
  }
}

void refresh3(){
  if (refreshState == 0) {
    LCDsetCursor(3, 3);
    LCDprint("  s");   
    refreshState = 1;
  }
}

void refresh4(){
  if (lastStation[side] != line[side]) {
    LCDsetCursor(1,1);
    LCDprint("H[                ]");
    LCDsetCursor(1,2);
    LCDprint("N[                ]");
    lastStation[side] = line[side];
  }
  if(refreshState == 0){
    LCDsetCursor(1,3);
    LCDprint("TIME:  ");
    refreshState = 1;
  }
}

void refresh5(){
  if(refreshState == 0){
    LCDsetCursor(12,2);
    LCDprint("  ");
    LCDsetCursor(12,3);
    LCDprint("  ");

    refreshState = 1;
  }
}

void screen1(){
  LCDsetCursor(5, 1);
  LCDprint("Bukan KRL");
  for(byte i = 0; i < 19; i++){
    LCDsetCursor(i, 3);
    LCDprint("/");
    delay(150);
  }
  LCDclear();
  LCDsetCursor(7, 1);
  LCDprint("By upi");

  digitalWrite(data, 1);
  for(byte i = 0; i < 32; i++){
    digitalWrite(clock, 0);
    digitalWrite(clock, 1);
    digitalWrite(latch, 1);
    digitalWrite(latch, 0);
    digitalWrite(data, 0);
    delay(100);
  }
  LCDclear();
}

void screen2(){
  LCDsetCursor(0,0);
  LCDprint("-------LINE 1-------");
  LCDsetCursor(0,2);
  LCDprint("-------LINE 2-------");
  LCDsetCursor(0,1);
  LCDprint("[                  ]");
  LCDsetCursor(0,3);
  LCDprint("[                  ]");
}

void screen3(){
  LCDsetCursor(0,0);
  LCDprint("LOKASI SAAT INI");
  LCDsetCursor(0,1);
  LCDprint("[                  ]");
  LCDsetCursor(0,2);
  LCDprint("PILIH STASIUN TUJUAN");
  LCDsetCursor(0,3);
  LCDprint("[                  ]");
}

void screen4(){
  LCDsetCursor(2,0);
  LCDprint("BIAYA PERJALANAN");
  LCDsetCursor(0,1);
  LCDprint("1[ 5.000           ]");
  LCDsetCursor(0,2);
  if(payAdd == 0){
    LCDprint("2[                 ]");
  }else{
    LCDprint("2[  .000           ]");
  }
  
  LCDsetCursor(2,3);
  LCDprint("Rp   .000");

  LCDsetCursor((payAdd / 10) ? 2 : 3,2);
  if(payAdd != 0)LCDprintNumb(payAdd);

  LCDsetCursor((pay / 10) ? 5 : 6,3);
  LCDprintNumb(pay);
  
}

void screen5(){
  LCDsetCursor(0,0);
  LCDprint("----KEBERANGKATAN---");
  LCDsetCursor(1,1);
  LCDprint("[                ]");
  LCDsetCursor(0,2);
  LCDprint("-----KEDATANGAN-----");
  LCDsetCursor(2,3);
  LCDprint("[  S] LINE");
}

void screen6(){
  LCDsetCursor(0,0);
  LCDprint("PERJALANAN DI LINE");
  LCDsetCursor(1,1);
  LCDprint("H[                ]");
  LCDsetCursor(1,2);
  LCDprint("N[                ]");
  LCDsetCursor(1,3);
  LCDprint("TIME:   seconds");

  LCDsetCursor(19,0);
  switch(side){
    case 0:
      LCDprint("1");
      break;
    case 1:
      LCDprint("2");
      break;
  }
}

void screen7(){
  LCDsetCursor(3,1);
  LCDprint("ANDA TIBA DI");
  LCDsetCursor(0,2);
  LCDprint("[                  ]");
  delay(800);
  LCDsetCursor(1,2);
  LCDprint(station[savePos]);
  delay(800);
  LCDsetCursor(0,2);
  LCDprint("[                  ]");
  delay(800);
  LCDsetCursor(1,2);
  LCDprint(station[savePos]);
  delay(2000);
}

void screen8(){
  LCDsetCursor(0,0);
  LCDprint("---PEMBAYARAN KRL---");
  LCDsetCursor(3,1);
  LCDprint("TARIF[Rp   .000]");
  LCDsetCursor(3,2);
  LCDprint("BAYAR[Rp   .000]");
  LCDsetCursor(3,3);
  LCDprint("SISA [Rp       ]");

  LCDsetCursor((((pay) / 10) >= 1 )? 12 : 13,1);
  LCDprintNumb(pay);
}

void screen9(){
  LCDsetCursor(2,1);
  LCDprint("UANG TIDAK CUKUP");
  LCDsetCursor(6,2);
  LCDprint("TWIN -_-");
  delay(2000);
  LCDclear();
  mode--;
}

void screen10(){
  LCDsetCursor(1,1);
  LCDprint("PEMBAYARAN BERHASIL");
  LCDsetCursor(9,2);
  LCDprint(":)");
  delay(2000);
  mode = 0;
}

void setup() {
  for(byte i = 0; i < 12; i++){
    pinMode(i, OUTPUT);
  }
  pinMode(A1, INPUT_PULLUP);
  pinMode(rs, OUTPUT);
  pinMode(E, OUTPUT);
  

  for(byte i = 0; i < 32; i++){
    digitalWrite(data, 0);

    digitalWrite(clock, 0);
    digitalWrite(clock, 1);

    digitalWrite(latch, 1);
    digitalWrite(latch, 0);
  }

  LCDinit();
  LCDclear();

  screen1();
}
byte ls;

void loop() {
{
  if(digitalRead(A1) == 1 && lb == 0){
    ls = 0;

    if(enable == 1)mode--;
    if(mode == 2)savePos = potMap;
    mode++;
    lb = 1;
    LCDclear();
  }
  if(digitalRead(A1) == 0)lb = 0;
}
{
  unsigned long mils = millis();
  if((mils - lasmil) >= 1000){
    refreshState = 0;

    if(enableCount == 1){
      count-=1;
      LCDsetCursor(7,3);
      LCDprint("  seconds");
    }

    line[0]++;
    line[1]++; 
    
    transit(0);
    transit(1);

    lasmil = mils;
  }

  if(line[0] > 15)line[0] = 0;
  if(line[1] > 31)line[1] = 16;

  pos(line[0]);
  pos(line[1]);
  if(line[0] == 0 || line[1] == 16){
    pos(15);
    pos(31);
  }
}

  switch(mode){
    case 0:
    {
      if(ls == 0) {
        screen2();
        ls = 1;
      }
      refresh1();

      LCDsetCursor(1,1);
      LCDprint(station[line[0]]);
      LCDsetCursor(1,3);
      LCDprint(station[15 - (line[1] - 16)]);

      break;
    }
    case 1:
    {
      if(ls == 0){
        screen3();
        LCDsetCursor(1,1);
        LCDprint(station[savePos]);
        ls = 1;
      }
      
      pot = analogRead(A0);
      potMap = map(pot, 0, 1023, 0, 15);
      refresh2();
      LCDsetCursor(1,3);
      LCDprint(station[potMap]);

      if(potMap < savePos-3)
      {
        payAdd = ((savePos - 3) - potMap)*3;
      }else if(potMap > savePos+3){
        payAdd = (potMap - (savePos + 3))*3;
      }else{
        payAdd = 0;
      }
      pay = 5 + payAdd;

      if(savePos == potMap){
        enable = 1;
      }else {
        enable = 0;
      }
      break;
    }
    case 2:
    {
      if(ls == 0){
        screen4();
        ls = 1;
      }
      break;
    }
    case 3:
    {
      if(ls == 0){
        screen5();   
        
        LCDsetCursor(2,1);
        LCDprint(station[lastPos]);
        
        if(lastPos > savePos && lastPos != 15)lastPos = descLine[lastPos]+1; 
        ls = 1;
      }

      if(lastPos < savePos && lastPos != 0){
        side = 0;

        if(lastPos < line[0]){
          wait = 15 - line[0] + lastPos;
        }else{
          wait = lastPos - line[0];
        }
        
        LCDsetCursor(13,3);
        LCDprint("1");
      }
      if(lastPos > savePos && lastPos != 15){
        side = 1;
       
        if(lastPos < line[1] - 15){
          wait = (15 + lastPos) - (line[1] - 15);
        }else if(lastPos > line[1] - 15){
          wait = lastPos - (line[1] - 15);
        }else{
          wait = 0;
        }

        LCDsetCursor(13,3);
        LCDprint("2");
      }

      if(lastPos != 0 && lastPos != 15)digitalWrite(2, (side) ? 0 : 1);digitalWrite(3, (side) ? 1 : 0);

      if(lastPos == 0){
        side = 0;
        wait = 15 - ((line[1]) - 16);
        LCDsetCursor(13,3);
        LCDprint("2");

        digitalWrite(2, 0);
        digitalWrite(3, 1);
      }
      if(lastPos == 15){
        side = 1;
        wait = lastPos - line[0];
        LCDsetCursor(13,3);
        LCDprint("2");

        digitalWrite(2, 1);
        digitalWrite(3, 0);
      }

      refresh3();
      LCDsetCursor(((wait) / 10 == 1 ) ? 3 : 4,3);
      if(wait != 0){
        LCDprintNumb(wait);
        enable = 1;
      }else if(wait == 0){
          LCDsetCursor(3,3);
          LCDprint("--");
          enable = 0;
        
      }
      break;
    }    
    case 4:
    {
      if(ls == 0){
          digitalWrite(2, (side) ? 0 : 1);
          digitalWrite(3, (side) ? 1 : 0);
          screen6();

          if(side == 0){
              count = (savePos - lastPos) + 1;
          }else{
              count = ((descLine[savePos]) - lastPos); 
          }
          for(byte i = 0; i < 14; i++){
              if((side == 0 && transPoint[i] > lastPos && transPoint[i] < savePos)
              || (side == 1 && descTransPoint[i] > lastPos && descTransPoint[i] > descLine[savePos])){
                  count += 2;
              }
          }
          
          ls = 1;
          enableCount = 1;
      }
      
      switch(side){
        case 0:
        {
          LCDsetCursor(3,1);
          LCDprint(station[line[0]]);

          LCDsetCursor(3,2);
          LCDprint(station[line[0] + 1]);
          break;
        }
        case 1:
        {
          LCDsetCursor(3,1);
          LCDprint(station[descLine[line[1] - 16]]);

          LCDsetCursor(3,2);
          LCDprint(station[descLine[line[1] - 16] - 1]);
          break;
        }
      }

      LCDsetCursor(((count) / 10) == 1 ? 7 : 8, 3);
      LCDprintNumb(count);

      refresh4();
      if(count == 0){
        digitalWrite(2, 0);
        digitalWrite(3, 0);
        count = 0;
        enableCount = 0;
        wait = 0;
        lastPos = savePos;

        mode+=1;
        LCDclear();
      }
      break;
    }
    case 5:
    {
      screen7();
      LCDclear();
      mode+=1;
      ls = 0;
      break;
    }
    case 6:
    {
      if(ls == 0){
        screen8();
        ls = 1;
      }

      pot = analogRead(A0);
      potMap = map(pot, 0, 1023, 3, 70);

      refresh5();

      LCDsetCursor((((potMap) / 10 ) >= 1) ? 12 : 13,2);
      LCDprintNumb(potMap);

      if(potMap > pay){
        LCDsetCursor(14,3);
        LCDprint(".000");
        LCDsetCursor((((potMap - pay) / 10) >= 1) ? 12 : 13, 3);
        LCDprintNumb(potMap - pay); 
      }else if(potMap <= pay){
        LCDsetCursor(13,3);
        LCDprint("  ");
        LCDsetCursor(14,3);
        LCDprint("    ");
      }
      break;
    }
    case 7:
    {
      if(potMap < pay){
        screen9();
      }else{
        mode++;
      }
      ls = 0;
      break;
    }
    case 8:
    { 
      lastPos = savePos;
      ls = 0;
      screen10();
      break;
    }
  };
}
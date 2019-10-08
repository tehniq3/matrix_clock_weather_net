
void getTimeLocal()
{
  time_t tT = now();
  // local time
  time_t tTlocal = CE.toLocal(tT);
  w = weekday(tTlocal);
  d = day(tTlocal);
  mo = month(tTlocal);
  ye = year(tTlocal);
  h = hour(tTlocal);
  m = minute(tTlocal);
  s = second(tTlocal);
  localMillisAtUpdate = millis();
  //client.stop();
}

void showSimpleClock()
{
  dx = dy = 0;
  clr();
  showDigit(h / 10,  0, dig6x8);
  showDigit(h % 10,  8, dig6x8);
  showDigit(m / 10, 17, dig6x8);
  showDigit(m % 10, 25, dig6x8);
//  showDigit(s / 10, 34, dig6x8);
//  showDigit(s % 10, 42, dig6x8);
  setCol(15, dots ? B00100100 : 0);
  setCol(32, dots ? B00100100 : 0);
  refreshAll();
}

// =======================================================================

void showAnimClock()
{
  byte digPos[6] = {0, 8, 17, 25, 34, 42};
  int digHt = 12;
  int num = 6;
  int i;
  if (del == 0) {
    del = digHt;
    for (i = 0; i < num; i++) digold[i] = dig[i];
    dig[0] = h / 10 ? h / 10 : 10;
    dig[1] = h % 10;
    dig[2] = m / 10;
    dig[3] = m % 10;
//    dig[4] = s / 10;
//    dig[5] = s % 10;
    for (i = 0; i < num; i++)  digtrans[i] = (dig[i] == digold[i]) ? 0 : digHt;
  } else
    del--;

  clr();
  for (i = 0; i < num; i++) {
    if (digtrans[i] == 0) {
      dy = 0;
      showDigit(dig[i], digPos[i], dig6x8);
    } else {
      dy = digHt - digtrans[i];
      showDigit(digold[i], digPos[i], dig6x8);
      dy = -digtrans[i];
      showDigit(dig[i], digPos[i], dig6x8);
      digtrans[i]--;
    }
  }
  dy = 0;
  setCol(15, dots ? B00100100 : 0);
  setCol(32, dots ? B00100100 : 0);
  refreshAll();
  delay(30);
}




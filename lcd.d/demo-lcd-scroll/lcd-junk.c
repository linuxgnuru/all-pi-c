
/*
  lcdPosition (lcdHandle, 0, 0) ; lcdPuts (lcdHandle, "Gordon Henderson") ;
  lcdPosition (lcdHandle, 0, 1) ; lcdPuts (lcdHandle, "  wiringpi.com  ") ;

  waitForEnter () ;

    lcdPosition (lcdHandle, 0, 1) ; lcdPuts (lcdHandle, "  wiringpi.com  ") ;

  waitForEnter () ;

  lcdCharDef  (lcdHandle, 2, newChar) ;

  lcdClear    (lcdHandle) ;
  lcdPosition (lcdHandle, 0, 0) ;
  lcdPuts     (lcdHandle, "User Char: ") ;
  lcdPutchar  (lcdHandle, 2) ;

  lcdCursor      (lcdHandle, TRUE) ;
  lcdCursorBlink (lcdHandle, TRUE) ;

  waitForEnter () ;

  lcdCursor      (lcdHandle, FALSE) ;
  lcdCursorBlink (lcdHandle, FALSE) ;
  lcdClear       (lcdHandle) ;
*/
/*
    tim = time (NULL) ;
    t = localtime (&tim) ;

    sprintf (buf, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec) ;

    lcdPosition (lcdHandle, (cols - 8) / 2, 1) ;
    lcdPuts     (lcdHandle, buf) ;

    continue ;

    sprintf (buf, "%02d/%02d/%04d", t->tm_mday, t->tm_mon + 1, t->tm_year+1900) ;

    lcdPosition (lcdHandle, (cols - 10) / 2, 2) ;
    lcdPuts     (lcdHandle, buf) ;
*/

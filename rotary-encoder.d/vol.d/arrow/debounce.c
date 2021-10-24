/*
 * Debounce
 */
int  playButtonState,      prevButtonState,      nextButtonState,      infoButtonState,      quitButtonState;
int  lastPlayButtonState,  lastPrevButtonState,  lastNextButtonState,  lastInfoButtonState,  lastQuitButtonState;
long lastPlayDebounceTime, lastPrevDebounceTime, lastNextDebounceTime, lastInfoDebounceTime, lastQuitDebounceTime;
long debounceDelay = 50;

	lastPlayButtonState = lastPrevButtonState = lastNextButtonState = lastInfoButtonState = lastQuitButtonState = HIGH;
	lastPlayDebounceTime = lastPrevDebounceTime = lastNextDebounceTime = lastInfoDebounceTime = lastQuitDebounceTime = 0;

				/* play */
				reading = digitalRead(playButtonPin);
				// check to see if you just pressed the button 
				// (i.e. the input went from HIGH to LOW),  and you've waited 
				// long enough since the last press to ignore any noise:  
				// If the switch changed, due to noise or pressing:
				if (reading != lastPlayButtonState) lastPlayDebounceTime = millis(); // reset the debouncing timer
				if ((millis() - lastPlayDebounceTime) > debounceDelay)
				{
					// whatever the reading is at, it's been there for longer
					// than the debounce delay, so take it as the actual current state:
					// if the button state has changed:
					if (reading != playButtonState)
					{
						playButtonState = reading;
						if (playButtonState == LOW)
						{
							if (cur_song.play_status == PAUSE)
							{
								playMe();
								strcpy(cur_song.SecondRow_text, pause_text);
								lcdPosition(lcdHandle, 0, 1);
								lcdPuts(lcdHandle, lcd_clear);
								scroll_SecondRow_flag = printLcdSecondRow();
							}
							else
							{
								pauseMe();
								strcpy(pause_text, cur_song.SecondRow_text);
								strcpy(cur_song.SecondRow_text, "PAUSED");
								strcpy(cur_song.prevArtist, cur_song.artist);
								lcdPosition(lcdHandle, 0, 1);
								lcdPuts(lcdHandle, lcd_clear);
								scroll_SecondRow_flag = printLcdSecondRow();
							}
						}
					}
				}
				// save the reading. Next time through the loop, it'll be the lastButtonState:
				lastPlayButtonState = reading;
				if (cur_song.play_status != PAUSE)
				{
					/* prev */
					reading = digitalRead(prevButtonPin);
					if (reading != lastPrevButtonState) lastPrevDebounceTime = millis();
					if ((millis() - lastPrevDebounceTime) > debounceDelay)
					{
						if (reading != prevButtonState)
						{
							prevButtonState = reading;
							if (prevButtonState == LOW)
							{
								if (song_index - 1 != 0)
								{
									prevSong();
									song_index--;
								}
							}
						}
					}
					lastPrevButtonState = reading;
					/* next */
					reading = digitalRead(nextButtonPin);
					if (reading != lastNextButtonState) lastNextDebounceTime = millis();
					if ((millis() - lastNextDebounceTime) > debounceDelay)
					{
						if (reading != nextButtonState)
						{
							nextButtonState = reading;
							if (nextButtonState == LOW)
							{
								if (song_index + 1 < num_songs)
								{
									nextSong();
									song_index++;
								}
							}
						}
					}
					lastNextButtonState = reading;
					/* info */
					reading = digitalRead(infoButtonPin);
					if (reading != lastInfoButtonState) lastInfoDebounceTime = millis();
					if ((millis() - lastInfoDebounceTime) > debounceDelay)
					{
						if (reading != infoButtonState)
						{
							infoButtonState = reading;
							if (infoButtonState == LOW)
							{
								strcpy(cur_song.SecondRow_text, (strcmp(cur_song.SecondRow_text, cur_song.artist) == 0 ? cur_song.album : cur_song.artist));
								lcdPosition(lcdHandle, 0, 1);
								lcdPuts(lcdHandle, lcd_clear);
								scroll_SecondRow_flag = printLcdSecondRow();
							}
						}
					}
					lastInfoButtonState = reading;
					/* quit */
					reading = digitalRead(quitButtonPin);
					if (reading != lastQuitButtonState) lastQuitDebounceTime = millis();
					if ((millis() - lastQuitDebounceTime) > debounceDelay)
					{
						if (reading != quitButtonState)
						{
							quitButtonState = reading;
							if (quitButtonState == LOW)
								quitMe();
						}
					}
					lastQuitButtonState = reading;
				}
			}
			scroll_FirstRow_flag = scroll_SecondRow_flag = FALSE;
			pauseScroll_FirstRow_Flag = pauseScroll_SecondRow_Flag = FALSE;

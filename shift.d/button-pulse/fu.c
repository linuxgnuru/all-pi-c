// l to r - on
		for (i = 0; i < bitStop; i++)
		{
			if (i == 1)
			{
		       		digitalWrite(addr + i - 1, HIGH);
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i - 1, HIGH);
		       		digitalWrite(addr + i - 2, HIGH);
			}
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 30)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
				}
				else
				{
					if (i == 30)
					{
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
					}
					if (i == 31)
					{
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
					}
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
			       		digitalWrite(addr + i - 4, HIGH);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
			       		digitalWrite(addr + i - 4, HIGH);
			       		digitalWrite(addr + i - 5, HIGH);
				}
				if (i >= 6 && i < 30)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
			       		digitalWrite(addr + i - 4, HIGH);
			       		digitalWrite(addr + i - 5, HIGH);
			       		digitalWrite(addr + i - 6, HIGH);
				}
				else
				{
					switch (i)
					{
						case 30:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
			       			digitalWrite(addr + 24, HIGH);
						break;
						case 31:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
						break;
						case 32:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
						break;
						case 33:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
						break;
						case 34:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
						break;
					}
				}
			}
		       	digitalWrite(addr + i, HIGH);
			/*
			 * l to r - off
			 */
			delay(delayTime);
			if (i == 1)
			{
		       		digitalWrite(addr + i - 1, LOW);
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i - 1, LOW);
		       		digitalWrite(addr + i - 2, LOW);
			}
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 30)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
				}
				else
				{
					if (i == 30)
					{
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
					}
					if (i == 31)
					{
			       			digitalWrite(addr + 28, LOW);
					}
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
			       		digitalWrite(addr + i - 4, LOW);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
			       		digitalWrite(addr + i - 4, LOW);
			       		digitalWrite(addr + i - 5, LOW);
				}
				if (i >= 6 && i < 30)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
			       		digitalWrite(addr + i - 4, LOW);
			       		digitalWrite(addr + i - 5, LOW);
			       		digitalWrite(addr + i - 6, LOW);
				}
				else
				{
					switch (i)
					{
						case 30:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
			       			digitalWrite(addr + 24, LOW);
						break;
						case 31:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
						break;
						case 32:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
						break;
						case 33:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
						break;
						case 34:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
						break;
					}
				}
			}
		       	digitalWrite(addr + i, LOW);
		}

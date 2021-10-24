// r to l on
		delay(delayTime);
		for (i = bits - 1; i > -1; i--)
		{
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 28)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
				}
				if (i == 28)
				{
			       		digitalWrite(addr + 28, HIGH);
			       		digitalWrite(addr + 29, HIGH);
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
			       		digitalWrite(addr + i + 4, HIGH);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
			       		digitalWrite(addr + i + 4, HIGH);
			       		digitalWrite(addr + i + 5, HIGH);
				}
				if (i >= 6 && i < 24)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
			       		digitalWrite(addr + i + 4, HIGH);
			       		digitalWrite(addr + i + 5, HIGH);
			       		digitalWrite(addr + i + 6, HIGH);
				}
				else
			       	{
					switch (i)
					{
						case 24:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
			       			digitalWrite(addr + 24, HIGH);
						break;
						case 25:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
						break;
						case 26:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
						break;
						case 27:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
						break;
						case 28:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
						break;
					}
				}
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i + 1, HIGH);
		       		digitalWrite(addr + i + 2, HIGH);
			}
			if (i == 1)
			{
				digitalWrite(addr + i + 1, HIGH);
			}
			digitalWrite(addr + i, HIGH);
// r to l off
			delay(delayTime);
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 28)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
				}
				if (i == 28)
				{
			       		digitalWrite(addr + 28, LOW);
			       		digitalWrite(addr + 29, LOW);
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
			       		digitalWrite(addr + i + 4, LOW);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
			       		digitalWrite(addr + i + 4, LOW);
			       		digitalWrite(addr + i + 5, LOW);
				}
				if (i >= 6 && i < 24)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
			       		digitalWrite(addr + i + 4, LOW);
			       		digitalWrite(addr + i + 5, LOW);
			       		digitalWrite(addr + i + 6, LOW);
				}
				else
			       	{
					switch (i)
					{
						case 24:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
			       			digitalWrite(addr + 24, LOW);
						break;
						case 25:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
						break;
						case 26:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
						break;
						case 27:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
						break;
						case 28:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
						break;
					}
				}
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i + 1, LOW);
		       		digitalWrite(addr + i + 2, LOW);
			}
			if (i == 1)
			{
		       		digitalWrite(addr + i + 1, LOW);
			}
			digitalWrite(addr + i, LOW);
		}


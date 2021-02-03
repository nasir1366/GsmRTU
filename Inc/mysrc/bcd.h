#ifndef _BCD_INCLUDED_
#define _BCD_INCLUDED_



/*****************************************************************************
*
*   Function name : CHAR2BCD2
*
*   Returns :       Binary coded decimal value of the input (2 digits)
*
*   Parameters :    Value between (0-99) to be encoded into BCD
*
*   Purpose :       Convert a character into a BCD encoded character.
*                   The input must be in the range 0 to 99.
*                   The result is byte where the high and low nibbles
*                   contain the tens and ones of the input.
*
*****************************************************************************/
unsigned char bin2bcd(unsigned char input)
{
	unsigned char high = 0;
	
	
	while (input >= 10)                 // Count tens
	{
		high++;
		input -= 10;
	}

	return  (high << 4) | input;        // Add ones and return answer
}

/*****************************************************************************
*
*   Function name : CHAR2BCD3
*
*   Returns :       Binary coded decimal value of the input (3 digits)
*
*   Parameters :    Value between (0-255) to be encoded into BCD
*
*   Purpose :       Convert a character into a BCD encoded character.
*                   The input must be in the range 0 to 255.
*                   The result is an integer where the three lowest nibbles
*                   contain the ones, tens and hundreds of the input.
*
*****************************************************************************/
unsigned int CHAR2BCD3(char input)
{
	int high = 0;
	
	while (input >= 100)                // Count hundreds
	{
		high++;
		input -= 100;
	}

	high <<= 4;
	
	while (input >= 10)                 // Count tens
	{
		high++;
		input -= 10;
	}

	return  (high << 4) | input;        // Add ones and return answer
}

unsigned char bcd2bin(unsigned char input)
{
	unsigned char temp;
	temp=(input>>4)*10;
	temp += (input & 0x0F);
	return temp;
}

#endif

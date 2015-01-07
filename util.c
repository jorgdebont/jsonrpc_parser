#include "util.h"

char* custom_itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

int custom_atoi( char *pStr ){
  int iRetVal = 0;
  int iTens = 1;

  if ( pStr ){
    char* pCur = pStr;
    while (*pCur)
      pCur++;

    pCur--;

    while ( pCur >= pStr && *pCur <= '9' && *pCur >= '0' )
    {
      iRetVal += ((*pCur - '0') * iTens);
      pCur--;
      iTens *= 10;
    }
  }
  return iRetVal;
}

int compare_string(char *first, char *second)
{
   while(*first==*second)
   {
      if ( *first == '\0' || *second == '\0' )
         break;

      first++;
      second++;
   }
   if( *first == '\0' && *second == '\0' )
      return TRUE;
   else
      return FALSE;
}

void custom_strcat(char *dest, char *src)
{
	int i, j;
	for(i=0; dest[i]!='\0'; i++){

	}
	//strncpy(dest, src, sizeof(dest)-1);
	//strcpy(dest, src);
	for(j=0; src[j]!='\0'; j++){
		//dest[j+i] = src[j];
	}
}

#include <stdio.h>
#include <stdbool.h>

union   
{  
    int number;  
    char s;  
}test;  
  
bool testBigEndin()  
{  
    test.number=0x01000002;  
    return (test.s==0x01);  
}  
  
void main()  
{  
    if (testBigEndin())    
        printf("big end\n");
    else   
        printf("little end\n");
}  

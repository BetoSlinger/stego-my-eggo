#include <iostream>
#include <windows.h>
#include "BitWiseChar.h"
#include <stdio.h>
using namespace std;
 

BitWiseChar::BitWiseChar(char c)
{
    SetChar(c);
}
 
// Date member function
void BitWiseChar::SetChar(char c)
{
    this->c = c;
    bit0 = c%2;
    c = c/2;
    bit1 = c%2;
    c = c/2;
    bit2 = c%2;
    c = c/2;
    bit3 = c%2;
    c = c/2;
    bit4 = c%2;
    c = c/2;
    bit5 = c%2;
    c = c/2;
    bit6 = c%2;
    c = c/2;
    bit7 = c;
  
 
}

/*int main()
{
    BitWiseChar char1 ('r');
    BitWiseChar char2;
    char2.SetChar('s');
    
    cout << char1.GetChar() << endl;
    cout << char2.GetChar() << endl;
    
    printf("%X",char1.GetByte());
    cout << char2.GetByte() << endl;
    
    cout << char1.GetBit0() << endl;
    cout << char1.GetBit1() << endl;
    cout << char1.GetBit2() << endl;
    cout << char1.GetBit3() << endl;
    cout << char1.GetBit4() << endl;
    cout << char1.GetBit5() << endl;
    cout << char1.GetBit6() << endl;
    cout << char1.GetBit7() << endl;
    
    cout << char2.GetBit0() << endl;
    cout << char2.GetBit1() << endl;
    cout << char2.GetBit2() << endl;
    cout << char2.GetBit3() << endl;
    cout << char2.GetBit4() << endl;
    cout << char2.GetBit5() << endl;
    cout << char2.GetBit6() << endl;
    cout << char2.GetBit7() << endl;
    
    system("pause");
        
    return 0;
}*/

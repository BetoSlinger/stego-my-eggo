#ifndef BITWISECHAR_H
#define BITWISECHAR_H
 
class BitWiseChar
{
private:
    char c;
    DWORD bit0:1;
    DWORD bit1:1;
    DWORD bit2:1;
    DWORD bit3:1;
    DWORD bit4:1;
    DWORD bit5:1;
    DWORD bit6:1;
    DWORD bit7:1;
 
    
 
public:
    BitWiseChar() { }; 
    BitWiseChar(char c);
 
    void SetChar(char c);
    
    int GetBit0() { return bit0; }
    int GetBit1() { return bit1; }
    int GetBit2() { return bit2; }
    int GetBit3() { return bit3; }
    int GetBit4() { return bit4; }
    int GetBit5() { return bit5; }
    int GetBit6() { return bit6; }
    int GetBit7() { return bit7; }
    
    char GetChar() { return c; }
    
    BYTE GetByte() {return (BYTE) c; }
 
    
};
 
#endif

#include "myaescbc.h"

MyAesCBC::MyAesCBC()
{

}

MyAesCBC::MyAesCBC(int keySize, unsigned char *keyBytes)
{
    SetNbNkNr(keySize); // Set the number of key blocks, the number of rounds
    memcpy(key,keyBytes,keySize); //String copy function, copy the keySize bytes of keybytes to the key
    KeyExpansion(); // Key extension, initialization that must be done in advance
}

MyAesCBC::~MyAesCBC()
{

}

DWORD MyAesCBC::OnAesEncrypt(QByteArray InBuffer, DWORD InLength, QByteArray &OutBuffer)
{
    DWORD OutLength = 0; // Encrypted data length
    long blocknum = InLength / 16; // The original data can be divided into blocknum 16-byte data blocks
    long leftnum = InLength % 16; // The last leftnum bytes

    for (long i = 0; i < blocknum; i++)
    {
        QByteArray inbuff = InBuffer.mid(16 * i, 16); // Divide the original data into 16-byte blocks for encryption
        Cipher(inbuff, OutBuffer);
        OutLength += 16;
    }

    if (leftnum) // if there are more leftnum bytes, 16-leftnum bytes will be added after encryption
    {
        QByteArray inbuff = InBuffer.right(leftnum);
        inbuff.resize(16);
        Cipher(inbuff, OutBuffer);
        OutLength += 16;
    }

    // Add 16 bytes to determine the number of bytes added
    int extranum = 16 + (16 - leftnum) % 16;
    QByteArray extrabuff;
    extrabuff.setNum(extranum);
    extrabuff.resize(16);
    Cipher(extrabuff, OutBuffer);
    OutLength += 16;
    return OutLength;
}

DWORD MyAesCBC::OnAesUncrypt(QByteArray InBuffer, DWORD InLength, QByteArray &OutBuffer)
{
    DWORD OutLength = 0;
    long blocknum = InLength / 16;
    long leftnum = InLength % 16;

    if (leftnum)
    {
        return 0;
    }

    for(long i = 0; i < blocknum; i++)
    {
        QByteArray inbuff = InBuffer.mid(16 * i, 16);
        InvCipher(inbuff, OutBuffer);
        OutLength += 16;
    }

    QByteArray extrabuff = OutBuffer.right(16);
    int extranum = extrabuff.toInt();
    OutBuffer.chop(extranum);
    DWORD dwExtraBytes = OutLength - extranum;
    return dwExtraBytes;
}

void MyAesCBC::Cipher(QByteArray input, QByteArray &output)
{
    memset(&State[0][0], 0, 16);
    for (int i = 0; i < 4 * Nb; i++)
    {
        State[i % 4][i / 4] = input[i];
    }

    AddRoundKey(0);

    for (int round = 1; round <= (Nr - 1); round++) // main round loop
    {
        SubBytes();
        ShiftRows();
        MixColumns();
        AddRoundKey(round);
    }

    SubBytes();
    ShiftRows();
    AddRoundKey(Nr);

    // output += state
    for (int i = 0; i < (4 * Nb); i++)
    {
        output.append(State[i % 4][i / 4]);
    }
}

void MyAesCBC::InvCipher(QByteArray input, QByteArray &output)
{
    memset(&State[0][0], 0, 16);
    for (int i = 0; i < (4 * Nb); i++)
    {
        State[i % 4][ i / 4] = input[i];
    }

    AddRoundKey(Nr);

    for (int round = Nr-1; round >= 1; round--) // main round loop
    {
        InvShiftRows();
        InvSubBytes();
        AddRoundKey(round);
        InvMixColumns();
    }

    InvShiftRows();
    InvSubBytes();
    AddRoundKey(0);

    // output += state
    for (int i = 0; i < (4 * Nb); i++)
    {
        output.append(State[i % 4][i / 4]);
    }
}

void MyAesCBC::SetNbNkNr(int keySize)
{
    Nb=4;
    if (keySize == Bits128)
    {
        Nk=4;
        Nr=10;
    }
    else if (keySize == Bits192)
    {
        Nk=6;
        Nr=12;
    }
    else if (keySize == Bits256)
    {
        Nk=8;
        Nr=14;
    }
}

void MyAesCBC::AddRoundKey(int round)
{
    int i,j;                               // k0 k4 k8  k12
    for(j = 0; j < 4; j++)                 // k1 k5 k9  k13
    {                                      // k2 k6 k10 k14
        for(i = 0; i < 4; i++)             // k3 k7 k11 k15
        {
            State[i][j] = (unsigned char)((int)State[i][j] ^ (int)w[4 * ((round * 4) + j) + i]);
        }
    }
}

void MyAesCBC::SubBytes()
{
    //  byte substitute
    int i,j;
    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 4; i++)
        {
            State[i][j] = AesSbox[State[i][j]];
        }
    }
}

void MyAesCBC::InvSubBytes()
{
    int i,j;
    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 4; i++)
        {
            State[i][j] = AesiSbox[State[i][j]];
        }
    }
}

void MyAesCBC::ShiftRows()
{
    unsigned char temp[4 * 4];
    int i,j;
    for (j = 0; j < 4; j++)
    {
        for(i = 0; i < 4; i++)
        {
            temp[4 * i + j] = State[i][j];
        }
    }
    for (i = 1; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (i == 1)
                State[i][j] = temp[4 * i + (j + 1) % 4]; // Shift the first line 1 bit to the left
            else if (i == 2)
                State[i][j] = temp[4 * i + (j + 2) % 4]; // // Shift the second line 2 bits to the left
            else if (i == 3)
                State[i][j] = temp[4 * i + (j + 3) % 4]; // // Shift the third line 3 bits to the left
        }
    }
}

void MyAesCBC::InvShiftRows()
{
    unsigned char temp[4 * 4];
    int i, j;
    for (j = 0; j < 4; j++)
    {
        for(i = 0; i < 4; i++)
        {
            temp[4 * i + j] = State[i][j];
        }
    }
    for (i = 1; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (i == 1)
                State[i][j] = temp[4 * i + (j + 3) % 4];
            else if (i == 2)
                State[i][j] = temp[4 * i + (j + 2) % 4];
            else if (i == 3)
                State[i][j] = temp[4 * i + (j + 1) % 4];
        }
    }
}

void MyAesCBC::MixColumns()
{
    unsigned char temp[4 * 4];
    int i, j;
    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 4; i++)
        {
            temp[4 * i + j] = State[i][j];
        }
    }
    for (j = 0; j < 4; j++)
    {
        State[0][j] = (unsigned char) ( (int)gfmultby02(temp[0 + j]) ^ (int)gfmultby03(temp[4 * 1 + j]) ^
                (int)gfmultby01(temp[4 * 2 + j]) ^ (int)gfmultby01(temp[4 * 3 + j]) );
        State[1][j] = (unsigned char) ( (int)gfmultby01(temp[0 + j]) ^ (int)gfmultby02(temp[4 * 1 + j]) ^
                (int)gfmultby03(temp[4 * 2 + j]) ^ (int)gfmultby01(temp[4 * 3 + j]) );
        State[2][j] = (unsigned char) ( (int)gfmultby01(temp[0 + j]) ^ (int)gfmultby01(temp[4 * 1 + j]) ^
                (int)gfmultby02(temp[4 * 2 + j]) ^ (int)gfmultby03(temp[4 * 3 + j]) );
        State[3][j] = (unsigned char) ( (int)gfmultby03(temp[0 + j]) ^ (int)gfmultby01(temp[4 * 1 + j]) ^
                (int)gfmultby01(temp[4 * 2 + j]) ^ (int)gfmultby02(temp[4 * 3 + j]) );
    }
}

void MyAesCBC::InvMixColumns()
{
    unsigned char temp[4 * 4];
    int i, j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            temp[4 * i + j] = State[i][j];
        }
    }

    for (j = 0; j < 4; j++)
    {
        State[0][j] = (unsigned char) ( (int)gfmultby0e(temp[j]) ^ (int)gfmultby0b(temp[4 + j]) ^
                                        (int)gfmultby0d(temp[4 * 2 + j]) ^ (int)gfmultby09(temp[4 * 3 + j]) );
        State[1][j] = (unsigned char) ( (int)gfmultby09(temp[j]) ^ (int)gfmultby0e(temp[4 + j]) ^
                                        (int)gfmultby0b(temp[4 * 2 + j]) ^ (int)gfmultby0d(temp[4 * 3 + j]) );
        State[2][j] = (unsigned char) ( (int)gfmultby0d(temp[j]) ^ (int)gfmultby09(temp[4 + j]) ^
                                        (int)gfmultby0e(temp[4 * 2 + j]) ^ (int)gfmultby0b(temp[4 * 3 + j]) );
        State[3][j] = (unsigned char) ( (int)gfmultby0b(temp[j]) ^ (int)gfmultby0d(temp[4 + j]) ^
                                        (int)gfmultby09(temp[4 * 2 + j]) ^ (int)gfmultby0e(temp[4 * 3 + j]) );
    }
}

unsigned char MyAesCBC::gfmultby01(unsigned char b)
{
    return b;
}

unsigned char MyAesCBC::gfmultby02(unsigned char b)
{
    if (b < 0x80)
        return (unsigned char)(int)(b <<1);
    else
        return (unsigned char)( (int)(b << 1) ^ (int)(0x1b) );
}

unsigned char MyAesCBC::gfmultby03(unsigned char b)
{
    return (unsigned char) ( (int)gfmultby02(b) ^ (int)b );
}

unsigned char MyAesCBC::gfmultby09(unsigned char b)
{
    return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^ (int)b );
}

unsigned char MyAesCBC::gfmultby0b(unsigned char b)
{
    return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
                            (int)gfmultby02(b) ^ (int)b );
}

unsigned char MyAesCBC::gfmultby0d(unsigned char b)
{
    return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
                            (int)gfmultby02(gfmultby02(b)) ^ (int)(b) );
}

unsigned char MyAesCBC::gfmultby0e(unsigned char b)
{
    return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
                            (int)gfmultby02(gfmultby02(b)) ^(int)gfmultby02(b) );
}

void MyAesCBC::KeyExpansion()
{
    memset(w, 0 , 16 * 15);
    for (int row = 0; row < Nk; row++) // Copy the seed key
    {
        w[4 * row + 0] = key[4 * row];
        w[4 * row + 1] = key[4 * row + 1];
        w[4 * row + 2] = key[4  *row + 2];
        w[4 * row + 3] = key[4 * row + 3];
    }
    byte *temp = new byte[4];
    for (int row = Nk; row < 4 * (Nr + 1); row++)
    {
        temp[0] = w[4 * row - 4]; // The previous column of the current column
        temp[1] = w[4 * row - 3];
        temp[2] = w[4 * row - 2];
        temp[3] = w[4 * row - 1];
        if (row % Nk == 0) // When nk, do special treatment to the previous column of the current column
        {
            temp = SubWord(RotWord(temp));
            temp[0] = (byte)( (int)temp[0] ^ (int) AesRcon[4 * (row / Nk) + 0] );
            temp[1] = (byte)( (int)temp[1] ^ (int) AesRcon[4 * (row / Nk) + 1] );
            temp[2] = (byte)( (int)temp[2] ^ (int) AesRcon[4 * (row / Nk) + 2] );
            temp[3] = (byte)( (int)temp[3] ^ (int) AesRcon[4 * (row / Nk) + 3] );
        }
        else if ( Nk > 6 && (row % Nk == 4) )
        {
            temp = SubWord(temp);
        }


        w[4 * row + 0] = (byte) ( (int) w[4 * (row - Nk) + 0] ^ (int)temp[0] );
        w[4 * row + 1] = (byte) ( (int) w[4 * (row - Nk) + 1] ^ (int)temp[1] );
        w[4 * row + 2] = (byte) ( (int) w[4 * (row - Nk) + 2] ^ (int)temp[2] );
        w[4 * row + 3] = (byte) ( (int) w[4 * (row - Nk) + 3] ^ (int)temp[3] );
    } // for loop
}

unsigned char *MyAesCBC::SubWord(unsigned char *word)
{
    byte* temp = new byte[4];
    for (int j=0;j<4;j++)
    {
        temp[j] = AesSbox[16*(word[j] >> 4)+(word[j] & 0x0f)];
    }
    return temp;
}

unsigned char *MyAesCBC::RotWord(unsigned char *word)
{
    byte *temp = new byte[4];
    temp[0] = word[1];
    temp[1] = word[2];
    temp[2] = word[3];
    temp[3] = word[0];
    return temp;
}

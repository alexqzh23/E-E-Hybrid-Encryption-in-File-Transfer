#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include<string.h>
#include <math.h>
#include<algorithm>
using namespace std;
typedef long long ll;

int gcd(int a, int b)
{
    int c = 0;
    if(a<b) swap(a,b);
    c = b;
    do
    {
        b = c;
        c = a%b;
        a = b;
    }
    while (c != 0);
    return b;
}

int PrimarityTest(int a, int i)
{
    int flag=0;
    for(a;a<i;a++)
    {
        if(i%a==0)
        {
            flag=1;
            break;
        }
    }
    if(flag) return 0;
    return 1;
    // complete this part
}

int ModularExponention(int a, int b, int n)
{
    int y;


    y=1;

    while(b != 0)
    {
        /*For each 1 in b, accumulate y*/

        if(b & 1)
            y = (y*a) % n;

        /*For each bit in b, calculate the square of a*/
        a = (a*a) % n;

        /*Prepare the next digit in b*/
        b = b>>1;
    }

    return y;
    // complete this part
}

void extgcd(ll a,ll b,ll& d,ll& x,ll& y) //Get the result of (1/a) modb
{
    if(!b)
    {
        d=a;
        x=1;
        y=0;
    }
    else
    {
        extgcd(b,a%b,d,y,x);
        y-=x*(a/b);
    }
}

int ModularInverse(int a,int b)  //Get the result of (1/a) modb
{
    ll d,x,y;
    extgcd(a,b,d,x,y);
    return d==1?(x+b)%b:-1;
    // complete this part
}

int *KeyGeneration(int key[])  //Get the public key key
{
    int p, q;
    int phi_n;

    do
    {
        do
            p = rand();
        while (p % 2 == 0);

    }
    while (!PrimarityTest(2, p));

    do
    {
        do
            q = rand();
        while (q % 2 == 0);
    }
    while (!PrimarityTest(2, q));

    int n = p * q;
    phi_n = (p - 1) * (q - 1);

    int e;
    do
        e = rand() % (phi_n - 2) + 2; // 1 < e < phi_n
    while (gcd(e, phi_n) != 1);

    int d = ModularInverse(e,phi_n);

    memset(key,0,3);
    key[0] = e;
    key[1] = d;
    key[2] = n;
    return key;
}

int Encryption(int value, int e, int n)
{
    int cipher;
    cipher = ModularExponention(value, e, n);
    return cipher;
}

int Decryption(int value, int d, int n)
{
    int decipher;
    decipher = ModularExponention(value, d, n);
    return decipher;
}

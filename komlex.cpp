#include "komlex.h"
#include "grafika.h"
#include <SDL/SDL.h>
#include <stdio.h>
#include <math.h>
Komplex::Komplex(float r, float i )
{
    re = r;
    im = i;
}
Komplex::Komplex()
{
    re = 0;
    im = 0;

}
void Komplex::secti(Komplex m,Komplex n)
{
        re = m.re+n.re;
        im =m.im +n.im;

    }
void Komplex::nasob(Komplex m,Komplex n)
{
        re = m.re*n.re;
        im =m.im *n.im;

    }
float Komplex::absolutnihodnota()
{
    return re*re + im*im;


    }
void Komplex::print()
{
    printf("%f + %f i\n", re , im);
}
float Komplex::printah()
{
    return sqrt(re*re + im*im);
    printf("%f\n", sqrt(re*re + im*im));
}
Komplex Komplex::operator+(const Komplex & n)
{
    Komplex vysledek;
    vysledek.re = this->re + n.re;
    vysledek.im = this->im + n.im;

    return vysledek;
}
Komplex Komplex::operator*(const Komplex & n)
{
    Komplex vysledek;
    vysledek.re = this->re * n.re - this->im * n.im;
    vysledek.im = this->im * n.re + this->re * n.im;

    return vysledek;
}
Komplex Komplex::operator-(const Komplex & n)
{
    Komplex vysledek;
    vysledek.re = this->re - n.re;
    vysledek.im = this->im - n.im;

    return vysledek;
}


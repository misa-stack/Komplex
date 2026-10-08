#ifndef KOMLEX_H
#define KOMLEX_H


class Komplex
{
public:
    float re,im;
    Komplex(float r,float i);
    Komplex();
    void print();
    float printah();
    void secti(Komplex m,Komplex n);
    void nasob(Komplex m,Komplex n);
    float absolutnihodnota();
    Komplex operator+(const Komplex & n);
    Komplex operator*(const Komplex & n);
    Komplex operator-(const Komplex & n);


};

#endif // KOMLEX_H

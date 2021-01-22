Assignment 4
Complex nos using arbprecision

1) I first created a static library arbprecision using previous assignment on bignum
2) I created a struct comp which has a real and imag part, both of type bignum
3) I implemented basic operations of 
ADD - add real and complex parts respectively
SUB - subtract real and complex parts respectively
PROD - For (r1,i1) * (r2,i2) , real part = r1*r2 - i1*i2, complex part = r1*i1 + r2*i2
QUOT - (r1,i1) / (r2,i2) = (r1,i1)*(r2,-i2) / (r2*r2 + i2*i2)
ABS - sqrt(r1*r1 + i1*i1)
4) Then plot was made using gnuplot to compare avg time of each operation using libprecision and complex.h

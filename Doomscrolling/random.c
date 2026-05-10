#include "head.h"

unsigned int irr[256];
unsigned int ir1;
unsigned char ind_ran,ig1,ig2,ig3;

/**
 * Samples a random angle from a von Mises distribution.
 */
float vonMises(float past_value, float mu, float kappa) {
    float new_value;
    new_value = past_value+uniformRandomIn(-M_PI/4, +M_PI/4);
    if (new_value > M_PI) {
        new_value -= 2*M_PI;
    }
    if (new_value < -M_PI) {
        new_value += 2*M_PI;
    }

    float cos_subtract = (cosf(new_value-mu) - cosf(past_value-mu));
    if (cos_subtract >= (float) 0 || expf(kappa*cos_subtract) > uniformRandomIn(0, 1)) {
        return(new_value);
    }
    else {
        return(past_value);
    }
}

float uniformRandomIn(double min, double max) {
    return min + (max - min) * Random();
}

float Random(void)
{
    float r;
    ig1=ind_ran-24;
    ig2=ind_ran-55;
    ig3=ind_ran-61;
    irr[ind_ran]=irr[ig1]+irr[ig2];
    ir1=(irr[ind_ran]^irr[ig3]);
    ind_ran++;
    r=ir1*NormRANu;

    return r;
}

void  ini_ran(int SEMILLA)
{
    int INI,FACTOR,SUM,i;
    srand(SEMILLA);
    INI=SEMILLA;
    FACTOR=67397;
    SUM=7364893;

    for(i=0;i<256;i++)
    {
        INI=(INI*FACTOR+SUM);
        irr[i]=INI;
    }

    ind_ran=ig1=ig2=ig3=0;
}
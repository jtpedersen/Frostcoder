#include <math.h>
#include <stdio.h>
 /* gcc gensphere.c -o gensphere -lm  && ./gensphere > sphere.dat */

int main(int argc, char **args) {
    int i,j;
    int cnt = 10;
    for(j = 0; j < cnt ; j++){
        for(i = 0; i < cnt ; i++){
            float d = sqrt((cnt*.5-j)*(cnt*.5-j) + (cnt*.5-i)*(cnt*.5-i));
            float z = cos(d/(double)cnt);
            double x = 5.0 * i;
            double y = 5.0 * j;
            printf("%f %f %f\n", x, y, 15 *z);
        }
    }


    return 0;
}

#ifndef _STATS_H_
#define _STATS_H_


enum Quantities {
    PRIMARY,
    SHADOW,
    SPECULAR,
    TRANSMISSION,
    INTERSECTIONS,
    NUM_QUANTITIES
};


struct Stats {
    int id;
    int pixels;
    float timeSeconds;
    int quantities[NUM_QUANTITIES];

    Stats();
    void print();
};


typedef struct Stats Stats;


#endif

#ifndef SOR_H
#define SOR_H

typedef struct {
    float x;
    float y;
    float z;
} Point;

typedef struct {
    unsigned int ip[3];
} Face;

void SOR_main();

#endif // SOR_H

#ifndef SHOWMODEL_H
#define SHOWMODEL_H

typedef struct {
    float x;
    float y;
    float z;
} Point;

typedef struct {
    unsigned int ip[3];
} Face;

extern Point* mpoint;
extern Face* mface;

void ShowModel_main();

#endif // SHOWMODEL_H

#ifndef ATHENA_MOUSE_H
#define ATHENA_MOUSE_H

#ifdef ATHENA_MOUSE

typedef struct AthenaMouseData {
    int x;
    int y;
    int wheel;
    unsigned int buttons;
} AthenaMouseData;

int athena_mouse_init(void);
int athena_mouse_read(AthenaMouseData *data);
int athena_mouse_set_boundary(int minx, int maxx, int miny, int maxy);
unsigned int athena_mouse_get_mode(void);
unsigned int athena_mouse_set_mode(unsigned int mode);
float athena_mouse_get_accel(void);
int athena_mouse_set_accel(float accel);
int athena_mouse_set_position(int x, int y);

#endif /* ATHENA_MOUSE */

#endif /* ATHENA_MOUSE_H */

#ifndef GRANDO_HEAD_H
#define GRANDO_HEAD_H


int grando_generator(int days,int states,int sites,double delta[],double gamma[][3],double lambda[],double xi[],double tau[],double intensity_mat[][365*1]);
void inten_reader(double e[][365*1], int sites, int readings, char filename[]);


#endif 
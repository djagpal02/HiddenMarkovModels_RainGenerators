#ifndef GRANDO_HEAD_H
#define GRANDO_HEAD_H


int grando_generator(int days,int states,int sites,double delta[],double gamma[][3],double lambda[],double xi[],double tau[],double intensity_mat[][365*5]);
void inten_reader(double e[][25568], int sites, int readings, char filename[]);


#endif 
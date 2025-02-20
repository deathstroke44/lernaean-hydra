//
//  dft.c
//  vaplus C version
//
//  Created by Karima Echihabi on 18/12/2016
//  Copyright 2016 Paris Descartes University. All rights reserved.
//  
//


#include "../config.h"
#include "../globals.h"
#include "../include/dft.h"
#include "math.h"


double real_ephi (double u, double m)
{
  return cos(2*M_PI*u/m);
}

double complex_ephi (double u, double m)
{
  return -sin(2*M_PI*u/m);
}

double complex_mul_real(double r1, double im1, double r2, double im2)
{
    return r1*r2 - im1*im2;
}

double complex_mul_imag(double r1, double im1, double r2, double im2)
{
    return r1*im2 + r2*im1;
}


void fft_from_ts(struct vaplus_index * index, ts_type *ts,  fftwf_complex  *ts_out, ts_type *transform, fftwf_plan plan_forward)
{  
      unsigned long ts_length = index->settings->timeseries_size;
      int transforms_size = index->settings->fft_size;
      int offset = 0;
      offset = index->settings->start_offset;
      
       fftwf_execute ( plan_forward);       	   
	   
       //since normalizing, ignore first coefficent because it is 0., start k =1
	//norm for lower_bounding

       int j = 0;
       //for(int k = 0; k < ts_length/2+1; ++k)
       for(int k = 0; k< transforms_size/2+1; ++k)
       {
		// If it is normalized do not use the first complex co-efficient as it's
		// always 0 for normalized data.
		if(k==0 && index->settings->is_norm) {
			continue;
		}
	     transform[j] = (ts_type) ts_out[k][0];
	     transform[j+1] =  (ts_type) ts_out[k][1];
	     j +=2;
       }
       
      return;
}

/*

  The current transform is pointed to by dft_mem_array
   
*/

void vaplus_from_fft(struct vaplus_index * index, ts_type * cur_transform, unsigned int * cur_approx)
{  
      unsigned long ts_loaded = 0;
      unsigned long ts_length = index->settings->timeseries_size;
      int dft_length = index->settings->fft_size;
      int word_length = index->settings->fft_size;

      int offset = 0;
      offset = index->settings->start_offset;
      	   	   
     unsigned int num_cells;

     for (int k = 0; k < word_length; ++k)
     {
        num_cells = index->cells[k];
	if (cur_transform[k] < index->bins[k][0])
	  {
              cur_approx[k] = 0;
              break;
	  }
	else if (cur_transform[k] >= index->bins[k][num_cells])
	  {
              cur_approx[k] = num_cells-1;
              break;
	  }
        unsigned int c;
        for (c = 0; c < num_cells; c++)
        {
    	  if (cur_transform[k] >= index->bins[k][c] &&
	    cur_transform[k] < index->bins[k][c+1])
	  {
              cur_approx[k] = c;
              break;
	  }
        }             
     }
     
     
}



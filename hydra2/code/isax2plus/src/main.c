//
//  main.c
//  isaxlib
//
//  Created by Kostas Zoumpatianos on 3/12/12.
//  Copyright 2012 University of Trento. All rights reserved.
// 
// ************************************************************
// ******** MAJOR REFACTORING NEEDED FOR CLEANER CODE *********
// ************************************************************
//  IMPORTANT TODO: CLEANUP FLAGS FOR BUFFERS, FBLS, FIX MEMORY 
//  MANAGEMENT TO USE VARIABLE SIZED PAA AND TS REPRESENTATIONS
//  THESE STUFF ARE ALREADY FIXED IN ADAPTIVE ISAX.
// ************************************************************
#include "config.h"
#include "globals.h"

#define PRODUCT "----------------------------------------------\
\nThis is the iSax2+ index.\n\
Copyright (C) 2011-2012 University of Trento.\n\
----------------------------------------------\n\n"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>
#include <time.h>
#include "sax/sax.h"
#include "sax/ts.h"
#include "isax_visualize_index.h"
#include "isax_file_loaders.h"
#ifdef VALUES
#include <values.h>
#endif
		
int main (int argc, char **argv)
{

    INIT_STATS()
    COUNT_TOTAL_TIME_START
      
#ifndef BENCHMARK
    printf(PRODUCT);
    
#if VERBOSE_LEVEL == 0
    printf("Executing in silent mode. Please wait.\n");
#endif
    
#endif
    static char * dataset = "/Users/kostas/Desktop/Repositories/kstzmptn/OnTheFlyIsax/Code/datasets/1416/dataset_big_1416.dat";
    static char * queries = "/Users/kostas/Desktop/Repositories/kstzmptn/OnTheFlyIsax/Code/datasets/1416/dataset_big_1416.dat";
    static char * dataset_hists = "/home/karima/myDisk/data/Cgenerator/data_current_hists.txt";
    
    static char * index_path = "myexperiment/";
    static int dataset_size = 2000;
    static int queries_size = 2000;
    static int time_series_size = 256;
    static int paa_segments = 16;
    static int sax_cardinality = 8;
    static int leaf_size = 10;
    static int initial_lbl_size = 10;
    static unsigned long long flush_limit = 1000;
    static int initial_fbl_size = 100;
    static char use_index = 0;
    static char calc_tlb = 0;    
    static char use_ascii_input = 0;
    static float minimum_distance = FLT_MAX;
    static float epsilon = 0;  //by default perform exact search 
    static float delta = 1;  //by default perform exact search 
    static unsigned int k = 1;
    static char incremental = 0;
    static unsigned int nprobes = 0;
    
    while (1)
    {    
        static struct option long_options[] =  {
            {"ascii-input", no_argument, 0, 'a'},
            {"initial-lbl-size", required_argument, 0, 'b'},
            {"dataset-hists", required_argument, 0, 'n'},	   
	    {"epsilon", required_argument, 0, 'c'},
            {"dataset", required_argument, 0, 'd'},
            {"use-index", no_argument, 0, 'e'},	    
            {"incremental", no_argument, 0, 'q'},
	    {"delta", required_argument, 0, 'h'},
            {"flush-limit", required_argument, 0, 'f'},
            {"calc-tlb", no_argument, 0, 'g'},	    	    
             {"queries", required_argument, 0, 'j'},
            {"k", required_argument, 0, 'k'},
            {"index-path", required_argument, 0, 'p'},
            {"dataset-size", required_argument, 0, 'z'},
            {"queries-size", required_argument, 0, 'm'},
	    {"minimum-distance", required_argument, 0, 's'},
            {"timeseries-size", required_argument, 0, 't'},
            {"sax-cardinality", required_argument, 0, 'x'},
            {"leaf-size", required_argument, 0, 'l'},
            {"nprobes", required_argument, 0, 'o'},
            {"initial-fbl-size", required_argument, 0, 'i'},
            {"help", no_argument, 0, '?'},
        };
        
        /* getopt_long stores the option index here. */
        int option_index = 0;
        
        int c = getopt_long (argc, argv, "",
                             long_options, &option_index);
        if (c == -1)
            break;
        switch (c)
        {
            case 'j':
                queries = optarg;
                break;
				
            case 's':
                 minimum_distance = atof(optarg);
                 break;                

            case 'm':
                queries_size = atoi(optarg);
                break;

  	    case 'n':
	        dataset_hists = optarg;
	        break;	       

	case 'k':
                k = atoi(optarg);
                if (k < 1)
		{
		  fprintf(stderr,"Please change the k to be greater than 0.\n");
		  exit(-1);
		}
                break;		
                
            case 'd':
                dataset = optarg;
                break;
                
            case 'p':
                index_path = optarg;
                break;
                
            case 'z':
                dataset_size = atoi(optarg);
                break;
                
            case 't':
                time_series_size = atoi(optarg);
                break;
                
            case 'x':
                sax_cardinality = atoi(optarg);
                break;
                
            case 'l':
                leaf_size = atoi(optarg);
                break;
                
            case 'b':
                initial_lbl_size = atoi(optarg);
                break;
                
            case 'f':
                flush_limit = atoi(optarg);
                break;

            case 'g':
                calc_tlb = 1;
                break;		
                
            case 'i':
                initial_fbl_size = atoi(optarg);
                break;
            case '?':
#ifdef BENCHMARK
                printf(PRODUCT);
#endif
                printf("Usage:\n\
                       \t--dataset XX \t\t\tThe path to the dataset file\n\
                       \t--queries XX \t\t\tThe path to the queries file\n\
                       \t--dataset-size XX \t\tThe number of time series to load\n\
                       \t--queries-size XX \t\tThe number of queries to do\n\
                       \t--minimum-distance XX\t\tThe minimum distance we search (MAX if not set)\n\
                       \t--ascii-input  \t\t\tSpecifies that ascii input files will be used\n\
                       \t--index-path XX \t\tThe path of the output folder\n\
                       \t--timeseries-size XX\t\tThe size of each time series\n\
                       \t--sax-cardinality XX\t\tThe maximum sax cardinality in number of bits (power of two).\n\
                       \t--leaf-size XX\t\t\tThe maximum size of each leaf\n\
                       \t--initial-lbl-size XX\t\tThe initial lbl buffer size for each buffer.\n\
                       \t--flush-limit XX\t\tThe limit of time series in memory at the same time\n\
                       \t--initial-fbl-size XX\t\tThe initial fbl buffer size for each buffer.\n\
                       \t--use-index  \t\t\tSpecifies that an input index will be used\n\
                       \t--help\n\n");
                return 0;
                break;
            case 'a':
                use_ascii_input = 1;
                break;
            case 'e':
                use_index = 1;
                break;
            case 'q':
  	        incremental = 1;
                break;				
            case 'o':
                nprobes = atoi(optarg);
                break;		
            case 'c':
                epsilon = atof(optarg);
                break;		
	    case 'h':
                delta = atof(optarg);
                break;		
            default:
                exit(-1);
                break;
        }
    }

    isax_index *index;
    if (use_index)
    {
      if(calc_tlb)
      {
	  index = index_read(index_path);
	  if (index == NULL) 
	    { 
	      fprintf(stderr, "Error main.c:  Could not read the index from disk.\n");
	      return -1;              
	    }
	  isax_tlb_binary_file(queries, queries_size, index);		
      }
      else
      {
	  RESET_PARTIAL_COUNTERS()
	  COUNT_PARTIAL_TIME_START
	  index = index_read(index_path);
	  if (index == NULL) 
	    { 
	      fprintf(stderr, "Error main.c:  Could not read the index from disk.\n");
	      return -1;              
	    }
	  COUNT_PARTIAL_TIME_END	
	    index->stats->idx_reading_total_time  = partial_time;	
	  index->stats->idx_reading_input_time  = partial_input_time;
	  index->stats->idx_reading_output_time = partial_output_time;
	  index->stats->idx_reading_cpu_time    = partial_time
	    - partial_input_time
	    - partial_output_time;
	  index->stats->idx_reading_seq_input_count   = partial_seq_input_count;
	  index->stats->idx_reading_seq_output_count  = partial_seq_output_count;
	  index->stats->idx_reading_rand_input_count  = partial_rand_input_count;
	  index->stats->idx_reading_rand_output_count = partial_rand_output_count;

	  get_index_stats (index);
	  print_index_stats(index,dataset);

	  //calculate r_delta
	  ts_type r_delta = FLT_MAX;
	  
	  if (delta > 0 && delta < 1)
	   {
	     FILE *fp;
	     int bins;
	     double *x, *y;
	     int i,j;
	     if((fp=fopen(dataset_hists, "r"))!=NULL)
	       {
		 fscanf(fp, "%d", &bins);
		 
		 x = (double *) calloc(bins, sizeof(double));
		 y = (double *) calloc(bins, sizeof(double));
		 
		 for(int j=0; j<bins; j++)
		   fscanf(fp, "%lf%lf", &x[j], &y[j]);
		 fclose(fp);		
	       }
	     else printf("Error opening data distribution file.\n");

	     for (j=0; (j<bins)&&(y[j]<delta); j++);
	     j--;
	     fprintf(stderr,"Using histogram bin %lf.\n", y[j]);
	     
	     ts_type r_delta=x[j]+(((delta-y[j])*(x[j+1]-x[j]))/(y[j+1]-y[j]));
	     fprintf(stderr,"Using r_delta = %lf.\n", r_delta);
	   }
	  isax_knn_query_binary_file(queries, queries_size, index,
				     minimum_distance, epsilon, r_delta,k,nprobes,incremental);
      }
    }
    else
    {

        RESET_PARTIAL_COUNTERS()
        COUNT_PARTIAL_TIME_START
     
        isax_index_settings * index_settings = isax_index_settings_init(index_path,         // INDEX DIRECTORY
                                                                    time_series_size,   // TIME SERIES SIZE
                                                                    paa_segments,       // PAA SEGMENTS
                                                                    sax_cardinality,    // SAX CARDINALITY IN BITS
                                                                    leaf_size,          // LEAF SIZE
                                                                    initial_lbl_size,   // INITIAL LEAF BUFFER SIZE
                                                                    flush_limit,        // FLUSH LIMIT
								    initial_fbl_size,
								    1);  // INITIAL FBL BUFFER SIZE
        index = isax_index_init(index_settings);

        isax_index_binary_file(dataset, dataset_size, index);

        int already_finalized = 0;
        isax_index_finalize(index, NULL, &already_finalized);	
	COUNT_PARTIAL_TIME_END
	index->stats->idx_building_total_time  = partial_time;	
	index->stats->idx_building_input_time  = partial_input_time;
	index->stats->idx_building_output_time = partial_output_time;
	index->stats->idx_building_cpu_time    = partial_time
	                                        - partial_input_time
	                                        - partial_output_time;
	index->stats->idx_building_seq_input_count   = partial_seq_input_count;
	index->stats->idx_building_seq_output_count  = partial_seq_output_count;
	index->stats->idx_building_rand_input_count  = partial_rand_input_count;
	index->stats->idx_building_rand_output_count = partial_rand_output_count;	    
        RESET_PARTIAL_COUNTERS()
        COUNT_PARTIAL_TIME_START
	index_write(index);
        COUNT_PARTIAL_TIME_END
	index->stats->idx_writing_total_time  = partial_time;	
	index->stats->idx_writing_input_time  = partial_input_time;
	index->stats->idx_writing_output_time = partial_output_time;
	index->stats->idx_writing_cpu_time    = partial_time
	                                          - partial_input_time
	                                          - partial_output_time;
	index->stats->idx_writing_seq_input_count   = partial_seq_input_count;
	index->stats->idx_writing_seq_output_count  = partial_seq_output_count;
	index->stats->idx_writing_rand_input_count  = partial_rand_input_count;
	index->stats->idx_writing_rand_output_count = partial_rand_output_count;	    

	//isax_index_collect_stats(index, NULL);
        get_index_stats(index);
	
	print_index_stats(index,dataset);        


	
        float distance = 0;
        //PRINT_STATS(distance)
        //isax_query_binary_file(queries, queries_size, index, minimum_distance);
        //printf("Finalized: %d, %d\n", already_finalized, already_finalized * sizeof(sax_type) * index->settings->paa_segments);        
        //visualize_index(index);
    }

    /*
    
    if (use_ascii_input) {
        isax_index_ascii_file(dataset, dataset_size, DELIMITER, index);
        int already_finalized = 0;
        COUNT_TOTAL_TIME_START
        isax_index_finalize(index, NULL, &already_finalized);
        COUNT_TOTAL_TIME_END
	COUNT_PARTIAL_TIME_END
        float distance = 0;
        //PRINT_STATS(distance)
        isax_query_ascii_file(queries, queries_size, DELIMITER, index, minimum_distance);
        //printf("Finalized: %d, %d\n", already_finalized, already_finalized * sizeof(sax_type) * index->settings->paa_segments);
        
        //visualize_index(index);
    }
    else 
    {
    }
    */
    
    //isax_index_serialize_structure(index, "myexperiment.tpl");
    //free(index_settings->bit_masks);
    //free(index_settings);	
    

    COUNT_TOTAL_TIME_END
    isax_index_destroy(index, use_index, NULL);
    free(index->settings->bit_masks);

    if (use_index)
    {
      free(index->stats->leaves_heights);
      free(index->stats->leaves_sizes);
    }
    
    free(index->stats);
    free(index->settings->max_sax_cardinalities);    
    free(index->settings);
    
    free(index);	
    
    printf("\n");
    return 0;
}

//
//  main.c
//  MASS C version

//  Created by Karima Echihabi on 03/11/2017
//  Copyright 2017 Paris Descartes University. All rights reserved.
//
//  Based on code by Michele Linardi on 01/01/2016
//  Copyright 2016 Paris Descartes University. All rights reserved.
//


#include "../config.h"
#include "../globals.h"


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>
#include <time.h>
#include <time.h>
#include "../include/ts.h"
#include "../include/mass_file_loaders.h"
#include "../include/mass_instance.h"
#include "../include/mass_buffer_manager.h"

#ifdef VALUES
#include <values.h>
#endif
		
int main (int argc, char **argv)
{
    INIT_STATS()
    COUNT_TOTAL_TIME_START
    RESET_PARTIAL_COUNTERS()
    COUNT_PARTIAL_TIME_START

    static char * dataset = "data_current.txt";
    static char * queries = "query_current.txt";
    
    static char * index_path = "out/";
    static unsigned long long dataset_size = 1000;
    static unsigned int queries_count = 5;
    static unsigned long long time_series_size = 256;    
    static unsigned int init_segments = 1;  
    static unsigned long long query_size = 10;
    static double buffered_memory_size = 6439.2; 
    static int  use_ascii_input = 0;
    static int mode = 0;
    static float minimum_distance = FLT_MAX;
    struct mass_instance * mass_inst = NULL;
    boolean is_db_new = 1;
    
    while (1)
    {    
        static struct option long_options[] =  {
            {"dataset", required_argument, 0, 'd'},
            {"ascii-input", required_argument, 0, 'a'},
            {"queries", required_argument, 0, 'q'},
            {"index-path", required_argument, 0, 'p'},
            {"dataset-size", required_argument, 0, 'z'},
            {"queries-count", required_argument, 0, 'k'},
            {"mode", required_argument, 0, 'x'},
            {"buffer-size", required_argument, 0, 'b'},	    
	    {"minimum-distance", required_argument, 0, 's'},
            {"timeseries-size", required_argument, 0, 't'},
            {"query-size", required_argument, 0, 'l'},
            {"help", no_argument, 0, '?'}
        };
        
        /* getopt_long stores the option index here. */
        int option_index = 0;
        
        int c = getopt_long (argc, argv, "",
                             long_options, &option_index);
        if (c == -1)
            break;
        switch (c)
        {
            case 'q':
                queries = optarg;
                break;
						
            case 's':
                 minimum_distance = atof(optarg);
                 break;

	    case 'b':
                 buffered_memory_size = atof(optarg);
                 break;
		 
            case 'k':
                queries_count = atoi(optarg);
                if (queries_count < 1)
		{
		  fprintf(stderr,"Please change the queries size to be greater than 0.\n");
		  exit(-1);
		}
                break;
		
            case 'd':
                dataset = optarg;
                break;
		
            case 'p':
                index_path = optarg;
                break;
		
            case 'x':
                mode = atoi(optarg);
                break;
		
            case 'z':
                dataset_size = atoi(optarg);
                if (dataset_size < 1)
		{
		  fprintf(stderr,"Please change the dataset size to be greater than 0.\n");
		  exit(-1);
		}
                break;
		
            case 't':
                time_series_size = atoll(optarg);
                break;
		
            case 'l':
                query_size = atoll(optarg);
                if (query_size < 1)
		{
		  fprintf(stderr,"Please change the query size to be at least 1.\n");
		  exit(-1);
		}
                break;
		
            case '?':
                printf("Usage:\n\
                       \t--dataset XX \t\t\tThe path to the dataset file\n\
                       \t--queries XX \t\t\tThe path to the queries file\n\
                       \t--dataset-size XX \t\tThe number of time series to load\n\
                       \t--queries-count XX \t\tThe number of queries to do\n\
                       \t--mode: 0=index, 1=query, 2=index & query  \t\t1\n\
                       \t--index-path XX \t\tThe path of the output folder\n\
                       \t--buffer-size XX \t\tThe size of the buffer memory in MB\n\
                       \t--timeseries-size XX\t\tThe size of each time series\n\
                       \t--ascii-input 0 or 1 \t\t\tSpecifies that ascii input files will be used\n\
                       \t--query-size XX\t\t\tThe size of each query\n\
                       \t--help\n\n\
                       \t--**********************EXAMPLES**********************\n\n\
                       \t--*********************INDEX MODE*********************\n\n\
                       \t--./mass --dataset XX --dataset-size XX             \n\n\
                       \t--          --output-path XX --timeseries-size XX --mode 0\n\n\
                       \t--*********************QUERY MODE*********************\n\n\
                       \t--./mass --queries XX --queries-size XX             \n\n\
                       \t--           --output-path XX --mode 1                 \n\n\
                       \t--*****************INDEX AND QUERY MODE***************\n\n\
                       \t--./mass --dataset XX --dataset-size XX             \n\n\
                       \t--          --timeseries-size XX --output-path XX      \n\n\
                       \t--           --queries XX --queries-size XX --mode 2  \n\n\
                       \t--****************************************************\n\n");

                return 0;
                break;
            case 'a':
  	        use_ascii_input = atoi(optarg);
                break;
            default:
                exit(-1);
                break;
        }
    }

    minimum_distance = FLT_MAX;
        struct mass_settings * settings = mass_settings_init(
							     dataset,
							     queries,
                                                             time_series_size,
							     dataset_size,
							     query_size,
							     buffered_memory_size,
							     is_db_new);  
       if (settings == NULL)
       { 
         fprintf(stderr, "Error main.c:  Could not initialize the index settings.\n");
         return -1;              
       }
    
       mass_inst = mass_inst_init(settings);
       
       if (mass_inst == NULL)
       { 
          fprintf(stderr, "Error main.c:  Could not initialize mass.\n");
          return -1;              
       }
    
    if (mode == 0)  //only compute the ffts for the candidates and store them in disk
    {
	  if (!mass_new_binary_file(queries, query_size, queries_count, mass_inst, minimum_distance))
       { 
          fprintf(stderr, "Error main.c:  Could not create mass.\n");
          return -1;              
       }
    }
    else
    {
      fprintf(stderr, "Please use a valid mode. run dstree --help for more information. \n");
      return -1;              
    }
      
    COUNT_TOTAL_TIME_END
    fprintf(stderr,"Sanity check: Total time should be less than: %f secs \n",
      	   total_time/1000000);

    mass_inst_destroy(mass_inst, is_db_new);


    malloc_stats_print(NULL, NULL, NULL);    
    return 0;
}


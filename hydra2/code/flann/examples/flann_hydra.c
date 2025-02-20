#include <flann/flann.h>

#include <stdio.h>
#include <stdlib.h>


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>
#include <time.h>
//#include "ts.h"
//#include "srs_file_loaders.h"

//#include <iostream>
//#include <fstream>
#include <stdio.h>
#include <getopt.h>
#include <dirent.h>
#include <float.h>
//#include <vector>
#include <time.h>
#include <sys/time.h>
#include  "../src/cpp/flann/stats.h"
//#include <jemalloc/jemalloc.h>

struct stats_info  stats;


float* read_points(const char* filename, int rows, int cols)
{
	float* data;
	float *p;
	FILE* fin;
	int i,j;

    fin = fopen(filename,"r");
    if (!fin) {
        printf("Cannot open input file.\n");
        exit(1);
    }
    
    data = (float*) malloc(rows*cols*sizeof(float));
    if (!data) {
        printf("Cannot allocate memory.\n");
        exit(1);
    }
    p = data;
    
    for (i=0;i<rows;++i) {
        for (j=0;j<cols;++j) {
            fscanf(fin,"%g ",p);
            p++;
        }
    }
    
    fclose(fin);
    
    return data;
}

void read_points_binary_new(const char* filename, int rows, int cols, float ** data)
{
	FILE* fin;
	unsigned long long i,j;
    float ts = -1;

    fin = fopen(filename,"rb");
    if (!fin) {
      printf("Cannot open input file %s.\n", filename);
        exit(1);
    }
    
    
    for (i = 0; i < rows; ++i) {
     for(j=0; j<cols; ++j)
	  {
	    fread(&ts, sizeof(float), 1, fin);
		data [i][j] = ts;
	    //printf("p[%llu][%llu] = %g\n", i,j,data[i*cols+j]);
	  }
		//printf("p[%llu][0] = %g\n", i,j,data[i][0]);
    }
    fclose(fin);

}

void read_points_binary(const char* filename, int rows, int cols, float * data)
{
	float *p;
	FILE* fin;
	int i,j;

    fin = fopen(filename,"rb");
    if (!fin) {
      printf("Cannot open input file %s.\n", filename);
        exit(1);
    }
    

    // KOSTAS CHANGE:
    /*p = data;
    for (i=0;i<rows;++i) {
      fread(p, sizeof(float), cols, fin);
      //printf("p[%d][0] = %g\n", i,p[0]);
      p += cols;
    }*/
    for(j=0; j<rows; j++) {
      int res = fread(&(data[j*cols]), sizeof(float)*cols, 1, fin);
      if(!res) {
        printf("CANNOT READ DATA!\n"); exit(-1);
      }
    }
    
    fclose(fin);

}
/*
float* read_points_binary(const char* filename, int rows, int cols)
{
	float* data;
	float *p;
	FILE* fin;
	int i,j;

    fin = fopen(filename,"rb");
    if (!fin) {
      printf("Cannot open input file %s.\n", filename);
        exit(1);
    }
    
    data = (float*) malloc(rows*cols*sizeof(float));
    if (!data) {
        printf("Cannot allocate memory.\n");
        exit(1);
    }
    p = data;
    
    for (i=0;i<rows;++i) {
      fread(p, sizeof(float), cols, fin);
      //printf("p[%d][0] = %g\n", i,p[0]);
        p += cols;
    }
    
    fclose(fin);
    
    return data;
}
*/
void write_results(const char* filename, int *data, int rows, int cols)
{
	FILE* fout;
	int* p;
	int i,j;

    fout = fopen(filename,"w");
    if (!fout) {
        printf("Cannot open output file.\n");
        exit(1);
    }
    
    p = data;
    for (i=0;i<rows;++i) {
        for (j=0;j<cols;++j) {
            fprintf(fout,"%d ",*p);
            p++;
        }
        fprintf(fout,"\n");
    }
    fclose(fout);
}

		
int main (int argc, char **argv)
{


    stats_init();
    //INIT_STATS()

    stats_count_total_time_start();


    static char * dataset = "/home/karima/myDisk/data/Cgenerator/data_current.txt";
    static char * queries = "/home/karima/myDisk/data/Cgenerator/query_current.txt";
    
    static char * index_path = "out/";
    static unsigned int dataset_size = 1000;
    static unsigned int queries_size = 5;
    static unsigned int ts_length = 256;
    static unsigned int num_checks  = 16;    
    static unsigned int num_trees = 1;  
    static double buffered_memory_size = 6439.2; 
    static int  use_ascii_input = 0;
    static int mode = 0;
    static float minimum_distance = FLT_MAX;
    static float epsilon = 0; //perform exact search by default
    static float delta = 1;    //perform exact search by default
    struct srs_index * index = NULL;
    boolean is_index_new = 1;
    int k = 1; //k neighbors in k-NN search
 	   
	float* dataset_data;
	//float** dataset_data;
	//char* dataset_data;
	float* queries_data;
	int* result;
	float* dists;
	struct FLANNParameters p;
	float speedup;
	flann_index_t index_id;

    
    while (1)
    {    
        static struct option long_options[] =  {
            {"ascii-input", required_argument, 0, 'a'},
            {"buffer-size", required_argument, 0, 'b'},
            {"num-checks", required_argument, 0, 'c'},	    
            {"dataset", required_argument, 0, 'd'},
            {"epsilon", required_argument, 0, 'e'},
            {"delta", required_argument, 0, 'f'},	    
            {"k", required_argument, 0, 'k'},
            {"queries", required_argument, 0, 'q'},
            {"index-path", required_argument, 0, 'p'},
            {"dataset-size", required_argument, 0, 'z'},
            {"queries-size", required_argument, 0, 'g'},
            {"mode", required_argument, 0, 'x'},
	        {"minimum-distance", required_argument, 0, 's'},
            {"timeseries-size", required_argument, 0, 't'},
            {"num-trees", required_argument, 0, 'l'},
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

	    case 'e':
                 epsilon = atof(optarg);
                 break;

	    case 'f':
                 delta = atof(optarg);
                 break;
		 
            case 'k':
                k = atoi(optarg);
                if (k < 1)
		{
		  fprintf(stderr,"Please change k to be greater than 1.\n");
		  exit(-1);
		}
                break;		 
            case 'g':
                queries_size = atoi(optarg);
                if (queries_size < 1)
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
                ts_length = atoi(optarg);
                break;

            case 'c':
                num_checks = atoi(optarg);
                break;		
		
            case 'l':
                num_trees = atoi(optarg);
                break;
		
            case '?':
                printf("Usage:\n\
                       \t--Queries and Datasets should be single precision\n\
                       \t--floating points. They can be binary of ascii.\n\
                       \t--However performance is faster with binary files\n\
                       \t--dataset XX \t\t\tThe path to the dataset file\n\
                       \t--queries XX \t\t\tThe path to the queries file\n\
                       \t--dataset-size XX \t\tThe number of time series to load\n\
                       \t--k XX \t\tThe number of nearest neighbors to return\n\
                       \t--queries-size XX \t\tThe number of queries to run\n\
                       \t--mode: 0=index, 1=query, 2=index & query  3=calc_tlb\t\t\n\
                       \t--index-path XX \t\tThe path of the output folder\n\
                       \t--buffer-size XX \t\tThe size of the buffer memory in MB\n\
                       \t--timeseries-size XX\t\tThe size of each time series\n\
                       \t--ascii-input X \t\t\0 for ascii files and 1 for binary files\n\
                       \t--leaf-size XX\t\t\tThe maximum size of each leaf\n\
                       \t--help\n\n\
                       \t--**********************EXAMPLES**********************\n\n\
                       \t--*********************INDEX MODE*********************\n\n\
                       \t--bin/srs --dataset XX --dataset-size XX             \n\n\
                       \t--          --index-path XX --timeseries-size XX --mode 0\n\n\
                       \t--*********************QUERY MODE*********************\n\n\
                       \t--bin/srs --queries XX --queries-size XX             \n\n\
                       \t--           --index-path XX --mode 1                 \n\n\
                       \t--*****************INDEX AND QUERY MODE***************\n\n\
                       \t--bin/srs --dataset XX --dataset-size XX             \n\n\
                       \t--          --timeseries-size XX --index-path XX      \n\n\
                       \t--           --queries XX --queries-size XX --mode 2  \n\n\
                       \t--****************************************************\n\n");
		    printf("\n");
  
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

    char *index_full_filename = (char *) malloc(sizeof(char) * (strlen(index_path) + 10));
    index_full_filename = strcpy(index_full_filename, index_path);
    index_full_filename = strcat(index_full_filename, "index.bin");	
    
    if (mode == 0)  //only build and store the index
    {

      stats_reset_partial_counters(); 
      stats_count_partial_time_start();
      printf("Reading input data file.\n");
      
      // KOSTAS TEST
      size_t memsz = dataset_size*ts_length*sizeof(float);
      printf("I would have allocated: (%d x %d x %u) ?=? %u\n", dataset_size, ts_length, sizeof(float), memsz);
      
      // KOSTAS CHANGE
      dataset_data =(float*) calloc(dataset_size, ts_length * sizeof(float));
	  /* dataset_data =(float**) malloc(dataset_size * sizeof(float*));
      for (unsigned long i = 0; i< dataset_size; ++i)
      {
        dataset_data[i] = (float *)malloc(sizeof(float) * ts_length);
     }
      */
      if (!dataset_data) {
        printf("Cannot allocate memory.\n");
        exit(1);
      }
      read_points_binary(dataset, dataset_size, ts_length,dataset_data);
      //read_points_binary_new(dataset, dataset_size, ts_length,dataset_data);
      
      printf("read data successfully\n");
    
      p = DEFAULT_FLANN_PARAMETERS;
      p.algorithm = FLANN_INDEX_KDTREE;
      p.trees = num_trees;
      p.log_level = FLANN_LOG_INFO;
      p.checks =num_checks;

	
      printf("Computing index.\n");
      index_id = flann_build_index(dataset_data,dataset_size,ts_length, &speedup, &p);
      printf("index_full_filename=%s\n", index_full_filename); 
      flann_save_index(index_id, index_full_filename);
      stats_count_partial_time_end();
      stats_update_idx_building_stats();
      get_index_stats (&stats, index_full_filename);
      print_index_stats(stats, (char *)dataset, (unsigned int)1);
       
      //result = (int*) malloc(queries_size*k*sizeof(int));
      //dists = (float*) malloc(queries_size*k*sizeof(float));
      
      //flann_find_nearest_neighbors_index(index_id, queries_data, queries_size, result, dists, k, &p);      
      //write_results("results.dat",result, queries_size, k);
	
      flann_free_index(index_id, &p);
      /*
      for (unsigned long i = 0; i< dataset_size; ++i)
      {
        free(dataset_data[i]);
	  }
	  */
      free(dataset_data);
      //free(queries_data);
      //free(result);
      //free(dists);
      
    }
    /*
      SRS_In_Memory<float> * searcher = new SRS_In_Memory<float>(index_path);
      searcher->restore_index ();
      //delta = p_thres, c= epsilon + 1
      //t = max data points to look at, set it to dataset_size
      query_workload(searcher,
      k,
      dataset_size,
      cal_thres((epsilon+1),delta, projected_dim),  
      queries,
      ts_length,
      queries_size);

      stats_count_partial_time_end();
      stats_reset_partial_counters();

      delete searcher;
      }
      */
    else if (mode == 1)  //read an existing index and execute queries
      {
	
	p = DEFAULT_FLANN_PARAMETERS;
	p.algorithm = FLANN_INDEX_KDTREE;
	p.trees = num_trees;
	p.log_level = FLANN_LOG_INFO;
	p.checks =num_checks;
 	p.sorted = true;

	// KOSTAS CHANGE
        dataset_data =(float*) calloc(dataset_size, ts_length*sizeof(float));
	if (!dataset_data) {
	  printf("Cannot allocate memory.\n");
	  exit(1);
	}
	read_points_binary(dataset, dataset_size, ts_length, dataset_data);
	
	index_id =  flann_load_index(index_full_filename, dataset_data, dataset_size, ts_length);
	if (index_id == NULL) 
	  { 
	    fprintf(stderr, "Error main.c:  Could not read the index from disk.\n");
	    return -1;              
	  }
	      
	fprintf(stderr, ">>> Index loaded successfully from: %s\n", index_path);
	result = (int*) malloc(k*sizeof(int));
	dists = (float*) malloc(k*sizeof(float));

	stats_reset_partial_counters();
	stats_count_partial_time_start();
	stats_count_partial_input_time_start();
	FILE *ifile = fopen(queries, "rb");
	stats_count_partial_input_time_end();
	
	if (ifile == NULL) {
	  fprintf(stderr, "File %s not found!\n", queries);
	  exit(-1);
	}
	
    
	//queries_data = read_points_binary(queries, queries_size, ts_length);

	queries_data = (float*) malloc(ts_length*sizeof(float));
	
	unsigned int q_loaded = 0;
	
	while (q_loaded < queries_size)
	  {      
	    q_loaded++;
	    stats_reset_query_counters();
	  
	    stats_count_partial_seq_input(1);
	    stats_count_partial_input_time_start();
	    fread(queries_data, sizeof(float), ts_length, ifile);
            printf("%d\n", q_loaded);
	    stats_count_partial_input_time_end();
	      
	    flann_find_nearest_neighbors_index(index_id, queries_data, 1, result, dists, k, &p);
	    for (unsigned int pos = 1; pos <= k; ++pos)
	      {
		stats_count_partial_time_end();	
		stats_update_query_stats(sqrtf(dists[pos-1]), -1, result[pos-1]);
		get_query_stats(&stats);
		print_query_stats(stats, queries, q_loaded,pos);
		write_results("results.dat",result, 1, k);	    		
		stats_reset_query_counters();
		stats_reset_partial_counters();	
		stats_count_partial_time_start();
	      }

	  }
	
	stats_count_partial_time_end();
	stats_reset_partial_counters();
	
	

    
	if(fclose(ifile))
	  {   
	    fprintf(stderr, "Error in srs_file_loaders.c: Could not close the query filename %s", queries);
	    exit (0); 
	  }

	flann_free_index(index_id, &p);	
	free(result);
	free(dists);
	free(dataset_data);
	free(queries_data);	
      }

    else
      {
	fprintf(stderr, "Please use a valid mode. run srs --help for more information. \n");
	return -1;              
      }

    
    stats_count_total_time_end();
    fprintf(stderr,"Sanity check: combined indexing and querying times should be less than: %f secs \n",
	    stats.counters.total_time/1000000);
    

    free(index_full_filename);
    printf("\n");

   // malloc_stats_print(NULL, NULL, NULL);    
    return 0;
}

/*
  int main(int argc, char** argv)
  {
  float* dataset;
  float* testset;
  int nn;
  int* result;
  float* dists;
  struct FLANNParameters p;
  float speedup;
  flann_index_t index_id;

  int rows = 10000;
  int cols = 128;
  int tcount = 100; //#queries

  printf("Reading input data file.\n");
  dataset = read_points_binary("/home/karimae/projects/dsseval_full_compact_journal/code/flann-proj/data/siftsmall_base.bin", rows, cols);
  printf("Reading test data file.\n");
  testset = read_points_binary("/home/karimae/projects/dsseval_full_compact_journal/code/flann-proj/data/siftsmall_base.bin", tcount, cols);
    
    nn = 11;
    result = (int*) malloc(tcount*nn*sizeof(int));
    dists = (float*) malloc(tcount*nn*sizeof(float));
    
    p = DEFAULT_FLANN_PARAMETERS;
    p.algorithm = FLANN_INDEX_KDTREE;
    p.trees = 8;
    p.log_level = FLANN_LOG_INFO;
	p.checks = 64;
    
    printf("Computing index.\n");
    index_id = flann_build_index(dataset, rows, cols, &speedup, &p);
    flann_find_nearest_neighbors_index(index_id, testset, tcount, result, dists, nn, &p);
    
    write_results("results.dat",result, tcount, nn);
    
    flann_free_index(index_id, &p);
    free(dataset);
    free(testset);
    free(result);
    free(dists);
    
    return 0;
}
*/

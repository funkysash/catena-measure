#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <limits.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define STARTGARLIC 15
#define H_PER_KIB 	16	//1024/H_LEN

//Limits
#define GARLIC_HIGH 64
#define GARLIC_LOW 1
#define LAMBDA_HIGH 255
#define LAMBDA_LOW 1
#define ITER_HIGH INT_MAX
#define ITER_DEFAULT 3
#define ITER_LOW 1


char BRG[] = "BRG";
char BRGLONG[] = "Dragonfly";
char DBG[] = "DBG";
char DBGLONG[] = "Butterfly";

//functions defined from wrapper.c
extern void catena_BRG(const uint8_t lambda, const uint8_t garlic);
extern void catena_BRGFH(const uint8_t lambda, const uint8_t garlic);
extern void catena_DBG(const uint8_t lambda, const uint8_t garlic);
extern void catena_DBGFH(const uint8_t lambda, const uint8_t garlic);

//Constraints
static int algorithm;
static int garlic = -1;
static int lambda = -1;
static int iterations = ITER_DEFAULT;	//Number of runs
static int full_hash = 0;			//use fullhash or not


void print_usage(char **argv){
	fprintf(stderr,"Usage: %s --algorithm ALG --garlic G ",argv[0]);
	fputs("--lambda L --iterations I\n", stderr);
}


int chkparams(){
    if(garlic < GARLIC_LOW || lambda < LAMBDA_LOW){
    	puts("garlic and lambda required");
    	return 1;
    }
    return 0;
}


int parse_args(int argc, char **argv)
{
	while (1)
	{
		static struct option long_options[] =
		{
		  {"algorithm",	required_argument, 	0, 'a'},
		  {"garlic",	required_argument, 	0, 'g'},
		  {"lambda",	required_argument, 	0, 'l'},
		  {"iterations",required_argument, 	0, 'i'},
		  {"full_hash",	no_argument, 		0, 'f'},
		  /*The last element of the array has to be filled with zeros.*/
		  {0, 			0, 					0, 	0}
		};

		int r; //return value of getopt_long is the corresponding val
		char* endptr = NULL; //for parsing numbers
		long temp;
		errno = 0;

		//_only also recognizes long options that start with a single -
		r = getopt_long_only(argc, argv, "a:g:l:i:f", long_options, NULL);

		/* Detect the end of the options. */
		if (r == -1)
			break;

		switch (r)
		{
			case 'a':
				if(strcmp(optarg, BRG)==0 || strcmp(optarg, BRGLONG)==0){
					algorithm = 0;
				}
				else if(strcmp(optarg, DBG)==0 || strcmp(optarg, DBGLONG)==0){
					algorithm = 1;
				}
				else{
					puts("Algorithm invalid");
				}
				break;

			case 'g':
				temp = strtol(optarg, &endptr, 10);
				if (*endptr != '\0') {
					fputs("argument for garlic could not be parsed \n",
						stderr);
					return 1;	
				}
				if(temp < GARLIC_LOW || temp > GARLIC_HIGH || errno == ERANGE){
					fputs("garlic out of range\n", stderr);
			    	return 1;
			    }
			    garlic = temp;
				break;

			case 'l':
				temp = strtol(optarg, &endptr, 10);
				if (*endptr != '\0') {
					fputs("argument for LAMBDA could not be parsed \n",
						stderr);
					return 1;	
				}
				if(temp < LAMBDA_LOW || temp > LAMBDA_HIGH || errno == ERANGE){
					fputs("LAMBDA out of range\n", stderr);
			    	return 1;
			    }
			    lambda = temp;
				break;

			case 'i':
				temp = strtol(optarg, &endptr, 10);
				if (*endptr != '\0') {
					fputs("argument for iterations could not be parsed \n",
						stderr);
					return 1;	
				}
				if(temp < ITER_LOW || temp > ITER_HIGH || errno == ERANGE){
					fputs("iterations out of range\n", stderr);
			    	return 1;
			    }
			    iterations = temp;
				break;

			case 'f':
				full_hash = 1;
				break;
		}
	}

	//getopt already informs about unrecognized options
	if (optind < argc)
    {
    	fputs("Some arguments could not be assigned to an option \n", stderr);
    	return 1;
    }

    return chkparams();
}

//taken from libc documentation
int compare_doubles (const void *a, const void *b)
{
  const double *da = (const double *) a;
  const double *db = (const double *) b;

  return (*da > *db) - (*da < *db);
}


double measure(int DBG, uint8_t lambda, uint8_t garlic){
	double t[iterations];
	clock_t diff;
	
	for(int i = 0; i < iterations; i++){
		diff = clock();
		if(DBG){
			if(full_hash){
				catena_DBGFH(lambda, garlic);
			}
			else{
				catena_DBG(lambda, garlic);
			}
		}
		else{
			if(full_hash){
				catena_BRGFH(lambda,garlic);
			}
			else{
				catena_BRG(lambda, garlic);
			}
		}
	
		diff = clock() - diff;
		t[i] = ((double)diff) / CLOCKS_PER_SEC;
	}
	qsort(t, iterations, sizeof(t[0]), compare_doubles);
	
	return t[iterations/2]; //median
}


void print_result(double t){
	printf("Hashing requires %.2fs\n", t);
}


int main(int argc, char **argv){

	if(parse_args(argc,argv)){
		print_usage(argv);
		return 1;
	}
	double t = measure(algorithm, lambda, garlic);
	
	print_result(t);

	return 0;
}
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
#include<malloc.h>
#include<math.h>
int partition( double A[], int left, int right );
void print(double *x,int n)
{
    int i=0;
    for(;i<n;i++)
    {
        printf("%.3lf ",x[i]);
    }
    printf("\n");
}
void qsort_versions(double l[],int n, int version);
int *parallelScan( int *inputVec, int N);
int partition_parallel( double *A, int left, int right );
void qsort_seq( double A[], int left, int right );
void qsort_v1( double A[], int left, int right );
void qsort_v2( double A[], int left, int right );
void qsort_v3( double A[], int left, int right );
void qsort_versions(double l[],int n, int version);


int main( int argc, char ** argv )
{
    int version;
    int n;//problem size
    if (argc < 2 ) 
    {
		printf("We will use the default settings\n");
		n=100000;
        version=1;
  
    }
    else
    {
		version = atoi( argv[1] );
		n=atoi(argv[2]);
	}  
    int i;
	srand(time(0));
	double* ATest=(double *)malloc(n*sizeof(double));
	for(i=0;i<n;++i)
	{
		ATest[i]=1.0*rand()/RAND_MAX;
	}
 
	double start,end; 
	double seconds=0;
	
	start=omp_get_wtime(); 
	qsort_versions(ATest,n, version);
	end=omp_get_wtime();
	
	seconds=(double)(end-start);

	printf("%lf ",seconds); 

	printf( "\n" );
	free(ATest);
}
int partition( double A[], int left, int right )
{
    double pivot = A[left];
    int i = left - 1;
    int j = right + 1;
    while(1)
    {
        do { j = j - 1; 
        } while ( A[j] > pivot );
        do { i = i + 1; 
        } while ( A[i] < pivot );
        if (i < j)
        {
            //swap A[i] with A[j]
            double temp = A[i];
            A[i] = A[j];
            A[j] = temp;
        }
        else
            {return j;}
    }
}
int *parallelScan( int *inputVec, int N)
{
	 int i,sum, offset;;
    int  *arrSum;
    int *a = (int*)malloc( sizeof(int)*N);
  
    #pragma omp parallel private(sum, offset)
    {
        const int ithread = omp_get_thread_num();
        const int nthreads = omp_get_num_threads();
        #pragma omp single
        {
            arrSum = (int*)malloc( sizeof(int)*(nthreads+1));
            arrSum[0] = 0;
        }
        sum = 0;
        #pragma omp for
        for ( i=0; i<N; i++) {
            sum += inputVec[i];
            a[i] = sum;
        }
        arrSum[ithread+1] = sum;
        #pragma omp barrier
        offset = 0;
        for( i=0; i<(ithread+1); i++) {
            offset += arrSum[i];
        }
        #pragma omp for
        for( i=0; i<N; i++) {
            a[i] += offset;
        }
    }
    free(arrSum);
    return a;
}
int partition_parallel( double *A, int left, int right )
{
    double pivot = A[left];
    int n = right - left + 1;
    int *smallArray = (int*)malloc(sizeof(int)*n);
    int *equalArray = (int*)malloc(sizeof(int)*n);
    int *largArray = (int*)malloc(sizeof(int)*n);
    int i;
    #pragma omp parallel for private(i)
    for (i = left; i <= right; ++i)
    {
        smallArray[i] = (A[i] < pivot);
        equalArray[i] = (A[i] == pivot);
        largArray[i] = (A[i] > pivot);
    }
    int *smallLocations = parallelScan( smallArray, n );
    int *equalLocations = parallelScan( equalArray, n );
    int *largeLocations = parallelScan( largArray , n );
   
    int sCnt = 0;
    #pragma omp parallel for reduction(+:sCnt)
    for (i = left; i <= right; ++i)
        sCnt += smallArray[i];
    int ncount = 0;
    #pragma omp parallel for reduction(+:ncount)
    for (i = left; i <= right; ++i)
        ncount += equalArray[i];
    double *sArray = (double*)malloc(sizeof(double)*n);
    #pragma omp parallel for private(i)
    for (i=left; i <= right; i++)
        sArray[i] = A[i];
    const int nthreads = omp_get_num_threads();
    int *locations = (int*)malloc(sizeof(int)*nthreads);
    #pragma omp parallel for private(i)
    for (i = left; i <= right; ++i)
    {
        const int ithread = omp_get_thread_num();
        if (equalArray[i] == 1)
        {
            locations[ithread] = i;
        }
            
    }
    int loc = n+1;
    for (i=0; i<nthreads; i++)
    {
        if (locations[i] < loc)
            loc = locations[i];
    }

    #pragma omp parallel for private(i)
    for (i = left; i <= right; ++i)
    {
        if (smallArray[i] == 1)
        {
            A[smallLocations[i]-1] = sArray[i];
        }
        else if (equalArray[i] == 1)
        {
            A[equalLocations[i]-1+sCnt] = sArray[i];
        }
        else if (largArray[i] == 1)
        {
            A[largeLocations[i]-1+sCnt+ncount] = sArray[i];
        }
        else {}
    }
    free(sArray);
    free(locations);
    free(smallArray);
    free(equalArray);
    free(largArray);
    free(smallLocations);
    free(equalLocations);
    free(largeLocations);

    return loc;
}
void qsort_seq( double A[], int left, int right ) 
{
    if ( left < right ) 
    {
        int p = partition(A, left, right);
        qsort_seq(A, left, p);
        qsort_seq(A, p + 1, right);
    }
}
void qsort_v1( double A[], int left, int right ) 
{
    if ( left < right )
    {
        int p = partition(A, left, right);
        
        #pragma omp parallel
        {
            #pragma omp sections
            {
                #pragma omp section 
                qsort_v1(A, left, p);
                #pragma omp section
                qsort_v1(A, p + 1, right);
            }
        }
    }
}

void qsort_v2( double A[], int left, int right ) 
{
    if ( left < right )
    {
        int p = partition(A, left, right);
        int size = right - left + 1;
        #pragma omp
        {
        #pragma omp task if (size > 1 <<10 )
        qsort_v2(A, left, p);
        #pragma omp task if (size > 1 <<10 )
        qsort_v2(A, p + 1, right);
        #pragma omp taskwait //
        }
    }
}

void qsort_v3( double A[], int left, int right ) 
{
    if ( left < right )
    {
        int p = partition_parallel(A, left, right);
       
        #pragma omp
        {
            #pragma omp task
            qsort_seq(A, left, p);
            #pragma omp task
            qsort_seq(A, p + 1, right);
        }
    }
}
void qsort_versions(double l[],int n, int version)
{
    
    if (version == 0)
        qsort_seq(l,0,n-1); //sort from the first element to the last
    else if (version == 1)
        qsort_v1(l, 0, n-1);
    else if (version == 2)
        qsort_v2(l, 0, n-1);
    else if (version == 3)
        qsort_v3(l, 0, n-1);
    else {}//do nothing
}

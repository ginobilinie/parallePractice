#include<cstdio>
#include<time.h>
#include<omp.h>
#include<iostream>
#include<algorithm>
#include<math.h>
using namespace std;
#define N 10040000

int f[N],LC[N],RC[N],cnt;
double A[N],ans[N];
int n;// length of sequence
int th;// number of thread
void output(int i)
{
	if(i>=n) return;
	output(LC[i]);
	ans[cnt++]=A[i];
	output(RC[i]);
}
void printArray(double arr[], int len)
{
	for (int i=0;i<len;i++)
	{	
		printf("%f ",arr[i]);
	}
	printf("\n");
}
void qsort_seq( double A[], int left, int right );
int partition( double A[], int left, int right );
int main (int argc, char ** argv)
{
	int i,root;
	bool flag=false;
	double f1,f2;
	int T=12;
	int version;//to decide run for problem size or number of threads
	//printf("please type length of sequence and number threads\n");
	//char Usage[]="usage: ./quicksort3 version(1/2)";
	//while(scanf("%d%d",&n,&th)!=EOF) // input the length of sequence and number of thread
	if (argc <= 1 ) 
    {
        //fprintf( stderr, Usage);
		printf("We will use the default settings\n");
        version=1;//to run the problem size 
    }
    else
    {
		version = atoi( argv[1] );
	}  
	
	n=100000;
	const int arrLen=10;
	
	int nums[10]={100,1000,10000,50000,100000,200000,300000,400000,500000,1000000};
//	int nums[arrLen]={10};
	th=16;
	//for (th=1;th<17;th++)
	if (version ==1)
	{
		for (int kk=0;kk<arrLen;kk++)
		{
			n=nums[kk];
			cnt=0;
			//generate the original sequence
			srand(time(0));
			double* ATest=(double *)malloc(n*sizeof(double));
			//double* A=(double *)malloc(n*sizeof(double));
			for(i=0;i<n;++i)
			{
				 A[i]=rand()*1.0/RAND_MAX;
				ATest[i]=A[i];
			}
			//printArray(A,n);
			//first=clock();
			int mxTH=omp_get_max_threads();
			//printf("max # of threads availble is %d\n",mxTH);
			double beginTime,endTime;
			beginTime=omp_get_wtime();
			omp_set_num_threads(th);
		#pragma omp parallel private(i) shared(root)
			{
		#pragma omp for 
				for(i=0;i<n;++i)
				{
					root=i;// compete to be the root of the balance tree
					LC[i]=RC[i]=n+1;
				}
		#pragma omp for
				for(i=0;i<n;++i)
					f[i]=root;
			}
			
		#pragma omp parallel private(i,flag) shared(A,f,LC,RC)
			{
		#pragma omp for
				for(i=0;i<n;++i)
				if(i!=root)
				{
					flag=false;
					while(!flag)
					{
						if(A[i]<A[f[i]] || A[i]==A[f[i]] && i<f[i]) //go to left
						{
							if(LC[f[i]]>n)// if no left child
							{
								LC[f[i]]=i;
								flag=true;
							}
							else f[i]=LC[f[i]];
						}
						else //go to right
						{
							if(RC[f[i]]>n)// if no right child
							{
								RC[f[i]]=i;
								flag=true;
							}
							else f[i]=RC[f[i]];
						}
					}
				}
			}
			endTime=omp_get_wtime();
			//output(root); // recursively get the sorted sequence
			//printArray(ans,n);
			double first,second;
			//second=clock();
			//printf("n is %d\n",n);
			//printf("parallel time is %f\n",f1=(endTime-beginTime));
			f1=endTime-beginTime;
			double c=100000;
			//double gp=n/c*log(n)/f1;
			double gp_par=n/c*log(n)/f1;
			
			first=omp_get_wtime();
			qsort_seq(ATest,1,n-1);
			second=omp_get_wtime();
			f2=second-first;
			double gp_seq=n/c*log(n)/f2;
			//printf("%d %f %f\n",th,f1,gp);
			printf("%d %f %f %f %f\n",n,f1,gp_par,f2,gp_seq);
		//	printArray(ATest,n);
			//first=clock();
			//second=clock();
			//printf("sequence time is %lf\n",f2=(second-first));
			//printf("rate is %lf\n",f2/f1);
			free(ATest);
		}
	}
	else 
	{
		for (th=1;th<17;th++)
		{
			//n=nums[kk];
			cnt=0;
			//generate the original sequence
			srand(time(0));
			double* ATest=(double *)malloc(n*sizeof(double));
			//double* A=(double *)malloc(n*sizeof(double));
			for(i=0;i<n;++i)
			{
				 A[i]=rand()*1.0/RAND_MAX;
				ATest[i]=A[i];
			}
			//printArray(A,n);
			//first=clock();
			int mxTH=omp_get_max_threads();
			//printf("max # of threads availble is %d\n",mxTH);
			double beginTime,endTime;
			beginTime=omp_get_wtime();
			omp_set_num_threads(th);
		#pragma omp parallel private(i) shared(root)
			{
		#pragma omp for 
				for(i=0;i<n;++i)
				{
					root=i;// compete to be the root of the balance tree
					LC[i]=RC[i]=n+1;
				}
		#pragma omp for
				for(i=0;i<n;++i)
					f[i]=root;
			}
			
		#pragma omp parallel private(i,flag) shared(A,f,LC,RC)
			{
		#pragma omp for
				for(i=0;i<n;++i)
				if(i!=root)
				{
					flag=false;
					while(!flag)
					{
						if(A[i]<A[f[i]] || A[i]==A[f[i]] && i<f[i]) //go to left
						{
							if(LC[f[i]]>n)// if no left child
							{
								LC[f[i]]=i;
								flag=true;
							}
							else f[i]=LC[f[i]];
						}
						else //go to right
						{
							if(RC[f[i]]>n)// if no right child
							{
								RC[f[i]]=i;
								flag=true;
							}
							else f[i]=RC[f[i]];
						}
					}
				}
			}
			endTime=omp_get_wtime();
			//output(root); // recursively get the sorted sequence
			//printArray(ans,n);
			double first,second;
			//second=clock();
			//printf("n is %d\n",n);
			//printf("parallel time is %f\n",f1=(endTime-beginTime));
			f1=endTime-beginTime;
			double c=100000;
			//double gp=n/c*log(n)/f1;
			double gp_par=n/c*log(n)/f1;
			
			first=omp_get_wtime();
			qsort_seq(ATest,1,n-1);
			second=omp_get_wtime();
			f2=second-first;
			double gp_seq=n/c*log(n)/f2;
			printf("%d %f %f\n",th,f1,gp_par);
			//printf("%d %f %f %f %f\n",n,f1,gp_par,f2,gp_seq);
		//	printArray(ATest,n);
			//first=clock();
			//second=clock();
			//printf("sequence time is %lf\n",f2=(second-first));
			//printf("rate is %lf\n",f2/f1);
			free(ATest);
		}
	}
	return 0;
}

int partition( double A[], int left, int right )
{
    double pivot = A[left];
    int i = left - 1;
    int j = right + 1;
    while(1)
    {
        do 
		{ 
			j = j - 1; 
        } while ( A[j] > pivot );
        do 
		{ 
			i = i + 1; 
        } while ( A[i] < pivot );
        if (i < j)//swap A[i] with A[j]
        {
            double temp = A[i];
            A[i] = A[j];
            A[j] = temp;
        }
        else
		{
			return j;
		}
    }
}

void qsort_seq( double A[], int left, int right ) 
{
    if ( left < right ) 
    {
        int key = partition(A, left, right);
        qsort_seq(A, left, key);
        qsort_seq(A, key + 1, right);
    }
}

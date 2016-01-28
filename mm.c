// A simple matrix multiply code to show affect of ordering and blocking
// to compile this use gcc -O2 mm.c -lrt or icc -O2 mm.c -lrt

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define MATSIZE 8000
#define BLOCKSIZE 8

void setmat() ;
void fillmat() ;
void abasicmm() ;
void abettermm() ;
void ablockmm() ;
void checkmatmult() ;

int main(int argc, char *argv[])
{

   double *a, *b, *c, *aa ;
   unsigned int n ;
   unsigned i, j, k, iInner, jInner, kInner, blockSize ;
   struct timespec ts1, ts2, ts3, ts4, ts5, ts6, ts7 ;

   printf("hello code beginning\n") ;
   n = MATSIZE ; // default settings
   blockSize = BLOCKSIZE ;
   if (argc != 3)
   {
      printf("input matrix size and blocksize\n") ;
      exit(0);
   }
   n = atoi(argv[1]) ;
   blockSize = atoi(argv[2]) ;
   printf("matrix size %d blocksize %d\n", n,blockSize) ;
   if (n%blockSize)
   {
      printf("for this simple example matrix size must be a multiple of the block size.\n  Please re-start \n") ;
      exit(0);
   }
// allocate matrices
   a = (double *)calloc((n+blockSize)*(n+blockSize), sizeof(double)) ;
   b = (double *)calloc((n+blockSize)*(n+blockSize), sizeof(double)) ;
   c = (double *)calloc((n+blockSize)*(n+blockSize), sizeof(double)) ;
   aa = (double *)calloc((n+blockSize)*(n+blockSize), sizeof(double)) ;
   if (aa == NULL) // cheap check only the last allocation checked.
   {
     printf("insufficient memory \n") ;
     exit(0) ;
   }

// fill matrices

   setmat(n, n, a) ;
   setmat(n, n, aa) ;


   srand(4.16) ; // set random seed (change to go off time stamp to make it better

   fillmat(n,n,b) ;
   fillmat(n,n,c) ;

   clock_gettime(CLOCK_REALTIME, &ts1) ;
// multiply matrices
   abasicmm (n,n,a,b,c) ;

   clock_gettime(CLOCK_REALTIME, &ts2) ;

   setmat(n, n, a) ;

   clock_gettime(CLOCK_REALTIME, &ts3) ;

   abettermm (n,n,a,b,c) ;

   clock_gettime(CLOCK_REALTIME, &ts4) ;

   ablockmm (n, n, aa, b, c, blockSize) ;

   clock_gettime(CLOCK_REALTIME, &ts5) ;

   printf("matrix multplies complete \n") ; fflush(stdout) ;

/**/
   checkmatmult(n,n,a,aa) ;

   {
      double t1, t2, t3, tmp ;
      t1 =  ts2.tv_sec-ts1.tv_sec;
      tmp = ts2.tv_nsec-ts1.tv_nsec;
      tmp /= 1.0e+09 ;
      t1 += tmp ;
      printf("ijk ordering basic time %lf\n",t1) ;
      t2 =  ts4.tv_sec-ts3.tv_sec;
      tmp = ts4.tv_nsec-ts3.tv_nsec;
      tmp /= 1.0e+09 ;
      t2 += tmp ;
      printf("ikj ordering bette time %lf\n",t2) ;
      t3 =  ts5.tv_sec-ts4.tv_sec;
      tmp = ts5.tv_nsec-ts4.tv_nsec;
      tmp /= 1.0e+09 ;
      t3 += tmp ;
      printf("ikj blocked time        %lf\n",t3) ;

   }

}

void setmat(int n, int m, double a[n][m])
{
   int i, j ;

   for (i=0;i<n; i++)
      for (j = 0 ; j<m; j++)
      {
         a[i][j] = 0.0 ;
      }
}

void fillmat(int n, int m, double a[n][m])
{
   int i, j ;

   for (i = 0; i<n; i++)
      for (j = 0 ; j < m; j++)
      {
         a[i][j] = (double)rand() / 3.1e09 ;
      }
}

void abasicmm(int n, int m, double a[n][m], double b[n][m], double c[n][m])
{
   int i, j, k ;

   for (i=0;i<n; i++)
      for (j = 0; j<n; j++)
         for (k=0;k<n; k++)
            a[i][j] += b[i][k]* c[k][j] ;
}

void abettermm(int n, int m, double a[n][m], double b[n][m], double c[n][m])
{
   int i, j, k ;

   for (i=0;i<n; i++)
      for (k=0;k<n; k++)
         for (j = 0; j<n; j++)
            a[i][j] += b[i][k]* c[k][j] ;
}
void ablockmm(int n,int m,double a[n][m],double b[n][m],double c[n][m], int blockSize)
{
   int i, j, k, iInner, jInner, kInner ;
   for (i = 0; i < n; i+=blockSize)
      for (k = 0 ; k < n; k+=blockSize)
         for (j=0; j<n ; j+= blockSize)
            for (iInner = i; iInner<i+blockSize; iInner++)
               for (kInner = k ; kInner<k+blockSize ; kInner++)
                  for (jInner = j ; jInner<j+blockSize; jInner++)
                        a[iInner][jInner] += b[iInner][kInner] * c[kInner][jInner] ;
}

void checkmatmult(int n,int m, double a[n][m], double aa[n][m])
{
// crude check.  Never compare floating point or double with ==.
// most floating point results are too sensitive to order of operations.
// this worked(sizes up to about 4600) it was quick, in general this is not safe
   int i, j ;
for (i=0;i<n;i++)
for (j=0;j<m;j++)
   if (a[i][j]-aa[i][j] != 0.0) printf("diff i %d %d diff %lf\n",i,j,a[i][j]=aa[i][j]) ;
printf("check OK\n") ;
}

/**/
// use thsi main instead of the one above to measure impact of unalinged matrix
// this version expects intel compiler
/*
int main(int argc, char *argv[])
{
 
   double *a, *b, *c, *aa ;
   unsigned int n ;
   unsigned i, j, k, iInner, jInner, kInner, blockSize ;
   struct timespec ts1, ts2, ts3, ts4, ts5, ts6, ts7 ;

   printf("hello code beginning\n") ;
   n = MATSIZE ; // default settings
   blockSize = BLOCKSIZE ;
   if (argc != 3) {printf("input matrix size and blocksize\n") ; exit(0);}
   n = atoi(argv[1]) ;
   blockSize = atoi(argv[2]) ;
   printf("matrix size %d blocksize %d\n", n,blockSize) ;
   if (n%blockSize) {printf("for this simple example matrix size must be a multiple of the block size.\n  Please re-start \n") ; exit(0); }
// allocate matrices
   a = (double *)_mm_malloc((n+blockSize)*(n+blockSize)* sizeof(double),64) ;
   b = (double *)_mm_malloc((n+blockSize)*(n+blockSize)* sizeof(double),64) ;
   c = (double *)_mm_malloc((n+blockSize)*(n+blockSize)* sizeof(double),64) ;
   aa = (double *)_mm_malloc((n+blockSize)*(n+blockSize)* sizeof(double),64) ;
   if (aa == NULL)
   {
     printf("insufficient memory \n") ;
     exit(0) ;
   }

// fill matrices

   aa += 3 ; //offset aa.  aa is intentionally allocated larger than required.
   setmat(n, n, a) ;
   setmat(n, n, aa) ;


   srand(4.16) ; // set random seed (change to go off time stamp to make it better

   fillmat(n,n,b) ;
   fillmat(n,n,c) ;

   clock_gettime(CLOCK_REALTIME, &ts1) ;
// multiply matrices


   ablockmm (n, n, a, b, c, blockSize) ;
   clock_gettime(CLOCK_REALTIME, &ts2) ;
   ablockmm (n, n, aa, b, c, blockSize) ;
   clock_gettime(CLOCK_REALTIME, &ts3) ;

   printf("matrix multplies complete \n") ; fflush(stdout) ;

   checkmatmult(n,n,a,aa) ;

   {
      double t1, t2, t3, tmp ;
      t1 =  ts2.tv_sec-ts1.tv_sec;
      tmp = ts2.tv_nsec-ts1.tv_nsec;
      tmp /= 1.0e+09 ;
      t1 += tmp ;
      printf("aligned block time   %lf\n",t1) ;
      t2 =  ts3.tv_sec-ts2.tv_sec;
      tmp = ts3.tv_nsec-ts2.tv_nsec;
      tmp /= 1.0e+09 ;
      t2 += tmp ;
     printf("unaligned block time %lf\n",t2) ;
   }
}
*/

   


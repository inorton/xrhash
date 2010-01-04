#include <stdio.h>

#define XRHASH_SLOTS 32768

#include "xrhash.h"
#include <assert.h>

#include <sys/time.h>

#define TEST_STR "foo%d"



long long
timeval_diff(struct timeval *difference,
             struct timeval *end_time,
             struct timeval *start_time
            )
{
  struct timeval temp_diff;

  if(difference==NULL)
  {
    difference=&temp_diff;
  }

  difference->tv_sec =end_time->tv_sec -start_time->tv_sec ;
  difference->tv_usec=end_time->tv_usec-start_time->tv_usec;

  /* Using while instead of if below makes the code slightly more robust. */

  while(difference->tv_usec<0)
  {
    difference->tv_usec+=1000000;
    difference->tv_sec -=1;
  }

  return 1000000LL*difference->tv_sec+
                   difference->tv_usec;

} /* timeval_diff() */

int main( int argc, char** argv )
{
  struct timeval tstart;
  struct timeval tend;

  XRHash * xr = xr_init_hash( NULL, NULL );

  int x = 0;
  int contains = -1;
  int datacount = 100000;

  int datalen = 10 + strlen(TEST_STR); /* ten digits */

  char* data_vec = (char*) malloc ( datacount * datalen * sizeof(char) );
  char * newstr = data_vec;
  fprintf(stderr,"test add\n");
  gettimeofday( &tstart, 0x0 );
  while ( x++ < datacount ){
    snprintf(newstr,datalen,TEST_STR,x);
    xr_hash_add( xr, newstr, (void*) x ); /* store value of x as a pointer in $xr{$newstr} */
/*    contains = xr_hash_contains( xr, newstr );*/
/*    assert( contains == XRHASH_EXISTS_TRUE ); */
    newstr += 4;
  }
  gettimeofday( &tend, 0x0 );
  fprintf(stderr,"* avg %lld us per add", (timeval_diff(NULL,&tend,&tstart))/datacount );
  fprintf(stderr,"\n");
  fprintf(stderr,"test get\n");
  newstr = data_vec;
  x = 0;
  while ( x++ < datacount ){
    int got = -1;
    contains = xr_hash_get( xr, newstr, (void**) &got );
    assert( contains == XRHASH_EXISTS_TRUE );
    assert( got == x );
    
    newstr += 4;
  }

  fprintf(stderr,"test remove\n");
  newstr = data_vec;
  x = 0;
  while ( x++ < datacount ){
    contains = xr_hash_remove( xr, newstr );
    assert( contains == XRHASH_REMOVED );
    newstr += 4;
  }

  free( xr );
  free(data_vec);

  return 0;
}

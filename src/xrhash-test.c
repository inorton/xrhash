#include <stdio.h>

#define XRHASH_SLOTS 32768

#include "xrhash.h"
#include <assert.h>

#include "testutils.h"
#include <sys/time.h>

#define TEST_STR "foo%d"
#define DATASET 64000

void iterate( XRHash * xr )
{
  XRHashIter * iter = xr_init_hashiterator( xr ); 
  void * key = NULL;
  int x = 0;

  while ( ( key = xr_hash_iteratekey( iter ) ) != NULL  ){
    void * value = NULL;
    assert ( xr_hash_get( xr, key , &value ) == XRHASH_EXISTS_TRUE );
    x++;
  }

  assert( x == DATASET );

  free(iter);
}

int main( int argc, char** argv )
{
  struct timeval tstart;
  struct timeval tend;

  XRHash * xr = xr_init_hash( NULL, NULL );

  int x = 0;
  int contains = -1;
  int datacount = DATASET;

  int datalen = 10 + strlen(TEST_STR); /* ten digits */

  char* data_vec = (char*) malloc ( datacount * datalen * sizeof(char) );
  char * newstr = data_vec;
  fprintf(stderr,"test add\n");
  gettimeofday( &tstart, 0x0 );
  while ( x++ < datacount ){
    snprintf(newstr,datalen,TEST_STR,x);
    xr_hash_add( xr, newstr, (void*) x ); /* store value of x as a pointer in $xr{$newstr} */
    newstr += 4;
  }
  gettimeofday( &tend, 0x0 );
  fprintf(stderr,"* avg %lld us per add", (timeval_diff(NULL,&tend,&tstart))/datacount );
  fprintf(stderr,"\n");


  fprintf(stderr,"test get\n");
  newstr = data_vec;
  x = 0;
  gettimeofday( &tstart, 0x0 );
  while ( x++ < datacount ){
    int got = -1;
    contains = xr_hash_get( xr, newstr, (void**) &got );
    assert( contains == XRHASH_EXISTS_TRUE );
    assert( got == x );    
    newstr += 4;
  }

  gettimeofday( &tend, 0x0 );
  fprintf(stderr,"* avg %lld us per get", (timeval_diff(NULL,&tend,&tstart))/datacount );
  fprintf(stderr,"\n");

  fprintf(stderr,"test iteration\n");
  gettimeofday( &tstart, 0x0 );
  iterate( xr );
  gettimeofday( &tend, 0x0 );
  fprintf(stderr,"* avg %lld us per iteration with get", (timeval_diff(NULL,&tend,&tstart))/datacount );
  fprintf(stderr,"\n");



  fprintf(stderr,"test remove\n");
  newstr = data_vec;
  x = 0;
  gettimeofday( &tstart, 0x0 );
  while ( x++ < datacount ){
    contains = xr_hash_remove( xr, newstr );
    assert( contains == XRHASH_REMOVED );
    newstr += 4;
  }
  gettimeofday( &tend, 0x0 );
  fprintf(stderr,"* avg %lld us per remove", (timeval_diff(NULL,&tend,&tstart))/datacount );
  fprintf(stderr,"\n");

  assert( xr->count == 0 );

  free( xr );
  free(data_vec);

  return 0;
}

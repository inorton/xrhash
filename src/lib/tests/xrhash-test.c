#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "xrhash.h"
#include <assert.h>

#include "testutils.h"
#include <sys/time.h>

#define TEST_STR "foo%d"

int assert_contains( XRHash * xr, void * keyptr, int expected )
{
  int rv = xr_hash_contains( xr, keyptr );
  int hc;
  if ( rv == expected ) return rv;
  hc = xr_get_hashcode( xr, keyptr );
  fprintf(stderr,"test failure: xr_hash_contains(xr=0x%08x,key=0x%08x) returned 0x%02x - expected 0x%02x\n",
    (int)xr, (int)keyptr, rv, expected );
  fprintf(stderr,"            : key hashcode = 0x%08x, key index = 0x%08x\n",
    hc, xr_get_index( xr, hc ) );
  abort();
}

int assert_get( XRHash * xr, void * keyptr, void ** outptr, int expected )
{
  int rv = xr_hash_get( xr, keyptr, outptr );
  int hc;
  if ( rv == expected ) return rv;

  hc = xr_get_hashcode( xr, keyptr );
  fprintf(stderr,"test failure: xr_hash_get(xr=0x%08x,key=0x%08x,outptr=0x%08x) returned 0x%02x - expected 0x%02x\n",
    (int)xr, (int)keyptr, (int)outptr, rv, expected );
  fprintf(stderr,"            : key hashcode = 0x%08x, key index = 0x%08x\n",
    hc, xr_get_index( xr, hc ) );

  abort();
}


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

  assert( x == xr->maxslots );

  free(iter);
}

int runtest( int hashsize )
{
  struct timeval tstart;
  struct timeval tend;

  XRHash * xr = xr_init_hash_len( &xr_hash__strhash , &xr_hash__strcmp, hashsize );

  int x = 0;
  int contains = -1;
  int datacount = hashsize;

  int datalen = 10 + strlen(TEST_STR); /* ten digits */

  char* data_vec = (char*) malloc ( (datacount * datalen * sizeof(char)) + datacount );
  char * newstr = data_vec;
  fprintf(stderr,"test add\n");
  gettimeofday( &tstart, 0x0 );
  while ( x++ < datacount ){
    snprintf(newstr,datalen,TEST_STR,x);
    xr_hash_add( xr, newstr, (void*) x ); /* store value of x as a pointer in $xr{$newstr} */
    newstr[datalen] = 0x0;
    newstr += datalen;
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
    assert_get( xr, newstr, (void**) &got, XRHASH_EXISTS_TRUE );
    assert( got == x );    
    newstr += datalen;
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
    contains = assert_contains( xr, newstr, XRHASH_EXISTS_TRUE );
    xr_hash_remove( xr, newstr );
    contains = assert_contains( xr, newstr, XRHASH_EXISTS_FALSE );
    newstr += datalen;
  }
  gettimeofday( &tend, 0x0 );
  fprintf(stderr,"* avg %lld us per remove", (timeval_diff(NULL,&tend,&tstart))/datacount );
  fprintf(stderr,"\n");

  assert( xr->count == 0 );




  fprintf(stderr,"\n---\ncompleted test:");
  fprintf(stderr,"unique items added    : %d\n",datacount);
  fprintf(stderr,"used unique indexes   : %ld\n",(long)xr->touched_indexes);
  fprintf(stderr,"index collisions      : %lu\n",(unsigned long)( datacount - xr->touched_indexes));
  fprintf(stderr,"collision factor      : %f\n", ( xr->touched_indexes + 0.0 ) / datacount );
  fprintf(stderr,"average lookups / key : %f\n", ( datacount + 0.0 ) / xr->touched_indexes );


  xr_hash_free( xr );
  free(data_vec);

  return 0;
}


int main( int argc, char** argv )
{
  runtest( 512 );
  runtest( 4096 );
  runtest( 8192 );
  runtest( 16384 );
  runtest( 65000 );
  runtest( 200000 );
  return 0;
}



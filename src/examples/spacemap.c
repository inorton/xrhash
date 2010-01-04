#include <ncurses.h>
#include <assert.h>
#include "spacemap.h"
#include <string.h>

int spaceship_id_cmp( void * id_a, void * id_b )
{
  return strcmp((char*)id_a,(char*)id_b);
}

/* id is a pointer to spaceship.id */
int spaceship_id_hash( void * id )
{
  /* this hashes strings in a similar way to the mono String.cs class */
  char* str = (char*) id;
  size_t len = strlen(str);
  int hash = 0;
  int c = 0;
  while ( c < len ){
    hash = (hash << 5) - hash + str[c];
    c++;
  }
  if ( hash < 1 )
    hash = 3 + ( hash * -1 );

  return hash;
}

spacemap * spacemap_init()
{
  spacemap * map = (spacemap*) malloc( 1 * sizeof(spacemap) );
  map->xr = xr_init_hash( &spaceship_id_hash, &spaceship_id_cmp );
  map->selected_ship = "bob";
  return map;
}

spaceship * new_spaceship( char* name, uint8 type, int x, int y )
{
  spaceship * ship = (spaceship*)malloc(1 * sizeof(spaceship));
  ship->name = strdup( name );
  ship->x = x * 100;
  ship->y = y * 100;

  ship->vx = 100;
  ship->vy = 50;

  return ship;
}

void spacemap_add( spacemap * map, spaceship * ship )
{
  xr_hash_add( map->xr, (void*)ship->name, ship );
}

spaceship * spacemap_get( spacemap * map, const char* name )
{
  spaceship * ret = NULL;
  xr_hash_get( map->xr, (void*) name, (void**) &ret );
  return ret;   
}


void moveships( spacemap * map )
{
  XRHashIter * iter = xr_init_hashiterator( map->xr );
  void * key = NULL;

  while ( ( key = xr_hash_iteratekey( iter ) ) != NULL ){
    spaceship * ship = NULL;
    xr_hash_get( map->xr, key, (void**) &ship );
    if ( ship == NULL )
      break;

    ship->x += ship->vx;
    ship->y += ship->vy;

    /* wrap/warp ship at edges of map - asteroids style */
    if ( ship->x > ( map->w * 100 )  ){
      ship->x = 0;
    }
    if ( ship->x < 0 ){
      ship->x = map->w * 100;
    }

    if ( ship->y > ( map->h * 100 ) ){
      ship->y = 0;
    }
    if ( ship->y < 0 ){
      ship->y = map->h * 100;
    }


  }

  free ( iter );
}

void clearships( spacemap * map, WINDOW * wind )
{
  XRHashIter * iter = xr_init_hashiterator( map->xr );
  void * key = NULL;

  while ( ( key = xr_hash_iteratekey( iter ) ) != NULL ){
    spaceship * ship = NULL;
    int shipx;
    int shipy;
    xr_hash_get( map->xr, key, (void**) &ship );
    if ( ship == NULL )
      break;
    
    shipx = ( ship->x / 100 ) ;
    shipy = ( ship->y / 100 ) ;


    mvwprintw( wind, shipy, shipx, " " );

    mvwprintw( wind, shipy+1,shipx+1,"          "); 

  }

  free ( iter );
}

void paintships( spacemap * map, WINDOW * wind )
{
  XRHashIter * iter = xr_init_hashiterator( map->xr );
  void * key = NULL;

  while ( ( key = xr_hash_iteratekey( iter ) ) != NULL ){
    spaceship * ship = NULL;
    int shipx;
    int shipy;
    xr_hash_get( map->xr, key, (void**) &ship );
    if ( ship == NULL )
      break;
    
    shipx = ( ship->x / 100 ) ;
    shipy = ( ship->y / 100 ) ;


    mvwprintw( wind, shipy, shipx, "*" );

    if ( strcmp( map->selected_ship, ship->name ) == 0 ){
      wattron(wind,A_REVERSE);      
    }

    mvwprintw( wind, shipy+1,shipx+1,"%s", ship->name ); 

    if ( strcmp( map->selected_ship, ship->name ) == 0 ){
      wattroff(wind,A_REVERSE);      
    }


  }

  free ( iter );
}

WINDOW * w_map;
WINDOW * w_info;

int main( int argc, char** argv )
{
  int map_width;
  int map_height;

  int ch;

  int info_width = 30;
  int info_height;

  int rand_x;
  int rand_y;

  spacemap * map = spacemap_init();
  spaceship * bob;
  spaceship * newship;

  char * shipname;

  /* setup ncurses */
  
  initscr();
  start_color();

  cbreak();
  init_pair(1,COLOR_RED, COLOR_BLACK); /* for selected ship */

  info_height = map_height = LINES - 2;
  map_width   = COLS - info_width - 2;

  map->w = map_width - 1;
  map->h = map_height - 1;

  w_map = newwin( map_height, map_width, 0, 0);
  box( w_map, 0,0 );
  w_info = newwin( info_height, info_width, 0, map_width + 1 );
  box( w_info, 0,0 );

  keypad( stdscr, TRUE );
  refresh();

  timeout(50);

  /* add first ship */
  bob = new_spaceship("bob",VIPER, 5, 12 );
  spacemap_add( map, bob );

  /* game loop */
  while((ch = getch()) != KEY_F(1))
  { switch(ch)
    {
      case KEY_F(2):
      /* make a new ship */
      rand_x = ((int) rand()) % map_width;
      rand_y = ((int) rand()) % map_height;
      shipname = (char*) malloc( 10 * sizeof(char));
      snprintf(shipname,10,"contact %d",map->xr->count + 1 );
      newship = new_spaceship( shipname, RAIDER, rand_x, rand_y );

      if ( (rand_x % 2 )) {rand_x = rand_x * -3; } else {rand_x = rand_x * 3; }
      if ( (rand_y % 2 )) {rand_y = rand_y * -3; } else {rand_y = rand_y * 3; }
      
      newship->vx = rand_x;
      newship->vy = rand_y;

      spacemap_add( map, newship );
      break;

      default:
      
      break;
    }
    wrefresh( w_map );
    wrefresh( w_info );
    /* move ships */
    clearships( map, w_map );
    moveships( map );    
    /* show ships */
    paintships( map, w_map );
    box( w_map, 0, 0 );

    /* game loop delay */
    timeout(500);
  }  

  endwin();

  return 0;
}


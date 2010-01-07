#include <ncurses.h>
#include <assert.h>
#include "spacemap.h"
#include <string.h>

int spaceship_id_cmp( void * id_a, void * id_b )
{
  return xr_hash__strcmp( id_a, id_b );
}

/* id is a pointer to spaceship.id */
int spaceship_id_hash( void * id )
{
  return xr_hash__strhash( id );
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

  ship->vx = 0;
  ship->vy = 0;

  return ship;
}

void new_random_ship(spacemap * map)
{
  int rand_x = ((int) rand()) % map->w;
  int rand_y = ((int) rand()) % map->h;
  char * shipname = (char*) malloc( 20 * sizeof(char));
  spaceship * newship;
  snprintf(shipname,20,"contact %d",map->xr->count + 1 );
  newship = new_spaceship( shipname, RAIDER, rand_x, rand_y );

  if ( (rand_x % 2 )) {rand_x = rand_x * -0.5; } else {rand_x = rand_x * 0.5; }
  if ( (rand_y % 2 )) {rand_y = rand_y * -0.5; } else {rand_y = rand_y * 0.5; }

  newship->vx = rand_x;
  newship->vy = rand_y;

  spacemap_add( map, newship );
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

void paintinfo ( spacemap * map, WINDOW * wind )
{

  int menu_row      = 0;
  int menu_max_rows = LINES / 2;
  XRHashIter * iter = xr_init_hashiterator( map->xr );
  void * key = NULL;
  spaceship * current = spacemap_get( map, map->selected_ship );
  werase( wind );
  
  /* paint selected item details */

  mvwprintw( wind, 3, 2, "Current Target" );
  wattron(wind,A_BOLD);

  mvwprintw( wind, 5, 2, "Name: %s", current->name );
  mvwprintw( wind, 6, 2, "Position: x = %d", current->x );
  mvwprintw( wind, 7, 2, "          y = %d", current->y );
   
  wattroff(wind,A_BOLD);

  /* paint list - with selected highlighted */ 


  mvwprintw( wind, 10, 2, "%d Radar Contact(s)", map->xr->count );

  while (( key = xr_hash_iteratekey( iter ) ) != NULL ){
    char * iname = (char*)key;

    if ( menu_row > menu_max_rows ) break;

    if ( strcmp(iname,current->name) == 0 ){
      wattron(wind,A_REVERSE);
    }

    mvwprintw( wind, 12 + menu_row, 2, "* %-12s", iname );


    if ( strcmp(iname,current->name) == 0 ){
      wattroff(wind,A_REVERSE);
    }

    menu_row++;

  }
  free(iter);

  /* paint help */

  mvwprintw( wind, LINES - 7, 2 , "F1   - Quit" );
  mvwprintw( wind, LINES - 6, 2 , "F2   - Create Ship" );
  mvwprintw( wind, LINES - 5, 2 , "F9   - Select Previous Ship" );
  mvwprintw( wind, LINES - 4, 2 , "F10  - Select Next Ship" );

  box( wind, 0,0 );
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



    if ( strcmp( map->selected_ship, ship->name ) == 0 ){
      wattron(wind,A_BOLD);      
      mvwprintw( wind, shipy, shipx, "*" );
    } else {
      wattron(wind,A_DIM);
      mvwprintw( wind, shipy, shipx, "." );
    } 


    if ( strcmp( map->selected_ship, ship->name ) == 0 ){
      mvwprintw( wind, shipy+1,shipx+1,"%s", ship->name ); 
      wattroff(wind,A_BOLD);      
    } else {
      wattron(wind,A_DIM);
    }


  }

  free ( iter );
}

void choose_next( spacemap * map )
{
  XRHashIter * iter = xr_init_hashiterator( map->xr );
  void * next = NULL;

  while (( next = xr_hash_iteratekey( iter ) ) != NULL )
  {
    if ( next != NULL ){
      if ( strcmp( map->selected_ship, (char*) next ) == 0 ){
        next = xr_hash_iteratekey( iter );
        if ( next != NULL ){
          map->selected_ship = next;
          break;
        }
      }
    }
  }
  free(iter);
}

void choose_previous ( spacemap * map )
{
  XRHashIter * iter = xr_init_hashiterator( map->xr );

  void * prev = NULL;
  void * next = NULL;

  do {
    if ( next != NULL ){
      if ( strcmp(next,map->selected_ship) == 0 ){
        if ( prev != NULL )
          map->selected_ship = prev;
        break;
      }
    }
    prev = next;
  } while ( ( next = xr_hash_iteratekey( iter ) ) != NULL );

  free(iter);
}





WINDOW * w_map;
WINDOW * w_info;

int main( int argc, char** argv )
{
  int map_width;
  int map_height;

  int ch;

  int info_width = 38;
  int info_height;

  spacemap * map = spacemap_init();
  spaceship * bob;

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
  bob->vx = 20;
  bob->vy = 34;
  spacemap_add( map, bob );

 
  /* game loop */
  while((ch = getch()) != KEY_F(1))
  { switch(ch)
    {
      case KEY_F(2):
      /* make a new ship */
      srand( bob->y );
      new_random_ship( map );
      break;

      case KEY_F(10):
      choose_next( map );
      break;

      case KEY_F(9):
      choose_previous( map );
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

    paintinfo( map, w_info );

    box( w_map, 0, 0 );

    /* game loop delay */
    timeout(100);
  }  

  endwin();

  return 0;
}


#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <ctime>
#include <sys/time.h>
#include <omp.h>
#include <cmath>

int g_width = 800;
int g_height = 600;
#define BPP 4
#define DEPTH 32


inline Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

inline void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

SDL_Surface* SDL_ScaleSurface(SDL_Surface* Surface, Uint16 Width, Uint16 Height)
{
    if(!Surface || !Width || !Height)
        return 0;

    SDL_Surface *_ret = SDL_CreateRGBSurface(Surface->flags, Width, Height, Surface->format->BitsPerPixel,
        Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);

    double  _stretch_factor_x = (static_cast<double>(Width)  / static_cast<double>(Surface->w)),
        _stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(Surface->h));

    for(Sint32 y = 0; y < Surface->h; y++)
        for(Sint32 x = 0; x < Surface->w; x++)
            for(Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
                for(Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
                        putpixel(_ret, _stretch_factor_x*x + o_x, (_stretch_factor_y*y)+o_y, getpixel(Surface, x, y));

    return _ret;
}



int sdl_or_die()
{
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    else
    {
        fprintf(stderr, "SDL init ok\n", SDL_GetError());
    }
    atexit(SDL_Quit);
}

void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 *pixmem32;
    Uint32 colour;

    colour = SDL_MapRGB( screen->format, r, g, b );

    pixmem32 = (Uint32*) screen->pixels  + y + x;
    *pixmem32 = colour;
}

void DrawScreen(SDL_Surface* screen, int h)
{
    int x, y, ytimesw;

    if(SDL_MUSTLOCK(screen))
    {
        if(SDL_LockSurface(screen) < 0) return;
    }

    for(y = 0; y < screen->h; y++ )
    {
        ytimesw = y*screen->pitch/BPP;
        for( x = 0; x < screen->w; x++ )
        {
            setpixel(screen, x, ytimesw, (x*x)/256+3*y+h, (y*y)/256+x+h, h);
        }
    }

    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

    SDL_Flip(screen);
}

inline Uint32 get_r(Uint32 p){return (p >> 16) & 0xff;}
inline Uint32 get_g(Uint32 p){return (p >> 8) & 0xff;}
inline Uint32 get_b(Uint32 p){return p & 0xff;}

SDL_Surface *image1, *image2, *rgb;

int main(int argc, char *argv[])
{
  #ifdef _OPENMP
   fprintf(stderr, "OpenMP supported!\n");
   //omp_set_dynamic(0);      // запретить библиотеке openmp менять число потоков во время исполнения
   //omp_set_num_threads(20); // установить число потоков в 10
  #endif

  sdl_or_die();

  SDL_Surface *screen, *scaled_screen;
  SDL_Event event;

  int keypress = 0;
  int h=0;

  if (!(screen = SDL_SetVideoMode(g_width, g_height, DEPTH, SDL_HWSURFACE)))
  {
      SDL_Quit();
      return 1;
  }

	/* load the image */
	image1=IMG_Load(argv[1]);
	if(!image1)
	{
		/* image failed to load */
		printf("IMG_Load: %s\n", IMG_GetError());
		SDL_Quit();
		return 1;
	}

	/* load the image */
	image2=IMG_Load(argv[2]);
	if(!image2)
	{
		/* image failed to load */
		printf("IMG_Load: %s\n", IMG_GetError());
		SDL_Quit();
		return 1;
	}

  /* print some info about the image */
	printf("loaded %s: %dx%d %dbpp\n", argv[1],
			image1->w, image1->h, image1->format->BitsPerPixel);

  // ---------------------------------------------------------------------------
  rgb = SDL_CreateRGBSurface(image1->flags, image1->w, image1->h, image1->format->BitsPerPixel,
      image1->format->Rmask, image1->format->Gmask, image1->format->Bmask, image1->format->Amask);

  int bpp = image1->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */

  clock_t start;
  double diff, omp_start;

  SDL_LockSurface(image1);
  SDL_LockSurface(image2);
  SDL_LockSurface(rgb);

  fprintf(stderr, "Begin parallel part\n");
  start = clock();
  #ifdef _OPENMP
    omp_start = omp_get_wtime();
  #endif

  Uint32 r=0, g=0, b=0, p1=0, p2=0, r1=0, r2=0, g1=0, g2=0, b1=0, b2=0;
  int x=0,y=0;
  int px, py;
  int blur_size=16;
  int blur_border = (blur_size*2 +1);
  int blur_sum_factor = (blur_size*2 +1)*(blur_size*2 +1);
  for (y=blur_border; y < image1->h-blur_border; y++)
  {
    {
        for (x=blur_border; x < image1->w-blur_border; x++)
        {

          p1 = getpixel(image1, x, y);
          r1 = get_r(p1);
          g1 = get_g(p1);
          b1 = get_b(p1);

          p2 = getpixel(image2, x, y);
          r2 = get_r(p2);
          g2 = get_g(p2);
          b2 = get_b(p2);

          int mod = x%90;
          if(mod<=30)
          {
            r = r1 + r2;
            g = g1 + g2;
            b = b1 + b2;
          }
          else if(mod>30 && mod<=60)
          {
            r = r1;
            g = g1;
            b = b1;
          }
          else if(mod>60)
          {
            r = r2;
            g = g2;
            b = b2;
          }

          putpixel(rgb, x, y, SDL_MapRGB( screen->format, r, g, b ) );

        }
    }
  }

   #ifdef _OPENMP
     double omp_diff = omp_get_wtime() - omp_start;
   #endif
   diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
   fprintf(stderr, "End parallel part\n");
   #ifdef _OPENMP
   fprintf(stderr, "OMP Work time: %8.8f\n", omp_diff);
   #endif
   fprintf(stderr, "SYS Work time: %8.8f\n", diff);
   SDL_UnlockSurface(image1);
   SDL_UnlockSurface(image2);
   SDL_UnlockSurface(rgb);

   scaled_screen = SDL_ScaleSurface(rgb, g_width, g_height);
   //exit(0);
  // ---------------------------------------------------------------------------

  while(!keypress)
  {
       //DrawScreen(screen,h++);

       SDL_BlitSurface(scaled_screen,0,screen,0);
       SDL_Flip(screen);

       while(SDL_PollEvent(&event))
       {
            switch (event.type)
            {
                case SDL_QUIT:
              keypress = 1;
              break;
                case SDL_KEYDOWN:
                     keypress = 1;
                     break;
            }
       }
  }

  SDL_Quit();

  return 0;
}

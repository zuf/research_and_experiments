#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <ctime>
#include <sys/time.h>
#include <omp.h>
#include <cmath>




#define WIDTH 1280
#define HEIGHT 960
#define BPP 4
#define DEPTH 32


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

SDL_Surface *image, *rgb;

int main(int argc, char *argv[])
{
  #ifdef _OPENMP
   fprintf(stderr, "OpenMP supported!\n");
   //omp_set_dynamic(0);      // запретить библиотеке openmp менять число потоков во время исполнения
   //omp_set_num_threads(20); // установить число потоков в 10
  #endif

  sdl_or_die();

  SDL_Surface *screen;
  SDL_Event event;

  int keypress = 0;
  int h=0;

  if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
  {
      SDL_Quit();
      return 1;
  }

	/* load the image */
	image=IMG_Load(argv[1]);
	if(!image)
	{
		/* image failed to load */
		printf("IMG_Load: %s\n", IMG_GetError());
		SDL_Quit();
		return 1;
	}

  /* print some info about the image */
	printf("loaded %s: %dx%d %dbpp\n", argv[1],
			image->w, image->h, image->format->BitsPerPixel);


  // ---------------------------------------------------------------------------
  rgb = SDL_CreateRGBSurface(image->flags, image->w, image->h, image->format->BitsPerPixel,
      image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);

  int bpp = image->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */

  clock_t start;
  double diff, omp_start;

  SDL_LockSurface(image);
  SDL_LockSurface(rgb);

  fprintf(stderr, "Begin parallel part\n");
  start = clock();
  #ifdef _OPENMP
    omp_start = omp_get_wtime();
  #endif

  Uint32 p, r,g,b;
  int x,y;

  for (x=0; x < image->w; x++)
  {
    #pragma omp parallel shared(x) private(y,r,g,b,p)
    {
      #pragma omp for
        for (y=0; y < image->h; y++)
        {
          //printf("Thread %d\n", omp_get_thread_num());

          p = getpixel(image, x, y);

          r = 255 - (Uint32)(cos(p)*cos(y)*sin((p >> 16) & 0xff)*255.0f);
          g = 255 - (Uint32)(cos(p)*cos(y)*sin((p >> 8) & 0xff)*255.0f);
          b = 255 - (Uint32)(cos(p)*cos(y)*sin((p & 0xff))*255.0f);

          putpixel(rgb, x, y, ((r<<16) | (g<<8) | b) );
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
   SDL_UnlockSurface(image);
   SDL_UnlockSurface(rgb);
   //exit(0);
  // ---------------------------------------------------------------------------

  while(!keypress)
  {
       //DrawScreen(screen,h++);

       SDL_BlitSurface(rgb,0,screen,0);
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

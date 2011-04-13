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

inline int max(int a, int b)
{
  return a>b ? a : b;
}

inline int min(int a, int b)
{
  return a<=b ? a : b;
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

SDL_Surface *image, *rgb;

inline double step1(int layers_size, double q)
{
  return (1-q) / (1 - pow(q, 1+layers_size));
}

inline double stepn(int n, int layers_size, double q=0.75)
{
  return ((1-q) / (1 - pow(q, 1+layers_size))) * (pow(q, n));
}

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
  double rd=0.0,gd=0.0,bd=0.0;
  int x,y;
  int px, py;
  int blur_size=4;
  int blur_border = (blur_size*2 +1);
  int blur_sum_factor = (blur_size*2 +1)*(blur_size*2 +1);
  double *w;
  float q=0.75;
  w = (double *)malloc(sizeof(double)*blur_size);

  w[0]=(1-q)/(1-pow(q,blur_size+1));
  for(int n=1; n<=blur_size; n++)
  {
    w[n] = w[n-1]*q / ((n*2 +1)*(n*2 +1));
  }

  double test_sum=0;
  for(int n=0; n<=blur_size; n++)
  {
    printf("w[%d] = %f\n", n, w[n]);
    test_sum+=w[n];
  }
  printf("SUM(w)=%f\n\n", test_sum);

  //exit(0);
  for (y=blur_border; y < image->h-blur_border; y++)
  {
    #pragma omp parallel shared(y) private(x,r,g,b,p,px,py,rd,gd,bd)
    {
      #pragma omp for
        for (x=blur_border; x < image->w-blur_border; x++)
        {
          //printf("Thread %d\n", omp_get_thread_num());

          //p = getpixel(image, x, y);

          //r = 255 - (Uint32)(cos(p)*cos(y)*sin((p >> 16) & 0xff)*255.0f);
          //g = 255 - (Uint32)(cos(p)*cos(y)*sin((p >> 8) & 0xff)*255.0f);
          //b = 255 - (Uint32)(cos(p)*cos(y)*sin((p & 0xff))*255.0f);

          r=0;
          g=0;
          b=0;
          rd=0;
          gd=0;
          bd=0;

          for(py=-blur_size; py <=blur_size; py++ )
          {
            for(px=-blur_size; px <=blur_size; px++ )
            {
              if (px!=0 && py!=0)
              {
                p = getpixel(image, x+px, y+py);
                rd += ((double)get_r(p)) * w[abs(max(px,py))];
                gd += ((double)get_g(p)) * w[abs(max(px,py))];
                bd += ((double)get_b(p)) * w[abs(max(px,py))];
                if(rd>255)
                printf("rd=%f\n", rd);
              }
            }
          }
          //rd /= blur_sum_factor;
          //gd /= blur_sum_factor;
          //bd /= blur_sum_factor;

          r=min((Uint32)rd, 255);
          g=min((Uint32)gd, 255);
          b=min((Uint32)bd, 255);

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

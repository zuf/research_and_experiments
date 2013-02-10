#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libraw/libraw.h>

// Cоздадим обработчик изображений
LibRaw iProcessor;

int send_raw_preview_to_stdout(char *file)
{             
        // Открыть файл и считать метаданные
        int result;
        result = iProcessor.open_file(file);
        if (LIBRAW_SUCCESS!=result)
        {
          fprintf(stderr, "Can't open file %s. Error: %s\n", file, iProcessor.strerror(result));
          return -1;
        }       

        // Метаданные доступны в полях данных класса
        //printf("Image size: %d x %d\n",iProcessor.imgdata.sizes.width,iProcessor.imgdata.sizes.height);

        // Распакуем изображение
        //iProcessor.imgdata.params.half_size = 1;
        result = iProcessor.unpack_thumb();
        if (LIBRAW_SUCCESS!=result)
        {
          fprintf(stderr, "Can't unpack thumbnail for file %s. Error: %s\n", file, iProcessor.strerror(result));
          return -2;
        }
        
        if (iProcessor.imgdata.thumbnail.tformat==LIBRAW_THUMBNAIL_JPEG)
        {
          //printf("Thumbnail format: JPEG [%dx%d] length: %d\n", iProcessor.imgdata.thumbnail.twidth, iProcessor.imgdata.thumbnail.theight,  iProcessor.imgdata.thumbnail.tlength);
          
          //int image_width = iProcessor.imgdata.thumbnail.twidth;
          //int image_height = iProcessor.imgdata.thumbnail.theight;
                    
          fwrite(iProcessor.imgdata.thumbnail.thumb, sizeof(char), iProcessor.imgdata.thumbnail.tlength, stdout);
        }
        else if (iProcessor.imgdata.thumbnail.tformat==LIBRAW_THUMBNAIL_BITMAP)
          fprintf(stderr, "Thumbnail format not supported: BITMAP [%dx%d]\n", iProcessor.imgdata.thumbnail.twidth, iProcessor.imgdata.thumbnail.theight);          
        else
          fprintf(stderr, "Thumbnail format: unknown or unsupported\n");
                      
        // Освободим процессор для работы со следующим изображением
        iProcessor.recycle();
        return 0;
}

struct Options {
  bool silend_mode;
  int use_every_n_file;
}; 

Options options;
int init_options (int argc, char **argv)
{
  int c;
  options.silend_mode = true;
  options.use_every_n_file=0;
  
 const char *help = "Usage: cat-raw-previews [KEYS]\n"
            "Reads raw photo files list from stdin and output preview images (JPEG) from that files to stdout.\n\n"            
            "Keys:\n"
            "  -h\tshow this help and exit\n"
            "  -v\tverbose mode - print processed files with some stats\n"
            "  -e N\tuse only N-th file from input (where N - integer number). For example \"-e e\" reads each 3-rd file from stdin\n"
            "\n"
            "Examples:\n"
            "  Play CR2 photos with mplayer:\n"
            "  find -iname '*CR2' | cat-raw-previews | mplayer - -nosound -fps 25 -demuxer lavf -lavfdopts format=mjpeg -vf dsize=1280:-3,scale=1280:-3\n"
            "\n"
            "  Save preview from CR2 to file:\n"
            "  echo IMG_0001.CR2 | cat-raw-previews > preview.jpg\n"
            "\n"
            "  Make avi from previews of CR2 photos (useful for time-lapse video):\n"
            "  find ~/photos -type f -name '*.CR2' | cat-raw-previews | ffmpeg -f image2pipe -r 25 -vcodec mjpeg -i - -vf scale=1920:-1 -qmin 1 -qmax 1 -c:v mjpeg photostream.avi\n"
            "\n";
  
  while ((c = getopt (argc, argv, "hve:")) != -1)
  {
     switch (c)
     {
       case 'v': // verbose mode
           options.silend_mode = false;
         break;
       case 'e': // use only every N-th file
         options.use_every_n_file = atoi(optarg);
         break;
       case 'h':
         ;
         fprintf(stderr, help);
         exit(0);
         break;
       case '?':
         if (optopt == 'e')
           fprintf (stderr, "Option -%c requires an argument.\n", optopt);
         else if (isprint (optopt))
           fprintf (stderr, "Unknown option `-%c'.\n", optopt);
         else
           fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
         return 1;
       default:
         abort ();
     }
  }
  return 0;
}
  
int main(int argc, char **argv)
{  
  if (0!=init_options(argc, argv))
  {
    fprintf(stderr, "ERROR: Can\t init options.\n");    
  }
  
  char filename[FILENAME_MAX];
  int n=0;
  double seconds_from_start=0.0;
  int result;
  double fps; // files per second
  time_t current;
  time_t start = time(NULL);
  while(fgets(filename, FILENAME_MAX, stdin))
  {
    n++;
    if (options.use_every_n_file<=0 || n%options.use_every_n_file==0)
    {
      int len = strlen(filename);
      if( filename[len-1] == '\n' )
          filename[len-1] = 0;
      result=send_raw_preview_to_stdout(filename);
      current=time(NULL);
      seconds_from_start = difftime(current, start);
      if (result==0)
      {
        fps = n/seconds_from_start;
        if (!options.silend_mode)
        {
          fprintf(stderr, "%d. %.1fs FPS: %.2f %s\n", n, seconds_from_start, fps, filename);
        }
      }
    }
  }
  fflush(stdout);  
  fprintf(stderr, "\n");
  return 0;
}


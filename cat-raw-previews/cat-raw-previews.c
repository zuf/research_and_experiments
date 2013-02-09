#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#include <libraw/libraw.h>

#define MAX_FILENAME 4096

// Cоздадим обработчик изображений
LibRaw iProcessor;

int send_raw_preview_to_stdout(char *file)
{             
        // Открыть файл и считать метаданные
        if (LIBRAW_SUCCESS!=iProcessor.open_file(file))
        {
          fprintf(stderr, "Can't open file %s\n", file);        
          return -1;
        }       

        // Метаданные доступны в полях данных класса
        //printf("Image size: %d x %d\n",iProcessor.imgdata.sizes.width,iProcessor.imgdata.sizes.height);

        // Распакуем изображение
        //iProcessor.imgdata.params.half_size = 1;
        if (LIBRAW_SUCCESS!=iProcessor.unpack_thumb())
        {
          fprintf(stderr, "Can't unpack thumbnail for file %s\n", file);        
          return -2;
        }
        
        if (iProcessor.imgdata.thumbnail.tformat==LIBRAW_THUMBNAIL_JPEG)
        {
          //printf("Thumbnail format: JPEG [%dx%d] length: %d\n", iProcessor.imgdata.thumbnail.twidth, iProcessor.imgdata.thumbnail.theight,  iProcessor.imgdata.thumbnail.tlength);
          
          int image_width = iProcessor.imgdata.thumbnail.twidth;
          int image_height = iProcessor.imgdata.thumbnail.theight;
                    
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


  
int main(int argc, char *argv[])
{  
  char filename[MAX_FILENAME];
  int n=0;
  double seconds_from_start=0.0;
  int result;
  double fps; // files per second
  time_t current;
  time_t start = time(NULL);
  while(fgets(filename, MAX_FILENAME, stdin))
  {
    n++;
    int len = strlen(filename);
    if( filename[len-1] == '\n' )
        filename[len-1] = 0;
    result=send_raw_preview_to_stdout(filename);
    current=time(NULL);
    seconds_from_start = difftime(current, start);
    if (result==0)
    {
      fps = n/seconds_from_start;
      fprintf(stderr, "%d. %.1fs FPS: %.2f %s\n", n, seconds_from_start, fps, filename);
    }
  }
  fflush(stdout);  
  fprintf(stderr, "\n");
  return 0;
}


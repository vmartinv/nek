#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint_least32_t u32;
const u32 magic=0xBF;

struct initrd_header
{
   u32 magic; // The magic number is there to check for consistency.
   char name[64];
   u32 length; // Length of the file.
};

#define ARG_OFFSET 2
int main(int argc, char *argv[]){
	if(argc<2){
		printf("Usage: initrd_gen destimage file1 file2 file3 file4\n");
		return 1;
	}
   u32 nheaders = argc-2;
   struct initrd_header headers[nheaders];
   fprintf(stderr, "size of header: %lu\n", sizeof(struct initrd_header));
   u32 off = sizeof(struct initrd_header) * nheaders + sizeof(int);
   int i;
   for(i = 0; i < nheaders; i++)   {
       strcpy(headers[i].name, argv[ARG_OFFSET+i]);
       char *nam=basename(headers[i].name);
       strcpy(headers[i].name, nam);
       fprintf(stderr, "writing file %s at 0x%x\n",headers[i].name, off);
       FILE *stream = fopen(argv[ARG_OFFSET+i], "r");
       if(stream == 0){
         fprintf(stderr, "Error: file not found: %s\n", argv[ARG_OFFSET+i]);
         return 1;
       }
       fseek(stream, 0, SEEK_END);
       headers[i].length = ftell(stream);
       off += headers[i].length;
       fclose(stream);
       headers[i].magic = magic;
   }
   
   FILE *wstream = fopen(argv[1], "w");
   if(wstream == 0){
	 fprintf(stderr, "Error: can't write image: %s\n", argv[1]);
	 return 1;
   }
   unsigned char *data = (unsigned char *)malloc(off);
   fwrite(&magic, sizeof(u32), 1, wstream);
   fwrite(&nheaders, sizeof(u32), 1, wstream);
   for(i = 0; i < nheaders; i++){
     FILE *stream = fopen(argv[ARG_OFFSET+i], "r");
     unsigned char *buf = (unsigned char *)malloc(headers[i].length);
	 fwrite(&headers[i], sizeof(struct initrd_header), 1, wstream);
     fread(buf, 1, headers[i].length, stream);
     fwrite(buf, 1, headers[i].length, wstream);
     fclose(stream);
     free(buf);
   }
   fclose(wstream);
   free(data);
   return 0;
}

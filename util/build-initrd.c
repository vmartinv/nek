#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
typedef struct {
	char 		magic_a; //I
	char 		magic_b; //N
	char 		magic_c; //I
	char 		magic_d; //T 4
	uint32_t 	headers;   // 8
	uint32_t    checksum;// A + B + C + D + files
	uint8_t     pad[52];
} superheader_t;

typedef struct
{
   uint32_t magic;     // Magic number, for error checking.
   char name[52];  // Filename.
   uint32_t offset;   // Offset in the initrd that the file starts.
   uint32_t length;   // Length of the file.
} fileheader_t;

int main(int argc, char **argv)
{
	int nheaders = (argc-1)/2;
	fileheader_t headers[nheaders];
	if(nheaders == 0)
	{
		printf("usage: ./initutil [file] [file_dest]\n");
	}
	
	superheader_t sheader = {'I','N','I','T',nheaders,'I' + 'N' + 'I' + 'T' + nheaders};
	
	unsigned int off = sizeof(fileheader_t) * nheaders + sizeof(superheader_t);
	
	for(int i = 0; i < nheaders; i++)
	{
		printf("[ + ]| %15s | %10s @ 0x%08x\n", argv[i*2+1], argv[i*2+2], off);
		strcpy(headers[i].name, argv[i*2+2]);
		headers[i].offset = off;
		FILE *stream = fopen(argv[i*2+1], "r");
		if(stream == 0)
		{
		  printf("[ERR]| %15s | Doesn't exist\n", argv[i*2+1]);
		  return 1;
		}
		fseek(stream, 0, SEEK_END);
		headers[i].length = ftell(stream);
		off += headers[i].length;
		fclose(stream);
		headers[i].magic = 0xABABCBDC;
	}

    FILE *wstream = fopen("./initrd.img", "w");
    unsigned char *data = (unsigned char *)malloc(off);
    fwrite(&sheader, sizeof(superheader_t), 1, wstream);
    fwrite(headers, sizeof(fileheader_t), nheaders, wstream);
   
    for(int i = 0; i < nheaders; i++)
    {
      FILE *stream = fopen(argv[i*2+1], "r");
      unsigned char *buf = (unsigned char *)malloc(headers[i].length);
      fread(buf, 1, headers[i].length, stream);
      fwrite(buf, 1, headers[i].length, wstream);
      fclose(stream);
      free(buf);
    }
   
    fclose(wstream);
    free(data);
   
	
}
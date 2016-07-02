#define DIR_AMOUNT 8 //allow to have 8 files in this VD
#define Disk_Buf_AMOUNT 8//allow to have 8 files in this VD
#define BLOCK_SIZE 2


//define Dir struct:
typedef struct {
  char FILENAME[20];
  int FIRST_BLOCK;
  int LAST_BLOCK;
  int ID;
} Dir_struct;


//global var
int *FAT;
int fat_size;
Dir_struct **Dir;
char **Disk_Buf;
int header;//to save space in VD for FAT and DIR 
FILE *fpVD;

//funcution prototype
int myfs_create(const char*filesystemname,int max_size);
int find_FAT_free_block();
int myfs_file_create(const char* filename);
int myfs_file_open(const char* filename);
int myfs_file_write(int fd,char*buf,int count);
int myfs_file_read(int fd ,char* buf,int count);
int myfs_file_close(int fd);
int myfs_file_delete(const char*filename);
int free_Dir(int rows, Dir_struct **mat);
int free_Disk_Buf(int rows, char **mat);
int myfs_file_destroy(const char* filesystemname);

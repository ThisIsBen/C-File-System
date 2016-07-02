#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vd_header.h"//include header file.
#define max_size 40 //max_size=40 Bytes

void read(const char*first_file,int first_fd,int readcount)
{
	printf("Read content from file %s\n",first_file);
	char read_buf[100];
	//int readcount=8;
	read_buf[readcount]='\0';
	int readchar=myfs_file_read(first_fd ,read_buf,readcount);
	//if(myfs_file_read(0 ,readbuf,readcount)==readcount)
	if(readchar==readcount)
	{
		printf("read successfully!\n");
	}
	else
		printf("Fail to read in content in file!\n");
	printf("this is # of char that has been read in : %d\n",readchar);
	//print out the read in content
	printf("these are read in content: ");
	for(int j=0;j<strlen(read_buf);j++)
	{
		printf("%c",read_buf[j]);
	}
	printf("\n");

	printf("-------------------------------------------------\n");

}

void write(const char*first_file,int first_fd,char* write_buf)
{
	
	printf("Write content %s to file %s .\n",write_buf,first_file);
	myfs_file_write(first_fd,write_buf,strlen(write_buf));

	printf("-------------------------------------------------\n");



}

int main()
{
	
	//Create filesystem 
	const char* new_filesystem="NEWVD";
	
	if(myfs_create(new_filesystem,max_size)==1)
	{
		printf("file system %s has been set up!\n",new_filesystem);



	}
	else
		printf("file system failed to be set up.\n");

	
	//Create a file called ABC
	const char* first_file="ABC";
	myfs_file_create(first_file);//create a file called abc.
	int first_fd=myfs_file_open(first_file);
	printf("%d is fd of %s \n",first_fd,first_file);
	printf("-------------------------------------------------\n");
	//Create a file called DEF
	const char* second_file="DEF";
    myfs_file_create(second_file);//create a file called def.
	int second_fd=myfs_file_open(second_file);
	printf("%d is fd of %s \n",second_fd,second_file);
	printf("-------------------------------------------------\n");



//write content to file ABC .
	char* write_buf="abcdefg";
	write(first_file,first_fd,write_buf);
	


//write content to file ABC again.
    write_buf="hij";
    write(first_file,first_fd,write_buf);

    //write content to file DEF
    write_buf="klm";
    write(second_file,second_fd,write_buf);
    

	//read file ABC and DEF 
	read(first_file,first_fd,8);//read 8 bytes from file ABC
	read(second_file,second_fd,3);//read 3 bytes from file DEF
	

//test file close by read the Dir content back from VD again
	//close file ABC
	if(myfs_file_close(first_fd)==1)
	{
		printf("file closed successfully!\n");
	}
	else
		printf("Fail to close the file!\n");

	//read Dir info from VD to make sure storing to VD is successfully done.
   printf("Read Dir info from VD to make sure the update of Dir to VD is successfully done.\n");
   fseek(fpVD,4*fat_size,SEEK_SET );
   //memset(buffer,0,strlen(buffer));
   Dir_struct *buffer[DIR_AMOUNT];//read_buffer
   int nfile=0;
   for(int j=0;j<DIR_AMOUNT;j++)
   {
   		buffer[j]=NULL;

   }

   //read file data in Dir which is stored in VD.
   fread(buffer, 32, 1, fpVD);


   //find out the number of file stored in VD.
   for(int j=0;j<DIR_AMOUNT;j++)
   {
   		if(buffer[j]!=NULL)
   			nfile++;

   }
   
   

   //fread(buffer, 32, DIR_AMOUNT, fpVD);
   for(int i=0;i<nfile;i++)
   {	
      printf("Dir%d's Filename :",i);
      for(int j=0;j<strlen(buffer[i]->FILENAME);j++)
         printf("%c",buffer[i]->FILENAME[j]);
      printf("\n");
      printf("%d is Dir%d's First Block \n",buffer[i]->FIRST_BLOCK,i);
      printf("%d is Dir%d's Last Block \n",buffer[i]->LAST_BLOCK,i);
      printf("%d is Dir%d's ID \n",buffer[i]->ID,i);
   }
   fseek(fpVD,SEEK_SET, 0);//rewind
   printf("\n");

//end test file close :Dir
   int  FAT_content[fat_size];
   fseek(fpVD,SEEK_SET, 0);
   //memset(buffer,0,strlen(buffer));
   fread(FAT_content, 4, fat_size, fpVD);
   printf("Read FAT content from VD: ");
   for(int i=0;i<fat_size;i++)
   		printf(" %d", FAT_content[i]);
   printf("\n");
   rewind(fpVD);
   printf("-------------------------------------------------\n");

//end test file close :FAT


   //test delete file abc
   //error if the file doesn't exsit!
   const char* del_filename="ABC";
	if(myfs_file_delete(del_filename)==0)
	{

			printf("The file called : %s does not exist! You can't delete it!\n",del_filename);
			
	}
	else
	{
		printf("The file called :%s has been deleted!\n",del_filename);
	}
	
//test whether ABC is deleted!
//if core dumped means deletion is successful
	/*
	printf("When read content from the deleted file ABC, \n");
	read(first_file,first_fd,8);//read 8 bytes from file ABC
	*/
//end test whether ABC is deleted!
	printf("-------------------------------------------------\n");

   //end test delete file


	
   //test destroy the filesystem NEWVD.
   if(myfs_file_destroy(new_filesystem))
   	printf("Filesystem %s has been destroyed!\n",new_filesystem);
   else 
   	 printf("Filesystem %s wasn't be destroyed properly!\n",new_filesystem);
   	//end test destroy the filesystem NEWVD.
	
	




}
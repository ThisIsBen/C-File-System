#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vd_header.h"//include header file.

/*define Dir struct:
typedef struct {
  char FILENAME[20];
  int FIRST_BLOCK;
  int LAST_BLOCK;
  int ID;
} Dir_struct;
*/
/*
int *FAT;
int fat_size;
Dir_struct **Dir;
char **Disk_Buf;
int header;//to save space in VD for FAT and DIR 
FILE *fpVD;
*/

//create filesystem
int myfs_create(const char*filesystemname,int max_size)
{
	fat_size=max_size/BLOCK_SIZE;//2KB per block
	header=(4*(fat_size))+(32*DIR_AMOUNT);//size of FAT and DIR
	//setup FAT of size :max_size/2048
	 
	
    FAT=(int *) malloc(fat_size*sizeof(int));


    for(int i=0; i<fat_size;i++)//init each column to 0 : all the blocks are free.
    {
    	FAT[i]=0;

    }
//printf("%d is FAT's size \n",sizeof(FAT));

    ///////////////////////////////////
	
    
    //setup Dir of size 100: can have 100 files stored in this VD
    Dir=(Dir_struct **) malloc(DIR_AMOUNT*sizeof(Dir_struct*));
   //all the DIR pointer point to NULL at beginning.
    for(int i=0; i<DIR_AMOUNT; i++)
    /* Allocate array, store pointer  */
       Dir[i] = (Dir_struct *) NULL;
       ////////////////////////////////////////////////////// 
   //allcote DIR when create a file :Dir[0]=(Dir_struct *) malloc(sizeof(Dir_struct)*1); 

   //Dir[0]->FILENAME="BEN";
   /*
   Dir[0]->FIRST_BLOCK=25;
   Dir[0]->LAST_BLOCK=2;
   Dir[0]->ID=0;
   printf("%d is Dir's FB \n",Dir[0]->FIRST_BLOCK);
   printf("%d is Dir's LB \n",Dir[0]->LAST_BLOCK);
   printf("%d is Dir's ID \n",Dir[0]->ID);
    //printf("%s is Dir's FN \n",sizeof(Dir[0]->FILENAME));
*/
	//////////////////////////////////////////////////////////////
   //setup virtual disk of size: max_size+header
   //set VD pointer to point to the beginning of the VD file
    fpVD=fopen(filesystemname, "w+");
	fseek(fpVD, header+max_size, SEEK_SET);
	fputc('\n', fpVD);
	fseek(fpVD, 0, SEEK_SET);
	//fclose(fpVD);


	////////////////////////////////////////////
	//setup Disk_Buf of size 100
	 Disk_Buf = (char **) malloc( sizeof(char *) * ( Disk_Buf_AMOUNT ) );
	 for(int i=0; i<Disk_Buf_AMOUNT; i++)
    /* Allocate array, store pointer  */
    	Disk_Buf[i] = (char *) NULL; //all the Disk_Buf pointer point to NULL at beginning.


/////////////////////////////////
    //open virtual disk.
    //fpVD=fopen(filesystemname, "a+");
	//fseek(fpVD, 0, SEEK_SET); //set VDpointer to point to the beginning of the VD file.




    return 1;//means successfully create the filesystem.

}

int find_FAT_free_block()
{
	int free_block_no;
	for( free_block_no=0;free_block_no<fat_size;free_block_no++)
	{
		if(FAT[free_block_no]==0)//if free block is found
		{
			FAT[free_block_no]=-1;//init this block as the 1st block of this file 
			break;
		}


	}
 	return free_block_no;//return free block's index.


}




//create file
int myfs_file_create(const char* filename)
{
	int free_block_no;
	//find a free block in FAT:
	for(free_block_no=0;free_block_no<fat_size;free_block_no++)
	{
		if(FAT[free_block_no]==0)//if free block is found
		{
			FAT[free_block_no]=-1;//init this block as the 1st block of this file 
			break;
		}


	}
	//find a free column in Dir:
	for(int free_Dir_no=0;free_Dir_no<DIR_AMOUNT;free_Dir_no++)
	{
		if(Dir[free_Dir_no]==NULL)//if free block is found
		{
			Dir[free_Dir_no]=(Dir_struct *) malloc(sizeof(Dir_struct)*1); 
			for(int i=0;i<strlen(filename);i++)
				Dir[free_Dir_no]->FILENAME[i]=filename[i];
			Dir[free_Dir_no]->FIRST_BLOCK=free_block_no;
		    Dir[free_Dir_no]->LAST_BLOCK=free_block_no;
		    Dir[free_Dir_no]->ID=free_Dir_no;
		    
		    ////////////////trial
		    printf("filename: ");//print file name.
		    for(int i=0;i<strlen(Dir[free_Dir_no]->FILENAME);i++)
		    	printf("%c",Dir[free_Dir_no]->FILENAME[i]);
		    printf("\n");
		    printf("%d is FAT's free block number \n",free_block_no);
		    printf("%d is Dir's Dir number \n",free_Dir_no);
		    printf("%d is Dir's First Block \n",Dir[free_Dir_no]->FIRST_BLOCK);
		    printf("%d is Dir's Last Block \n",Dir[free_Dir_no]->LAST_BLOCK);
		    printf("%d is Dir's ID \n",Dir[free_Dir_no]->ID);
		    


			break;
		}


	}

	return  1;///means successfully create the file








}




int myfs_file_open(const char* filename)
{
	//find a column in Dir whose FILENAME == the input filename:
	int target_Dir_no;
	for(target_Dir_no=0;target_Dir_no<DIR_AMOUNT;target_Dir_no++)
	{	//if THE Dir column whose FILENAME == the input filename is found
		if(strcmp(Dir[target_Dir_no]->FILENAME,filename )==0)
		{
			//LET the the according disk_buf point to a char array of size 2048.
			Disk_Buf[target_Dir_no]=(char *) malloc(sizeof(char)*BLOCK_SIZE); 
			//trial
			//printf("%d is Disk_Buf's target_Dir_no \n",target_Dir_no);

			break;
		}

	}
	

	//printf("%d is len of Disk_Buf[target_Dir_no]\n",strlen(Disk_Buf[target_Dir_no]));
	//return fd
	return target_Dir_no;


}



//write content to file.
//!need to replace char *disk with int fd in the real world
int myfs_file_write(int fd,char*buf,int count)
{

	//get the content of the last block of this file to Disk_Buf for writing.
	fseek(fpVD, 0, SEEK_SET); //set VDpointer to point to the beginning of the VD file.
	fseek(fpVD, (Dir[fd]->LAST_BLOCK*BLOCK_SIZE)+header, SEEK_SET); 
	memset(Disk_Buf[fd],0,strlen(Disk_Buf[fd]));//clear disk_buf
	//load the content of the last blcok to Disk_Buf
	fread(Disk_Buf[fd], BLOCK_SIZE, 1, fpVD);

	//trial
	printf("The last block's position in virtual disk at beginning: %d\n", (Dir[fd]->LAST_BLOCK*BLOCK_SIZE)+header);



	printf("content in disk at the beginning of Write:\n");//store in VD
			
    for(int h=0;h<strlen(Disk_Buf[fd]);h++)
				printf("%c",Disk_Buf[fd][h]);//store in VD
	printf("\n");//store in VD
				
				


	//				
	
		
		
		
		int disk_len=strlen(Disk_Buf[fd]);
		
		if(disk_len+count<=BLOCK_SIZE)
		{
			for(int g=disk_len;g<disk_len+count;g++)
			{
				Disk_Buf[fd][g]=buf[g-disk_len];


			}
			
				//trial
			//printf("FAT_free_block found currently is %d :\n",FAT_free_block);//store in VD
			printf("content in disk :\n");//store in VD
			
			for(int h=0;h<strlen(Disk_Buf[fd]);h++)
				printf("%c",Disk_Buf[fd][h]);//store in VD
			printf("\n");//store in VD
				
				//
			
		}

		else
		{
			int current_block=Dir[fd]->LAST_BLOCK;

			int i=0;//don't delete this line!! it's critical
			
			int times=(count+strlen(Disk_Buf[fd]))/BLOCK_SIZE;
			int remain=(count+strlen(Disk_Buf[fd]))%BLOCK_SIZE;
			int left1=strlen(Disk_Buf[fd]);
			int left2=strlen(Disk_Buf[fd]);
			//printf("time %d , remain %d\n",times,remain);
			
			//to record the FAT_free_block currently found .
			int FAT_free_block;
			for(int k=0;k<times;k++)
			{
				//Next=Next BLOCK_SIZE
				int Next=i+BLOCK_SIZE-left1;
				for(;i<Next;i++)
				{
					Disk_Buf[fd][i+left2]=buf[i];




				}
				//because the Disk_Buf is full,so write it back to VD
				fseek(fpVD, 0, SEEK_SET); //set VDpointer to point to the beginning of the VD file.
				fseek(fpVD, (current_block*BLOCK_SIZE)+header, SEEK_SET); 
				//write the current change back to VD.
				fwrite(Disk_Buf[fd] , 1 , strlen(Disk_Buf[fd]) , fpVD );
				
				//find_FAT_free_block
				FAT_free_block=find_FAT_free_block();

				FAT[current_block]=FAT_free_block;
				FAT[FAT_free_block]=-1;
				Dir[fd]->LAST_BLOCK=FAT_free_block;
				current_block=FAT_free_block;


				left1=0;
				left2=-1*i;





				//trial
			printf("FAT_free_block found currently is %d :\n",FAT_free_block);//store in VD
			printf("Content stored to disk :\n");//store in VD
			
			for(int h=0;h<strlen(Disk_Buf[fd]);h++)
				printf("%c",Disk_Buf[fd][h]);//store in VD
			printf("\n");//store in VD
				
				//
				

				//clear Disk_Buf
				memset(Disk_Buf[fd],0,strlen(Disk_Buf[fd]));//clear disk_buf








			}

			int rest=remain+i;
			for(;i<rest;i++)
			{
				Disk_Buf[fd][i+left2]=buf[i];

			}

			//trial
			printf("The rest part stay in Disk Buffer: \n");

			
			for(int h=0;h<strlen(Disk_Buf[fd]);h++)
				printf("%c",Disk_Buf[fd][h]);//store in VD
			printf("\n");//store in VD
			   //printf("len of disk %d\n",strlen(disk));
			//

			








		}

		
		//update content in Disk_Buf back to VD.
		fseek(fpVD, 0, SEEK_SET); //set VDpointer to point to the beginning of the VD file.
		fseek(fpVD, (Dir[fd]->LAST_BLOCK*BLOCK_SIZE)+header, SEEK_SET); 
		
		//write the current change back to VD.
		fwrite(Disk_Buf[fd] , 1 , strlen(Disk_Buf[fd]) , fpVD );
		
		printf("The last block's position in virtual disk in the end: %d\n", (Dir[fd]->LAST_BLOCK*BLOCK_SIZE)+header);





		return 1;
}


//file read
int myfs_file_read(int fd ,char* buf,int count)
{
	/*update content in Disk_Buf back to VD before loading the 1st block to Disk_Buf.
		fseek(fpVD, 0, SEEK_SET); //set VDpointer to point to the beginning of the VD file.
		fseek(fpVD, (Dir[fd]->LAST_BLOCK*BLOCK_SIZE)+header, SEEK_SET); 
		
		//write the current change back to VD.
		fwrite(Disk_Buf[fd] , 1 , strlen(Disk_Buf[fd]) , fpVD );
	*/



	//get the content of the last block of this file to Disk_Buf for writing.
	fseek(fpVD, 0, SEEK_SET); //set VDpointer to point to the beginning of the VD file.
	fseek(fpVD, (Dir[fd]->FIRST_BLOCK*BLOCK_SIZE)+header, SEEK_SET); 
	memset(Disk_Buf[fd],0,strlen(Disk_Buf[fd]));//clear disk_buf
	//load the content of the last blcok to Disk_Buf
	fread(Disk_Buf[fd], BLOCK_SIZE, 1, fpVD);

	/*trial
	
			//printf("FAT_free_block found currently is %d :\n",FAT_free_block);//store in VD
			printf("content in disk :\n");//store in VD
			
			for(int h=0;h<strlen(Disk_Buf[fd]);h++)
				printf("%c",Disk_Buf[fd][h]);//store in VD
			printf("\n");//store in VD
				
				*/

	if(count<=BLOCK_SIZE)
	{
		for(int i=0;i<count;i++)
			buf[i]=Disk_Buf[fd][i];


	}

	else
	{
		int times=count/BLOCK_SIZE;
		int remain=count%BLOCK_SIZE;
		int current_block=Dir[fd]->FIRST_BLOCK;
		int Next;//Next=Next BLOCK_SIZE
		int d=0;
		int k;
		for(k=1;k<=times;k++)
		{
			//Next=Next BLOCK_SIZE
			Next=BLOCK_SIZE*k;
			for(;d<Next;d++)
			{
				buf[d]=Disk_Buf[fd][d-(BLOCK_SIZE*(k-1))];
				
			/*trial 
			printf("This is current buf[d]:%c\n",buf[d]);
			printf("This is current Disk_Buf[fd][d-(BLOCK_SIZE*(k-1))]: %c\n",Disk_Buf[fd][d-(BLOCK_SIZE*(k-1))]);
			//printf("This is current strlen(buf): %d\n",strlen(buf));
			*/

			}
			

			memset(Disk_Buf[fd],0,strlen(Disk_Buf[fd]));//clear disk_buf
			current_block=FAT[current_block];
			if(current_block!=0 && current_block!=-1)
			{
				//get the content of the last block of this file to Disk_Buf for writing.
				fseek(fpVD, 0, SEEK_SET); //set VDpointer to point to the beginning of the VD file.
				fseek(fpVD, (current_block*BLOCK_SIZE)+header, SEEK_SET); 
				//load the content of the last blcok to Disk_Buf
				fread(Disk_Buf[fd], BLOCK_SIZE, 1, fpVD);






			}


		}

		//if(FAT[current_block]==-1)
		//{
			//printf("in outer hi Ben!\n");
			for(;d<remain+Next;d++)
			{
				buf[d]=Disk_Buf[fd][d-(BLOCK_SIZE*(k-1))];

				/*trial
				printf("buf[d] : %c\n",buf[d]);
				printf("The final d: %d\n",d);
				//trial
				printf("This is strlen(buf) in read function: %d\n",strlen(buf));
				*/


			}


		//}









	}
	
	//return the length of read in buffer.
	return strlen(buf);


}

//file close
int myfs_file_close(int fd)
{
	/*update content in Disk_Buf back to VD .
		fseek(fpVD, 0, SEEK_SET); //set VDpointer to point to the beginning of the VD file.
		fseek(fpVD, (Dir[fd]->LAST_BLOCK*BLOCK_SIZE)+header, SEEK_SET); 
		
		//write the current change back to VD.
		fwrite(Disk_Buf[fd] , 1 , strlen(Disk_Buf[fd]) , fpVD );
	*/


	//return the Mem space of Disk_Buf[fd] .
	free(Disk_Buf[fd]);
	Disk_Buf[fd] = (char *) NULL; //all the Disk_Buf pointer point to NULL at beginning.
	

	//Update FAT table to VD ,which are store tempararily in Mem.

	fseek(fpVD, 0, SEEK_SET); //set VDpointer to point to the beginning of the VD file.	
	//write the current FAT table back to VD.
	fwrite(FAT , 4 , fat_size , fpVD );

	//Update Dir table to VD ,which are store tempararily in Mem.
	//Dir is stored just behind FAT table in VD.
	//fseek(fpVD, fd*32, SEEK_CUR); 
	//fwrite(Dir[fd],32,1,fpVD);
	fwrite(Dir,32,DIR_AMOUNT,fpVD);

	return 1;//which means file is closed successfully!





}


//to delete file.
int myfs_file_delete(const char*filename)
{
	int target_Dir_no;
	int file_found=0;
	
	for(target_Dir_no=0;target_Dir_no<DIR_AMOUNT;target_Dir_no++)
	{	//if THE Dir column whose FILENAME == the input filename is found
		if(strcmp(Dir[target_Dir_no]->FILENAME,filename )==0)
		{
			//indicate file is found!
			file_found=1;
			int Next_del_Block=FAT[Dir[target_Dir_no]->FIRST_BLOCK];

			//to indicate that this block is free now!
			FAT[Dir[target_Dir_no]->FIRST_BLOCK]=0;
			//trial
			
			printf("First Freed blcok: %d\n",FAT[Dir[target_Dir_no]->FIRST_BLOCK]);
			
			//
			//if this file occupy more than one block
			if(Next_del_Block!=-1)
			{
				while(FAT[Next_del_Block]!=-1)
				{
					int temp=FAT[Next_del_Block];
					//trial
					printf("Freed blcok: %d\n",Next_del_Block);
					//

					//to indicate that this block is free now!
					FAT[Next_del_Block]=0;

					Next_del_Block=temp;

					




				}
				//to indicate that the last block of this file is free now!
				//trial
				printf("Freed block: %d\n",Next_del_Block);
				//
				FAT[Next_del_Block]=0;
				



			}

			break;

		}

		
	}
	
	//trial
	return 1;
	
	if(file_found==0)
	{
		return 0;//means file doesn't exsit ,so can't be deleted.
	}
	else
	{
		//del that file's file info.
		free(Dir[target_Dir_no]);
		Dir[target_Dir_no]=NULL;

		//free that file's Disk_buf
		free(Disk_Buf[target_Dir_no]);
		Disk_Buf[target_Dir_no]=NULL;


		
		//Update FAT table to VD ,which are store tempararily in Mem.
		fseek(fpVD, 0, SEEK_SET); //set VDpointer to point to the beginning of the VD file.	
		//write the current FAT table back to VD.
		fwrite(FAT , 4 , fat_size , fpVD );

		//Update Dir table to VD ,which are store tempararily in Mem.
		//Dir is stored just behind FAT table in VD.
		fwrite(Dir,32,DIR_AMOUNT,fpVD);

		return 1; //means file is deleted and everything goes well.
	}


	
}

//to free Dir
int free_Dir(int rows, Dir_struct **mat){
 
    for(int i=0;i<rows;i++)    
    {
    	free(mat[i]);
    	mat[i]=NULL;
    }    
    free(mat);

    return 1;
}

//to free Disk_Buf
int free_Disk_Buf(int rows, char **mat){
 
    for(int i=0;i<rows;i++)    
    {
    	free(mat[i]);
    	mat[i]=NULL;
    }    
    free(mat);

    return 1;
}

int myfs_file_destroy(const char* filesystemname)
{
	fclose(fpVD);

	//int destory_done=remove()
	free(FAT);//free FAT table.
	FAT=NULL;
	//free Dir table
	int del_Dir=free_Dir(DIR_AMOUNT,Dir);
	//free Disk_Buf 
	int del_Disk_Buf=free_Disk_Buf(Disk_Buf_AMOUNT,Disk_Buf);


	//remove VD
	int remove_status=remove(filesystemname);

	if(del_Dir && del_Disk_Buf==1  && remove_status==0)
		return 1;//means filesystem is destroyed.
	else 
		return 0;//means filesystem isn't destroyed properly




}




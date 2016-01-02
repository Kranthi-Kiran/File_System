#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100
/* 
*******************************************************************************************
Name: Kranthi Kiran Kalletla
Net ID: wg8498
Assignment: To Create a file System.
FIles: 
filesystem.c     -- Code for the file System
COMMANDS.txt     -- File to store the commands for Restore Operation
LOG.txt          -- To print the log file to check the directory structure and the file.
*******************************************************************************************
*/
//Declaration of functions
void fn_restore();
void fn_create();
void fn_write(int new);
void fn_change_dir();
void fn_open();
void fn_read();
void fn_seek();
void fn_list();
void fn_help();
void fn_size();
void fn_delete(char* file_name);
void fn_create_list_of_Sectors();
void fn_add_empty_sector(int data);
int fn_delete_empty_sector();
void fn_print_empty_sectors();
void fn_open_dir(int data);
int fn_close_dir();
void fn_display_path();
void fn_print_Dir_sector(int presentSec);
void fn_print_file(int Sector_num);

//Declaring Structure to hold subdirectories and files
#pragma pack(1)
typedef struct DirStructure
{
	char TYPE;
	char NAME[9];
	int  LINK;
	short SIZE;
}DirStruct;

//Declaring Structure to hold Directory Structure
#pragma pack(1)
typedef struct
{
	int BACK;
	int FRWD;
	int FREE;
	char FILLER[4];
	DirStruct DIR[31];
}Directory;

//Declaring Structure to hold File Structure
#pragma pack(1)
typedef struct
{
	int BACK;
	int FRWD;
	char USER_DATA[504];
}File;

//Declaring the union of the above 2 srtuctures
typedef union
{
	Directory   directory;
	File  file;
}Sector;

struct list
{
    int info;
    struct list *prev;
	struct list *next;
}*front_dir,*rear_dir,*temp,*front_dir1;

//Decleration of variables
int cnt;
Sector *Sectors[100];  //Creating an array of addresses as to logically represent the sectors.
int sectornum=0;       //For storing the present sector number being refered to.
int presentSec=0;      //For storing the location of the present working directory.
int newSec=0;          //For storing the location of the new created sector if current working sector is full.
char file_name[9];     //For storing the current working file name.
char action[10];       //For Reading the action from the user.
char type;             //For Reading Type from the user.
char mode;             //For Reading Mode from the user.
int opened;            //For storing if the file is opened or not. 1 is opened 0 is closed.
char name[10];         //For Reading Name of File or Directory from the user.
char data[10000];      //For Reading Data to be inputted to File from user.
int bytes;             //For Reading bytes to read or write to a file.
int base;              //For Reading base.
int offset;            //For Reading offset.
int opened_link;       //For Storing the Sector number of the file opened.
int file_pointer;      //For Storing the pointer to a position in the file to read.
int LIST[MAX];         //Creating an array to store the list of empty sectors.
int rear = -1;
int front = -1;
int created = 0;
FILE *file;            //For writing the commands to be execuited for restoring the previous session.
FILE *f;               //For writing the debug into a file for debugging
char dir_name[9];      //For Reading the directory name to change directory
char restore;          //For Restoring the previously executed commands.
int restoring = 0;

void main()
{
	//clrscr();
	f = fopen("log.txt", "w+");
	if (f == NULL)
	{
		printf("Error opening file!\n");
	}

	fprintf(f,"\nProgram Started %d %d\n",front,rear);
	fn_create_list_of_Sectors();

	/*Creating root Directory and initializing it with the default values*/
	Sectors[0] = (Directory *) malloc (sizeof(Directory));
	Sectors[0]->directory.BACK=NULL;
	Sectors[0]->directory.FRWD=NULL;
	Sectors[0]->directory.FILLER[0]='\0';
	Sectors[0]->directory.FREE=NULL;
	for(cnt=0;cnt<31;cnt++)
	Sectors[0]->directory.DIR[cnt].TYPE='F';
	fn_open_dir(0);
	fn_print_Dir_sector(0);

	//fn_print_empty_sectors();
	/*for(cnt=0;cnt<MAX;cnt++)
	fprintf(f,"\nDeleted  %d",fn_delete_empty_sector());*/
	printf("Welcome to the File System.\n");
	fn_help();

	printf("Do you want to restore previous session[Y|N]: ");
	scanf("%c",&restore);
	if(restore == 'Y' || restore == 'y')
	{
		file = fopen("Commands.txt", "r");
		if(file == NULL)
		{
			printf("No file to restore\n");
			file = fopen("Commands.txt", "w");
		}
		else
		{
			fn_restore();
			fclose(file);
			file = fopen("Commands.txt", "a");
		}
	}
	else
	{
		printf("Restore File is overriten. \n");
		file = fopen("Commands.txt", "w");
	}
	while(1)
	{
		fn_display_path();
		printf("> ");
		scanf("%s",&action);
		fprintf(file," %s",action);
		//fprintf(f,"\nCase Statement: %s\n",action);
		if(strcmp(action,"CREATE") == 0)
		{
			fn_create();
		}
		else if (strcmp(action,"OPEN") == 0)
		{
			fn_open();
		}
		else if (strcmp(action,"CLOSE") == 0)
		{
			mode = NULL;
			strcpy(file_name,"\0");
			opened = 0;
			opened_link = NULL;
			file_pointer = NULL;
		}
		else if (strcmp(action,"DELETE") == 0)
		{
			scanf("%s",&name);
			fprintf(file," %s",name);
			fn_delete(name);
		}
		else if (strcmp(action,"READ") == 0)
		{
			fn_read();
		}
		else if (strcmp(action,"WRITE") == 0)
		{
			scanf("%d",&bytes);
			fprintf(file," %d",bytes);
			//printf("Enter data");
			fn_write(0);
		}
		else if (strcmp(action,"SEEK") == 0)
		{
			fn_seek();
		}
		else if (strcmp(action,"SIZE") == 0)
		{
			fn_size();
		}
		else if (strcmp(action,"SECTOR_SIZE") == 0)
		{
			printf("\nSize of Directory sector is: %d",sizeof(Directory));
			printf("\nSize of File sector is: %d\n",sizeof(File));
		}
		else if (strcmp(action,"CD") == 0)
		{
			fn_change_dir();
		}
		else if (strcmp(action,"LS") == 0)
		{
			fn_list();
		}
		else if (strcmp(action,"HELP") == 0)
		{
			fn_help();
		}
		else if (strcmp(action,"EXIT") == 0)
		{
			fprintf(f,"\nCase Statement: %s\n",action);
			exit(0);
		}
		else
		{
			printf("\nError: Command not found: %s TO EXECUTE\n",action);
			fprintf(f,"\nCommand not found: %s TO EXECUTE\n",action);
		}
	}
	fclose(f);
	fclose(file);
}

/*This function is used to print help*/
void fn_help()
{
	printf("--------------------------------------------------------------------------------\n");
	printf("This File System works with the following commands:\n");
	printf("CREATE <D|U> <NAME(9 Chars max)>   D - Directory U - User File\n");
	printf("OPEN <I|O|U> <FILE NAME(9 Chars max)>  I - Input, O - Output, U - Update\n");
	printf("CLOSE -- To Close any open file.\n");
	printf("DELETE <NAME>  -- To Delete a file. Only empty directories can be delted.\n");
	printf("READ <n>  -- To Read n bytes from the opened file.\n");
	printf("WRITE <n>  -- To WRITE n bytes to the opened file.\n");
	printf("SEEK <BASE> <OFFSET>  -- Move the pointer position in a file.\n");
	printf("CD <NAME> -- TO change the directory. Use \'..\' togo to the previous directory.\n");
	printf("LS -- To print list of files in the current directory.\n");
	printf("SIZE -- To print the size of the opened file\n");
	printf("HELP -- To print the above information.\n");
	printf("EXIT -- To exit from the system.\n");
	printf("--------------------------------------------------------------------------------\n");
}

/*This function is used to restore previous session*/
void fn_restore()
{
	char c;
	restoring = 1;
	fprintf(f,"In Restoring\n");
	c = fscanf(file,"%s",&action);
	while(c!=EOF)
	{
		//fn_display_path();
		//fscanf(file,"%s",&action);
		if(strcmp(action,"CREATE") == 0)
		{
			fn_create();
		}
		else if (strcmp(action,"OPEN") == 0)
		{
			fn_open();
		}
		else if (strcmp(action,"CLOSE") == 0)
		{
			mode = NULL;
			strcpy(file_name,"\0");
			opened = 0;
			opened_link = NULL;
			file_pointer = NULL;
		}
		else if (strcmp(action,"DELETE") == 0)
		{
			fscanf(file,"%s",&name);
			fn_delete(name);
		}
		else if (strcmp(action,"READ") == 0)
		{
			fn_read();
		}
		else if (strcmp(action,"WRITE") == 0)
		{
			fscanf(file,"%d",&bytes);
			fn_write(0);
		}
		else if (strcmp(action,"SEEK") == 0)
		{
			fn_seek();
		}
		else if (strcmp(action,"CD") == 0)
		{
			fn_change_dir();
		}
		else
		{
			NULL;
		}
		
		c = fscanf(file,"%s",&action);
	}
	mode = NULL;
	strcpy(file_name,"\0");
	opened = 0;
	opened_link = NULL;
	file_pointer = NULL;
	restoring = 0;
	fprintf(f,"Returning Restoring\n");
}

/*This function is used to create a new directory or file*/
void fn_create()
{
	int find_cnt = presentSec;
	int valid = 1;
	if(restoring == 0)
	{
		scanf(" %c",&type);
		scanf("%s",&name);
		fprintf(file," %c",type);
		fprintf(file," %s",name);
	}
	else
	{
		fscanf(file," %c",&type);
		fscanf(file," %s",&name);
	}
	//fgets(name, 9, stdin);
	created = 0;
	fprintf(f,"\nIn fn_create");
	fprintf(f,"\nAction: %s",action);
	fprintf(f,"\nType: %c",type);
	fprintf(f,"\nName: %s",name);

	if(opened != 0)
	{
		if(restoring == 0)
		printf("%s File is opened. Please close it and try again.\n",file_name);
		fprintf(f,"\nNo File is opened.\n");
		return;
	}
	if(type != 'D' && type != 'U')
	{
		if(restoring == 0)
		printf("Error: Incorrect File Type.\n");
		fprintf(f,"Incorrect File Type.");
		return;
	}

	if(strlen(name)>9)
	{
		if(restoring == 0)
		printf("Error: File or Directory Name exceeds max length.\n");
		fprintf(f,"\nError: File or Directory Name exceeds max length.");
		valid = 0;
		return;
	}

	/*Loop through the directory to check if the file or directory already exists*/
	do
	{
		for(cnt=0;cnt<31;cnt++)
		{
			if(strcmp(Sectors[find_cnt]->directory.DIR[cnt].NAME,name) == 0)
				if(Sectors[find_cnt]->directory.DIR[cnt].TYPE == 'D')
				{
					if(restoring == 0)
						printf("Error: Directory already exists.\n");
					fprintf(f,"\nError: Directory already exists.");
					valid = 0;
				}
				else
				{
					fn_delete(name);
				}
		}
		find_cnt = Sectors[find_cnt]->directory.FRWD;
	}while(NULL != Sectors[find_cnt]->directory.FRWD);

	fprintf(f,"\nValid: %d",valid);
	if((type == 'D' || type == 'U') && valid == 1 )
	{
		if(type == 'D')
		{
			sectornum = fn_delete_empty_sector();
			if(sectornum == 0)
			{
				printf("Out Of Memory. Cannot Create directory\n");
				return;
			}
			Sectors[sectornum] = (Directory *) malloc (sizeof(Directory));
		}
		else if(type == 'U')
		{
			sectornum = fn_delete_empty_sector();
			if(sectornum == 0)
			{
				printf("Out Of Memory. Cannot Create File\n");
				return;
			}
			Sectors[sectornum] = (File *) malloc (sizeof(File));
		}

		/*Point to the last sector of the directory*/
		while(NULL != Sectors[presentSec]->directory.FRWD)
		{
			presentSec = Sectors[presentSec]->directory.FRWD;
		}

		/*loop throught the present directory to find free space and insert the link to the new created file*/
		fprintf(f,"\nBefore looping FRWD: %d",Sectors[presentSec]->directory.FRWD);
		fprintf(f,"\nBefore looping Present Dir: %d",presentSec);
		for(cnt=0;cnt<31;cnt++)
		{
			if(Sectors[presentSec]->directory.DIR[cnt].TYPE == 'F')
			{
				Sectors[presentSec]->directory.DIR[cnt].TYPE = type;
				strcpy(Sectors[presentSec]->directory.DIR[cnt].NAME,name);
				Sectors[presentSec]->directory.DIR[cnt].LINK = sectornum;
				Sectors[presentSec]->directory.DIR[cnt].SIZE = NULL;
				created = 1;
				break;
			}
		}
		fprintf(f,"\ncreated: %d",created);
		if(created != 1)
		{
			fprintf(f,"\nPresent working sector if full. Creating a new sector");
			newSec = fn_delete_empty_sector();
			if(newSec == 0)
			{
				printf("Out Of Memory. Cannot Create another sector\n");
				return;
			}
			Sectors[newSec] = (Directory *) malloc (sizeof(Directory));
			Sectors[presentSec]->directory.FRWD=newSec;
			Sectors[newSec]->directory.BACK=presentSec;
			Sectors[newSec]->directory.FRWD=NULL;
			Sectors[newSec]->directory.FILLER[0]='\0';
			if(NULL != Sectors[presentSec]->directory.FREE)
			{
				Sectors[newSec]->directory.FREE = Sectors[presentSec]->directory.FREE;
			}
			else
			{
				Sectors[newSec]->directory.FREE=presentSec;
			}
			for(cnt=0;cnt<31;cnt++)
			{
				Sectors[newSec]->directory.DIR[cnt].TYPE='F';
				strcpy(Sectors[newSec]->directory.DIR[cnt].NAME,'\0');
				Sectors[newSec]->directory.DIR[cnt].LINK = NULL;
				Sectors[newSec]->directory.DIR[cnt].SIZE = NULL;
			}

			presentSec = newSec;
			Sectors[presentSec]->directory.DIR[0].TYPE = type;
			strcpy(Sectors[presentSec]->directory.DIR[0].NAME,name);
			Sectors[presentSec]->directory.DIR[0].LINK = sectornum;
			Sectors[presentSec]->directory.DIR[0].SIZE = NULL;
		}
		fprintf(f,"\nsectornum %d",sectornum);

		if(type == 'D')
		{
			Sectors[sectornum]->directory.BACK=NULL;
			Sectors[sectornum]->directory.FRWD=NULL;
			strcpy(Sectors[sectornum]->directory.FILLER,"FST\0");
			Sectors[sectornum]->directory.FREE=sectornum;
			for(cnt=0;cnt<31;cnt++)
			Sectors[sectornum]->directory.DIR[cnt].TYPE='F';
		}

		if(type == 'U')
		{
			strcpy(file_name,name);
			bytes = 0;
			fn_write(1);
		}
	}

	presentSec = Sectors[presentSec]->directory.FREE;
	fn_print_Dir_sector(presentSec);
	fprintf(f,"\nReturning from fn_create");
}

/*This function is used to delete a file or a directory*/
void fn_delete(char* file_name)
{
	int DelSector;
	int DelLink;
	int valid = 0;
	//scanf("%s",&name);
	fprintf(f,"\nIn fn_delete");
	while(NULL != Sectors[presentSec]->directory.BACK)
	{
		presentSec = Sectors[presentSec]->directory.BACK;
	}

	fprintf(f,"presentSec: %d",presentSec);
	do
	{
		for(cnt=0;cnt<31;cnt++)
		{
			if(strcmp(Sectors[presentSec]->directory.DIR[cnt].NAME,file_name) == 0)
				{
				   valid = 1;
				   DelLink = cnt;
				   break;
				}
		}
		presentSec = Sectors[presentSec]->directory.FRWD;
	}while(NULL != Sectors[presentSec]->directory.FRWD);

	fprintf(f,"\nvalid: %d",valid);
	if(valid == 1)
	{
		// future code based on question 5
		DelSector = Sectors[presentSec]->directory.DIR[cnt].LINK;
		if(Sectors[presentSec]->directory.DIR[cnt].TYPE == 'U')
		{
			do
			{
				Sectors[presentSec]->directory.DIR[DelLink].TYPE = 'F';
				fn_add_empty_sector(DelSector);
				DelSector = Sectors[DelSector]->file.FRWD;
			}while(NULL != Sectors[DelSector]->file.FRWD);
		}
		else if(Sectors[presentSec]->directory.DIR[cnt].TYPE == 'D')
		{
			for(cnt=0;cnt<31;cnt++)
			if(Sectors[DelSector]->directory.DIR[cnt].TYPE != 'F')
			{
				valid = 0;
				break;
			}
			if(valid == 1)
			{
				Sectors[presentSec]->directory.DIR[DelLink].TYPE = 'F';
				fn_add_empty_sector(DelSector);
			}
			else
			{
				if(restoring == 0)
					printf("Error: Can\'t delete. Directory is not empty.\n");
				fprintf(f,"\nError: Can\'t delete. Directory is not empty");
			}

		}
	}
	else
	{
		if(restoring == 0)
			printf("Error: Can\'t delete. File Dosen\'t exists.\n");
		fprintf(f,"\nError: Can\'t delete. File Dosen\'t exists.");
	}
	fn_print_Dir_sector(presentSec);
	fprintf(f,"\nReturning from fn_delete");
}

/*This finction is used to open a file*/
void fn_open()
{
	int find_cnt = presentSec;
	int valid = 0,frwd;
	fprintf(f,"\nIn fn_open");
	
	if(restoring == 0)
	{
		scanf(" %c",&mode);
		scanf("%s",&file_name);
		fprintf(file," %c",mode);
		fprintf(file," %s",file_name);
	}
	else
	{
		fscanf(file," %c",&mode);
		fscanf(file," %s",&file_name);
	}
	
	if(mode != 'I' && mode != 'O' && mode != 'U')
	{
		mode = NULL;
		strcpy(file_name,"\0");
		opened = 0;
		if(restoring == 0)
			printf("Please enter the correct mode.\n");
		fprintf(f,"Please enter the correct mode.\n");
		return;
	}
	fprintf(f,"printing mode %c file  %s",mode,file_name);
	if(NULL != Sectors[find_cnt]->directory.FREE)
	{
		find_cnt = Sectors[find_cnt]->directory.FREE;
	}
	do
	{
		for(cnt=0;cnt<31;cnt++)
		{
			if(strcmp(Sectors[find_cnt]->directory.DIR[cnt].NAME,file_name) == 0)
				if(Sectors[find_cnt]->directory.DIR[cnt].TYPE == 'U')
				{
				   valid = 1;
				   opened_link = Sectors[find_cnt]->directory.DIR[cnt].LINK;
				   file_pointer = 0;
				   break;
				}
		}
		frwd = Sectors[find_cnt]->directory.FRWD;
		find_cnt = Sectors[find_cnt]->directory.FRWD;
	}while(NULL != frwd);
	fprintf(f,"\nvalid %d",&valid);
	if(valid == 1)
	{
		if(restoring == 0)
			printf("\nFile \'%s\' Opened.\n",&file_name);
		fprintf(f,"\nFile \'%s\' Opened.\n",&file_name);
		opened = 1;
		valid = 1;
	}
	else
	{
		mode = NULL;
		strcpy(file_name,"\0");
		opened = 0;
		if(restoring == 0)
			printf("\nFile \'%s\' not found.\n",&file_name);
		fprintf(f,"\nFile \'%s\' not found.\n",&file_name);
	}
	fprintf(f,"\nReturning from fn_open");
}

/*This function is used to read data from a file*/
void fn_read()
{
	int find_cnt = opened_link;
	int valid = 0;
	int counter = 0;
	int pointer,i;
	char pString[504];
	fprintf(f,"\nIn fn_read");
	
	if(restoring == 0)
	{
		scanf("%d",&bytes);
		fprintf(file," %d",bytes);
	}
	else
	{
		fscanf(file," %d",&bytes);
	}
	
	if(restoring == 0)
		printf("\n");
	fprintf(f,"\nOpenrd Link: %d Pointer: %d",opened_link,file_pointer);
	if(opened == 0)
	{
		if(restoring == 0)
			printf("No File is opened.\n");
		fprintf(f,"\nNo File is opened.\n");
		return;
	}
	if(mode == 'I' || mode == 'U')
	{
		fprintf(f,"printing mode %c file  %s",mode,file_name);
		fprintf(f,"\nCounter: %d   %d",counter,file_pointer/504);
		while(counter != file_pointer/504)
		{
			find_cnt = Sectors[find_cnt]->file.FRWD;
			counter++;
		}
		pointer = file_pointer%504;
		fprintf(f,"\nCounter: %d   %d      %d",counter,pointer,find_cnt);
		fprintf(f,"\nPrinting file: %s\n",Sectors[find_cnt]->file.USER_DATA);
		//strcpy(pString,Sectors[find_cnt]->file.USER_DATA);
		for(i=0;i<bytes;i++)
		{
			if(restoring == 0)
				printf("%c",Sectors[find_cnt]->file.USER_DATA[pointer]);
			if(Sectors[find_cnt]->file.USER_DATA[pointer] == '\0')
			{
				if(restoring == 0)
					printf("\nEnd of File has reached.\n");
				fn_size();
				fprintf(f,"\nEnd of File has reached.\n");
				break;
			}
			pointer++;
		}
		if(restoring == 0)
			printf("\n");
		file_pointer = file_pointer + bytes;
	}
	else
	{
		if(restoring == 0)
			printf("Not in Input or update Mode.");
		fprintf(f,"Not in Input or update Mode.");
	}
	fprintf(f,"\nReturning from fn_read");
}

/*This function is used to seek the pointer*/
void fn_seek()
{
	int find_cnt = opened_link;
	int valid = 0;
	int counter = 1;
	fprintf(f,"\nIn fn_seek");
	
	if(restoring == 0)
	{
		scanf("%d",&base);
		scanf("%d",&offset);
		fprintf(file," %d",base);
		fprintf(file," %d",offset);
	}
	else
	{
		fscanf(file," %d",&base);
		fscanf(file," %d",&offset);
	}
	
	fprintf(f,"\nOpenrd Link: %d Pointer: %d",opened_link,file_pointer);
	if(opened == 0)
	{
		if(restoring == 0)
			printf("No File is opened.\n");
		fprintf(f,"No File is opened.\n");
		return;
	}
	else
	{
		if(mode == 'I' || mode == 'U')
		{
			if(base == -1)
			{
				if(offset >= 0)
				file_pointer = offset;
				else
				file_pointer=0;
			}
			else if(base == 0)
			{
				if(file_pointer + offset >= 0)
				file_pointer = file_pointer + offset;
				else
				file_pointer=0;
			}
			else if(base == 1)
			{
				while(NULL != Sectors[find_cnt]->file.FRWD)
				{
					find_cnt = Sectors[find_cnt]->file.FRWD;
					counter++;
				}
				if(offset <= 0)
				file_pointer = (strlen( Sectors[find_cnt]->file.USER_DATA) - 1) + offset;
				else
				file_pointer = (strlen( Sectors[find_cnt]->file.USER_DATA) - 1);
			}
		}
	}
}

/*This finction is used to write data into file*/
void fn_write(int new)
{
	int len,len1,i,file_counter,new_Sec;
	int valid = 0;
	int prsntSec;
	int firstSec;
	//int new_bytes;
	char str[10000];
	char str1;//[10000];
	/*char *str;
	char *str1;*/
	char str2 = '\"';

	strcpy(str,"\0");
	//strcpy(str1,"\0");
	fprintf(f,"In fn_write");
	if(opened == 0 && new != 1)
	{
		if(restoring == 0)
			printf("No File is opened.\n");
		fprintf(f,"No File is opened.\n");
		return;
	}
	if(mode == 'O' || mode == 'U' || new == 1)
	{
		fprintf(f,"\n in new else present sector: %d COUNT: %d",presentSec,cnt);
		fprintf(f,"\n in new else present sectornum: %d Counter: %d",sectornum,file_counter);
		if(bytes == 0)
		{
			if(restoring == 0)
				printf("Enter the number of characters you want to enter: ");
			
			if(restoring == 0)
			{
				scanf("%d",&bytes);
				fprintf(file," %d",bytes);
			}
			else
			{
				fscanf(file," %d",&bytes);
			}
			
			if(restoring == 0)
			{
				printf("\nEnter data to be read into file in double quotes: \n");
			}
		}

		/*str = malloc(bytes);
		str1 = malloc(bytes);*/


		file_counter=0;
		if(restoring == 0)
		{
			scanf(" %c",&str1);
			fprintf(file," %c",str1);
		}
		else
		{
			fscanf(file," %c",&str1);
		}
		fprintf(f,"\nScaned: %c",str1);
		do
		{
			/*len1=strlen(str1);
			for(i=0;i<len1;i++)
			{
				str[file_counter] = str1[i];
				file_counter++;
			}
			str[file_counter] = ' ';
			file_counter++;
			scanf("%s",str1);*/
			
			if(restoring == 0)
			{
				scanf("%c",&str1);
				fprintf(file,"%c",str1);
			}
			else
			{
				fscanf(file,"%c",&str1);
			}
			fprintf(f,"\nScaned: %c",str1);
			str[file_counter] = str1;
			file_counter++;
		}while(str1 != '\"');
		len = strlen(str);
		file_counter--;
		while(len < bytes)
		{
			str[file_counter] = ' ';
			file_counter++;
			len++;
		}
		str[file_counter] = '\0';
		fprintf(f,"\nLength of string to be read: %d\n%s\n",len,str);
		if (len > 0 )//&& str[len-1] == '\n')
		{
			//str[len-1] = '\0';
			if(new == 1)
			{
				Sectors[presentSec]->directory.DIR[cnt].SIZE = ((len%504) + (len/504));
				file_counter=0;
				firstSec = sectornum;
			}
			else
			{
				fprintf(f,"\n in new else present sector: %d ",Sectors[presentSec]->directory.FREE);
				if(NULL != Sectors[presentSec]->directory.FREE)
				presentSec = Sectors[presentSec]->directory.FREE;
				prsntSec = presentSec;
				//strcpy(file_name,"FILE");
				do
				{
					for(cnt=0;cnt<31;cnt++)
					{
						fprintf(f,"\n in new else Name: %s",Sectors[prsntSec]->directory.DIR[cnt].NAME);
						if(strcmp(Sectors[prsntSec]->directory.DIR[cnt].NAME,file_name) == 0)
						{
							valid = 1;
							break;
						}
					}
					prsntSec = Sectors[prsntSec]->directory.FRWD;
					/*if(valid == 1)
					break;*/
				}while(NULL != Sectors[prsntSec]->directory.FRWD);

				file_counter =Sectors[prsntSec]->directory.DIR[cnt].SIZE - 1;
				Sectors[prsntSec]->directory.DIR[cnt].SIZE = (((Sectors[prsntSec]->directory.DIR[cnt].SIZE + len)%504) + (len/504));
				sectornum = Sectors[prsntSec]->directory.DIR[cnt].LINK;
				firstSec = sectornum;
				fprintf(f,"\n in new else present sector: %d COUNT: %d",prsntSec,cnt);
				fprintf(f,"\n in new else present sectornum: %d Counter: %d",sectornum,file_counter);
				while(NULL != Sectors[sectornum]->file.FRWD)
				{
					sectornum = Sectors[sectornum]->file.FRWD;
				}
			}
			i=0;
			while(i<len)
			{
				if(i%504 == 0 && i != 0)
				{
					fprintf(f,"\nin if i= %d,  file_counter = %d ",i,file_counter);
					Sectors[sectornum]->file.USER_DATA[file_counter] = '\0';
					new_Sec = fn_delete_empty_sector();
					if(new_Sec == 0)
					{
						printf("Out Of Memory. Cannot append the file\n");
						return;
					}
					Sectors[new_Sec] = (File *) malloc (sizeof(File));
					Sectors[sectornum]->file.FRWD = new_Sec;
					Sectors[new_Sec]->file.BACK=sectornum;
					Sectors[new_Sec]->file.FRWD=NULL;
					sectornum = new_Sec;
					Sectors[sectornum]->file.USER_DATA[0] = str[i];
					file_counter = 1;
					i++;

				}
				else
				{
					fprintf(f,"\nin else i= %d,  file_counter = %d ",i,file_counter);
					Sectors[sectornum]->file.USER_DATA[file_counter] = str[i];
					i++;
					file_counter++;
				}
			}
		}
		fprintf(f,"Bytes: %d length: %d MOD: %d\n",bytes,len,bytes%len);
		/*if((bytes-len != 0) && (bytes > len))
		{
			fprintf(f," coming for spaces");
			for(i=file_counter;i<504;i++)
			{
				Sectors[sectornum]->file.USER_DATA[file_counter] = ' ';
			}
			Sectors[sectornum]->file.USER_DATA[504] = '\0';
		}*/
		/*free(str);
		free(str1);*/
		//fn_print_file(firstSec);
	}
	else
	{
		if(restoring == 0)
			printf("Not in Output or update Mode.");
		fprintf(f,"Not in Output or update Mode.");
	}
	fprintf(f,"\nReturning from fn_write");
}

/*This is used to print the list of files*/
void fn_list()
{
	int prsntSec = presentSec;
	int frwd;
	fprintf(f,"\nIn fn_list");
	if(restoring == 0)
		printf("\nNAME\tTYPE");
	
	if(NULL != Sectors[prsntSec]->directory.FREE)
	{
		prsntSec = Sectors[prsntSec]->directory.FREE;
	}
	do
	{
		for(cnt=0;cnt<31;cnt++)
		{
			if(Sectors[prsntSec]->directory.DIR[cnt].TYPE != 'F')
				{
				   if(restoring == 0)
						printf("\n%s\t%c",Sectors[prsntSec]->directory.DIR[cnt].NAME,Sectors[prsntSec]->directory.DIR[cnt].TYPE);
				}
		}
		frwd = Sectors[prsntSec]->directory.FRWD;
		prsntSec = Sectors[prsntSec]->directory.FRWD;
	}while(NULL != frwd);
	if(restoring == 0)
		printf("\n");
	fprintf(f,"\nReturning from fn_list");
}

/*This function is used to find the size of the opened file*/
void fn_size()
{
	int prsntSec = presentSec;
	int counter = 0;
	int size =0;
	if(opened == 0)
	{
		if(restoring == 0)
			printf("No File is opened.\n");
		fprintf(f,"\nNo File is opened.\n");
		return;
	}
	do
	{
		for(cnt=0;cnt<31;cnt++)
		{
			if(strcmp(Sectors[prsntSec]->directory.DIR[cnt].NAME,file_name) == 0)
			{
				if(Sectors[prsntSec]->directory.DIR[cnt].TYPE == 'U')
				{
					counter = 0;
				   while(NULL != Sectors[sectornum]->file.FRWD)
					{
						sectornum = Sectors[sectornum]->file.FRWD;
						counter++;
					}
					size = Sectors[prsntSec]->directory.DIR[cnt].SIZE + (504 * counter);
					if(restoring == 0)
						printf("File Size is: %d bytes\n",size);
				}
				if(Sectors[prsntSec]->directory.DIR[cnt].TYPE == 'D')
				{
					if(restoring == 0)
						printf("Directory file can\'t be Determined\n");
				}
			}
		}
		prsntSec = Sectors[prsntSec]->directory.FRWD;
	}while(NULL != Sectors[prsntSec]->directory.FRWD);
	if(restoring == 0)
		printf("\n");
}

/*This function is used to change directory*/
void fn_change_dir()
{
	int prsntSec = presentSec;
	int cntSec, frwd;
	int valid =0;
	fprintf(f,"\nIn fn_change_dir");
	
	if(restoring == 0)
	{
		scanf("%s",&dir_name);
		fprintf(file," %s",dir_name);
	}
	else
	{
		fscanf(file," %s",&dir_name);
	}
	if(strcmp(dir_name,"..") !=0)
	{
		if(NULL != Sectors[prsntSec]->directory.FREE)
		{
			prsntSec = Sectors[prsntSec]->directory.FREE;
		}
		
		do
		{
			for(cntSec=0;cntSec<31;cntSec++)
			{
				if(strcmp(Sectors[prsntSec]->directory.DIR[cntSec].NAME,dir_name) == 0 && Sectors[prsntSec]->directory.DIR[cntSec].TYPE == 'D')
					{
					   presentSec = Sectors[prsntSec]->directory.DIR[cntSec].LINK;
					   fn_open_dir(Sectors[prsntSec]->directory.DIR[cntSec].LINK);
					   valid = 1;
					   break;
					}
			}
			frwd = Sectors[prsntSec]->directory.FRWD;
			prsntSec = Sectors[prsntSec]->directory.FRWD;
		}while(NULL != frwd);
		if(valid == 0)
		{
			if(restoring == 0)
				printf("\nERROR: No such Directory");
			fprintf(f,"\nERROR: No such Directory");
		}
	}
	else if(strcmp(dir_name,"..") ==0)
	{
		presentSec = fn_close_dir();
		fprintf(f,"Returnrd Dir is: %d",presentSec);
	}
	//fn_display_path();
	//fprintf(f,"Printing Directory of sector: ",rear_dir->info);
	//fn_print_Dir_sector(rear_dir->info);
	fprintf(f,"\nReturning from fn_change_dir");
}

/* Create a queue with list of empty sectors */
void fn_create_list_of_Sectors()
{
    int i;
	fprintf(f,"\nIn fn_create_list_of_Sectors");
	for(i=MAX-1; i >0 ; i--)
	{
		//fn_add_empty_sector(i);
		if (front >= MAX)
		fprintf(f,"\nMemory Overflow \n");
		else
		{
			if (rear == -1)
			rear = 0;
			front = front + 1;
			LIST[front] = i;
		}
	}
	fprintf(f,"\nReturning from fn_create_list_of_Sectors");
}

/* Adding a sector number which got empty to the queue */
void fn_add_empty_sector(int data)
{
	if (front >= MAX)
    fprintf(f,"\nMemory Overflow \n");
    else
    {
		if (rear == -1)
		rear = 0;
		front = front + 1;
		LIST[front] = data;
    }
	fprintf(f,"\nAdded Sector %d",LIST[front]);
}

/*Delete a sector number from the list and allocate memory to it*/
int fn_delete_empty_sector()
{
	int i;
	fprintf(f,"\nIN fn_delete_empty_sector");
	i=LIST[front];
    if(front == -1)
	{
		return 0;
		//printf("Memory Overflow. No More memory Space");
	}
	else if(front == rear)
	{
		front = -1;
		rear  = -1;
	}
	else
		front--;
	fprintf(f,"\nReturning from fn_delete_empty_sector");
	return i;
}

/*Print list of empty sectors*/
void fn_print_empty_sectors()
{
	int i;
	fprintf(f,"\nIn fn_print_empty_sectors");
	for(i=front;i >= rear;i--)
		fprintf(f,"%d ",LIST[i]);
	fprintf(f,"\nReturning from fn_delete_empty_sector");
}

/*This function is used to change the directory by opening it*/
void fn_open_dir(int data)
{
	fprintf(f,"\nIn fn_open_dir");
    if (rear_dir == NULL)
    {
        rear_dir = (struct list *)malloc(1*sizeof(struct list));
        rear_dir->prev = NULL;
		rear_dir->next = NULL;
        rear_dir->info = data;
        front_dir = rear_dir;
    }
    else
    {
        temp=(struct list *)malloc(1*sizeof(struct list));
        rear_dir->next = temp;
        temp->info = data;
        temp->prev = rear_dir;
		temp->next = NULL;

        rear_dir = temp;
    }
    fprintf(f,"Added Dir: %d",rear_dir->info);
	fprintf(f,"\nReturning from fn_open_dir");
}

/*This function is used to change the directory by closing it*/
int fn_close_dir()
{
    int return_sec = 0;
	front_dir1 = rear_dir;

	fprintf(f,"\nIn fn_close_dir");
    if (front_dir1 == NULL)
    {
        fprintf(f,"\n Error: Trying to display elements from empty queue");
        return 0;
    }
    else
        if (front_dir1->prev != NULL)
        {
            front_dir1 = front_dir1->prev;
            fprintf(f,"\n Dequed value : %d", rear_dir->info);
			
            free(rear_dir);
			front_dir1->next = NULL;
            rear_dir = front_dir1;
			return_sec = rear_dir->info;
        }
        else
        {
            fprintf(f,"\n Dequed value IN else : %d", rear_dir->info);
			return_sec = rear_dir->info;
            /*free(rear_dir);
            front_dir = NULL;
            rear_dir = NULL;*/
        }

	fprintf(f,"\nReturning from fn_close_dir %d",return_sec);
	return return_sec;
}

/*This function is used to display the current path*/
void fn_display_path()
{
    int prsntSec = 0;
	int cntSec;
	int valid =1;
	front_dir1 = front_dir;

	fprintf(f,"\nIn fn_display_path\n");
    if ((front_dir1 == NULL) && (rear_dir == NULL))
    {
        fprintf(f,"Queue is empty");
        return;
    }
    /*while (front_dir1 != rear_dir)
    {
        fprintf(f,"%d/", front_dir1->info);
        front_dir1 = front_dir1->next;
    }
    if (front_dir1 == rear_dir)
        fprintf(f,"%d/", front_dir1->info);*/

	fprintf(f,"\nPrinting actual path: \n");
	fprintf(f,"/root/");
	if(restoring == 0)
		printf("/");
	while (NULL != front_dir1->next)
    {
		//Sectors[prsntSec]->directory.DIR[cntSec].LINK
		do
		{
			fprintf(f,"\nfront_dir1->info %d",front_dir1->info);
			fprintf(f,"\nfront_dir1->info %d",front_dir1->next->info);
			fprintf(f,"prsntSec: %d",prsntSec);
			if(prsntSec != front_dir1->info && valid == 1)
			{
				prsntSec = front_dir1->info;
				front_dir1 = front_dir1->next;
				valid = 0;
			}
			fprintf(f,"prsntSec: %d",prsntSec);
			if(front_dir1->info != 0)
			{
				for(cntSec=0;cntSec<31;cntSec++)
				{
					fprintf(f,"\nPrinting links %d",Sectors[prsntSec]->directory.DIR[cntSec].LINK);
					if(Sectors[prsntSec]->directory.DIR[cntSec].LINK == front_dir1->info && Sectors[prsntSec]->directory.DIR[cntSec].TYPE == 'D')
						{
						   if(restoring == 0)
								printf("%s/", Sectors[prsntSec]->directory.DIR[cntSec].NAME);
						   fprintf(f,"%s/", Sectors[prsntSec]->directory.DIR[cntSec].NAME);
						   //valid = 1;
						   break;
						}
				}
			}
			else
			{
				for(cntSec=0;cntSec<31;cntSec++)
				{
					fprintf(f,"\n Printing links %d",Sectors[prsntSec]->directory.DIR[cntSec].LINK);
					if(Sectors[prsntSec]->directory.DIR[cntSec].LINK == front_dir1->next->info && Sectors[prsntSec]->directory.DIR[cntSec].TYPE == 'D')
						{
						   if(restoring == 0)
								printf("%s/", Sectors[prsntSec]->directory.DIR[cntSec].NAME);
						   fprintf(f,"%s/", Sectors[prsntSec]->directory.DIR[cntSec].NAME);
						   //valid = 1;
						   break;
						}
				}
			}
			prsntSec = Sectors[prsntSec]->directory.FRWD;
		}while(NULL != Sectors[prsntSec]->directory.FRWD);
        //fprintf(f,"%d/", front_dir1->info);
		prsntSec = front_dir1->info;
        front_dir1 = front_dir1->next;
    }
    if (front_dir1 == rear_dir)
        fprintf(f,"%d/", front_dir1->info);
}

/*Print list of empty sectors*/
void fn_print_Dir_sector(int prsntSec)
{
	int i,frwd;
	int prsntSec1 = prsntSec;
	
	do
	{
		fprintf(f,"\n\n\t\t%d",Sectors[prsntSec1]->directory.BACK);
		//fprintf(f,"\n_________________________");
		fprintf(f,"\n\t\t%d",Sectors[prsntSec1]->directory.FRWD);
		//fprintf(f,"\n_________________________");
		fprintf(f,"\n\t\t%d",Sectors[prsntSec1]->directory.FREE);
		//fprintf(f,"\n_________________________");
		fprintf(f,"\n\t\t%s",Sectors[prsntSec1]->directory.FILLER);
		//fprintf(f,"\n_________________________");
	
		for(cnt=0;cnt<31;cnt++)
		{
			fprintf(f,"\n\t|%c|",Sectors[prsntSec1]->directory.DIR[cnt].TYPE);
			fprintf(f,"\t%s|",Sectors[prsntSec1]->directory.DIR[cnt].NAME);
			fprintf(f,"\t%d|",Sectors[prsntSec1]->directory.DIR[cnt].LINK);
			fprintf(f,"\t%d|",Sectors[prsntSec1]->directory.DIR[cnt].SIZE);
			//fprintf(f,"\n_________________________");
		}
		frwd = Sectors[prsntSec1]->directory.FRWD;
		prsntSec1 = Sectors[prsntSec1]->directory.FRWD;
	}while(NULL != frwd);
}

void fn_print_file(int Sector_num)
{
	//sectornum = Sectors[presentSec]->directory.DIR[cnt].LINK;
	fprintf(f,"Printing Sector number: %d",Sector_num);
	do
	{
		fprintf(f,"\n%s",Sectors[Sector_num]->file.USER_DATA);
		fprintf(f,"\n--------------------------------------------\n");
		sectornum = Sectors[Sector_num]->file.FRWD;
	}while(NULL != Sectors[Sector_num]->file.FRWD);
}

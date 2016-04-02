#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define LEN sizeof(struct commandHistory)
#define LEN1 sizeof(struct savePath)

struct commandHistory
{
    int iOrder;
    char *strDisplay_name;
    char *strDirectory;
    struct commandHistory *next;
};

struct savePath
{
    char *path_name;
    struct savePath *next;
};


/* Function prototypes */
char * extractRoute_cd(char *, const char *, int);
char * extractRoute_path(char *, const char *, int);
struct commandHistory * create_commandHistroy(int, char *, char *);
void display_History(struct commandHistory *);
int clear_History(struct commandHistory*);
struct commandHistory *  write_History(int , char *, char *, struct commandHistory *);
void display_singleHistory(int, int, struct commandHistory *);
struct savePath * create_path_head(char *);
void write_Path(int, char *, struct savePath *);
struct savePath * delete_path(char *, struct savePath *);
void display_path(struct savePath *);
int execu_tion(char *);



int main(int argc, const char * argv[]) {
    char *strCommand_name[6] = { "exit","cd ","pwd","path","history"}; //add a " " to each command so that the program can know if the format is wrong
    char *strUser_command1;          //for saving user's input
    char strHistory_offset1[10] = "history -c";     //for "strcmp" to find the offset
    char strUser_command[100];
    char strNew_directory[100] = {0};         //for saving user's file route
    char strOffset_empty[8] = {"    "};         //"pwd" command does not have offset
    int iDigit=0;                          //to record the command's number of character
    int irecordNumber = 0;                 //to record which record "history -[num]" wants to look for
    int iNumber = 0;     // which command was executed? 0 means "exit ";1 means "cd ",....
    int iTotal_execution = 0; //to record how many commands were executed.
    struct commandHistory * head = NULL;  //use a chain to record command history
    struct savePath * head_path = NULL;
    char *strCommand_extract_cd = NULL;
    char *strCommand_extract_history = NULL;
    char *strCommand_extract_path = NULL;
    
    char *token=NULL;
    
    int iNumber_path = 0;
    char buf[80];
    while (1)
    {
        char str1[100]="+ ";
        char str2[100]="- ";
        char *pathDirectory = NULL;
        printf("\n$");
        strUser_command1 = (char *)malloc(100 *sizeof(char));
        fgets(strUser_command1,100,stdin);
        strcpy(strUser_command, strUser_command1);
        
        iDigit = ((int)strlen(strUser_command))-1;
        //printf("\n%d---digit",iDigit);
        strUser_command[iDigit]='\0';
        free(strUser_command1);
        //if (head_path!=NULL)
        //{
        
        if(strUser_command[0]=='/'||(strUser_command[0]=='.'&&strUser_command[1]=='/')) //if the input is a
        {
            int status;
            status = execu_tion(strUser_command);
            continue;
        }
        
        
        
        else if (strncmp(strUser_command, strCommand_name[1],3) == 0) //if the command is "cd ..."
        {
            strCommand_extract_cd = (char *)malloc(iDigit *sizeof(char));
            strCommand_extract_cd = extractRoute_cd(strCommand_extract_cd, strUser_command, iDigit);
            iNumber = 1;
            iTotal_execution = iTotal_execution + 1;
            strcat(strNew_directory, strCommand_extract_cd);
            
            
            
        }
        else if (strncmp(strUser_command, strCommand_name[2],3) == 0) //if the command is "pwd"
        {
            iNumber = 2;
            iTotal_execution = iTotal_execution + 1;
        }
        else if (strncmp(strUser_command, strCommand_name[0], 4) == 0) //if the command is "exit"
        {
            iNumber = 0;
            //iTotal_execution = iTotal_execution + 1;
        }
        else if (strncmp(strUser_command, strCommand_name[3], 4) == 0) //if the command is "path"
        {
            
            
            //printf("\n%s---extract",strCommand_extract_path);
            // printf("judge");
            iNumber = 3;
            iTotal_execution = iTotal_execution + 1;
        }
        else if (strncmp(strUser_command, strCommand_name[4], 7) == 0) //if the command is "history" (with offset)
        {
            iNumber = 4;
            //iTotal_execution = iTotal_execution + 1;
        }
        
        else
        {
            printf("\nerror:%%your error message\n");
        }
        
        switch (iNumber)
        {
            case 0:
                exit(0);
                break;
            case 1:  //if "cd xxx" was executed
                if (iTotal_execution==1)
                {
                    head = create_commandHistroy(iTotal_execution, strCommand_name[1], strCommand_extract_cd); //if the first command is "cd", create the head and get the address.
                    errno = 0;
                    chdir(strCommand_extract_cd);
                    if (errno != 0)
                        printf("error:%s\n",strerror(errno));
                }
                
                if (iTotal_execution>1)
                {
                    head= write_History(iTotal_execution, strCommand_name[1],strCommand_extract_cd, head);
                    
                    errno = 0;
                    chdir(strCommand_extract_cd);
                    if (errno != 0)
                        printf("error:%s\n",strerror(errno));
                    
                    
                    
                }
                //free(strCommand_extract_cd);
                
                
                break;
            case 2: // pwd
                if (iTotal_execution==1)
                {
                    head = create_commandHistroy(iTotal_execution, strCommand_name[2], strOffset_empty);
                    getcwd(buf, sizeof(buf));
                    printf("\n%s", buf);
                }
                if (iTotal_execution>1)
                {
                    head = write_History(iTotal_execution, strCommand_name[2],strOffset_empty, head);
                    getcwd(buf, sizeof(buf));
                    printf("\n%s", buf);
                }
                break;
            case 3: //path
                
                
                // printf("\nentrence");
                strCommand_extract_path = (char *)malloc(iDigit *sizeof(char));
                strCommand_extract_path = extractRoute_path(strCommand_extract_path, strUser_command,iDigit);
                if (strUser_command[5]=='+'&&strUser_command[4]==' '&&strUser_command[6]==' '&&((strUser_command[7]=='/')||(strUser_command[7]=='.'))) //when user use command "path +"
                {
                    iNumber_path = iNumber_path + 1;
                    
                    //printf("\n%s---extract",strCommand_extract_path);
                    pathDirectory = strcat(str1,strCommand_extract_path);
                    if (iTotal_execution==1) //it's the first time to execute command, so head should be created first.
                    {
                        
                        head = create_commandHistroy(iTotal_execution, strCommand_name[3], pathDirectory);
                        errno = 0;
                        if (errno != 0)
                            printf("error:%s\n",strerror(errno));
                    }
                    if  (iTotal_execution>1) //it's not the first time to execute command
                    {
                        head = write_History(iTotal_execution, strCommand_name[3],pathDirectory, head);
                        errno = 0;
                        if (errno != 0)
                            printf("error:%s\n",strerror(errno));
                    }
                    
                    
                    if (iNumber_path==1)     //it's the first time to add path, so head_Path should be created first
                    {
                        head_path= create_path_head(strCommand_extract_path);
                        errno = 0;
                        if (errno != 0)
                            printf("error:%s\n",strerror(errno));
                    }
                    if (iNumber_path>1)   //it's not the first time to add path
                    {
                        write_Path(iNumber_path, strCommand_extract_path, head_path);
                        errno = 0;
                        if (errno != 0)
                            printf("error:%s\n",strerror(errno));
                        
                    }
                    
                }
                else if(strUser_command[5]=='-'&&strUser_command[4]==' '&&strUser_command[6]==' '&&((strUser_command[7]=='/')||(strUser_command[7]=='.')))  //when use command "path -"
                {
                    
                    //printf("\n%s---extract",strCommand_extract_path);
                    pathDirectory = strcat(str2,strCommand_extract_path);
                    
                    if (iNumber_path>1)
                    {
                        iNumber_path = iNumber_path - 1;
                        head_path = delete_path(strCommand_extract_path,head_path);
                    }
                    else
                    {
                        printf("\nerror:%%now there is no path record ");
                    }
                    if (iTotal_execution==1) //it's the first time to execute command, so head should be created first.
                    {
                        
                        head = create_commandHistroy(iTotal_execution, strCommand_name[3], pathDirectory);
                        errno = 0;
                        if (errno != 0)
                            printf("error:%s\n",strerror(errno));
                        
                    }
                    if (iTotal_execution>1)   //it's not the first time to execute command
                    {
                        
                        head = write_History(iTotal_execution, strCommand_name[3], pathDirectory, head);
                        errno = 0;
                        if (errno != 0)
                            printf("error:%s\n",strerror(errno));
                    }
                    
                }
                else if(strUser_command[4]=='\0')   //if user input "path"
                {
                    //printf("here");
                    if (iTotal_execution==1) //it's the first time to execute command, so head should be created first.
                    {
                        
                        head = create_commandHistroy(iTotal_execution, strCommand_name[3], strOffset_empty);
                        printf("\nerror:%%\\nyour error message, there is no path record");
                        
                    }
                    if (iTotal_execution>1) //it's not the first time to execute command
                    {
                        head = write_History(iTotal_execution, strCommand_name[3], strOffset_empty, head);
                        errno = 0;
                        if (errno != 0)
                        {
                          printf("error:%s\n",strerror(errno));
                        }
                        
                        display_path(head_path);
                    }
                    
                }
                else {printf("\nerror:%%\\n, your error message\nthe history is\n");}
                
                break;
            case 4: //history
               
                if (strUser_command[7]=='\0')
                    display_History(head);
                else if (strUser_command[7]==' ')
                {
                    if (strncmp(strUser_command, strHistory_offset1, 10)==0)
                    {
                        iTotal_execution = 0;
                        head = NULL;
                    }
                    else if ((strUser_command[7]==' ')&&((isdigit(strUser_command[8])||(isdigit(strUser_command[8])&&isdigit(strUser_command[9]))||(isdigit(strUser_command[8])&&isdigit(strUser_command[9])&&isdigit(strUser_command[10])))))
                    {
                        strCommand_extract_history = (char *)malloc(iDigit *sizeof(char));
                        strCommand_extract_history = strUser_command;
                        token = strsep(&strCommand_extract_history, " ");
                        irecordNumber = atoi(strCommand_extract_history);
                        display_singleHistory(irecordNumber, iTotal_execution, head);
                    }
                     else printf("\nerror:%%\\n, your error message\n");
                   
                }
                break;
                
                
        }
    }
    
    
    
}



/*******************************************************************************************************
 FUNCTION DEFINITION
 ******************************************************************************************************/

char * extractRoute_cd(char *dest, const char *src, int n)
{
    int i;
    
    for (i=0;i<n&&src[i]!='\0';i++)
    {
        dest[i]=src[i+3];
    }
    dest[n]='\0';
    dest[n-1] ='\0';
    return dest;
}



char * extractRoute_path(char *dest, const char *src, int n)
{
    int i;
    for (i=0;i<n&&src[i]!='\0';i++)
    {
        dest[i]=src[i+7];
    }
    dest[n]='\0';
    //dest[n-1] ='\0';
    return dest;
}




//  x means how many commands were executed, y means which kind of command was put in to record.
//   z means the directory in the command (if it exists.)

struct commandHistory * create_commandHistroy(int x, char *y, char *z)
{
    struct commandHistory * head = NULL;
    struct commandHistory * p1;
    p1 = (struct commandHistory *)malloc(LEN);
    p1->iOrder = x-1;
    p1->strDisplay_name = y;
    p1->strDirectory = z;
    head=p1;
    
    return head;
}

// x means the total number of executed command, y means which kind of command was put in to record,
//z means the directory in the command (if it exists.)
//head is the first address of the chain

struct commandHistory * write_History(int x, char *y, char *z, struct commandHistory * head)
{
    int j=0;
    struct commandHistory * p1, *p2, *p3;
    p2 = p3 = head;
    p1 = (struct commandHistory *)malloc(LEN);
    p1->iOrder = x-1;
    p1->strDisplay_name = y;
    p1->strDirectory = z;
    p2 = p1;
    for (int i=1; i<=x; i++)
    {
        if (p3->next==NULL)
        {
            p3->next=p2;
            p2->next=NULL;
        }
        else p3=p3->next;
    }
    if (x==101)
    {
        p2 = p3 = head;
        head = head->next;
        free(p2);
        for(j=0;j<100;j++)
        {
            p3->iOrder = j;
            
            if (j == 99)
                p3->next = NULL;
            else
                p3=p3->next;
        }
        
    }
    return head;
    
    
}


void display_History(struct commandHistory * head)
{
    struct commandHistory * p;
    p = head;
    if (head == NULL)
        printf("\nNo record");
    else
    {
        while(p != NULL)
        {
            if (p->iOrder<10)
                printf("%d        %s        %s\n",p->iOrder,p->strDisplay_name, p->strDirectory);
            else if ((p->iOrder)>9)
                printf("%d       %s        %s\n",p->iOrder,p->strDisplay_name, p->strDirectory);
            else
                printf("%d      %s        %s\n",p->iOrder,p->strDisplay_name, p->strDirectory);
            p = p->next;
        }
    }
    head = NULL;
    
    
}



int clear_History(struct commandHistory * head)
{
    struct commandHistory * p = NULL;
    while(head!=NULL)
    {
        p = head;
        head = head->next;
        free(p);
    }
    //head = (struct commandHistory *)malloc(LEN);
    head = NULL;
    return 1;
}


void display_singleHistory(int x, int y, struct commandHistory * head)
{
    if (x <= y )
    {
        struct commandHistory * p;
        p = head;
        
        while (p->iOrder!=(x-1))
        {
            p = p->next;
        }
        printf("%s        %s", p->strDisplay_name, p->strDirectory);
    }
    else
        printf("\nerror:%%\\n, your error message");
}

struct savePath * create_path_head(char *y)
{
    struct savePath * head = NULL;
    struct savePath * p1;
    p1 = (struct savePath *)malloc(LEN1);
    p1->path_name = y;
    head=p1;
    //printf("\n%s----head",head->path_name);
    
    return head;
}

void write_Path(int x, char *y, struct savePath * head)
{
    struct savePath * p1, *p2, *p3;
    p2 = p3 = head;
    p1 = (struct savePath *)malloc(LEN1);
    p1->path_name = y;
    p2 = p1;
    for (int i=1; i<x; i++)
    {
        if (p3->next==NULL)
        {
            p3->next=p2;
            p2->next=NULL;
            // printf("\n%s---%dnd",p2->path_name,x);
        }
        else p3=p3->next;
    }
}

struct savePath * delete_path(char *y, struct savePath * head)
{
    struct savePath *p, *p1=NULL;
    if (strcmp(y, head->path_name)==0&&head != NULL)
    {
        if (head->next!=NULL)
        {
            p=head;
            head=head->next;
            free(p);
            return head;
        }
        else
        {
            p=head;
            free(p);
            return NULL;
            
        }
    }
    else
    {
        p=head;
        
        
        while (strcmp(y,p->path_name)!=0&&(p!=NULL))
        {
            p1 = p;
            p=p->next;
        }
    }
    if (p==NULL)
    {
        printf("\nerror:%%\\n, your error message");
    }
    else
    {
        p1->next = p->next;
        free(p);
    }
    return head;
    
}



void display_path(struct savePath * head)
{
    struct savePath * p=NULL;
    p = head;
    if (p==NULL)
    {
        printf("\nNo path record");
    }
    else
    {
        while(p != NULL)
        {
            printf(" %s:",p->path_name);
            p = p->next;
        }
    }
}

int execu_tion(char *path)
{
    int status,i=0;
    char *strPath = NULL, *p=NULL;
    p = path;
    strPath = strsep(&p," ");
    char *argv[]={strPath,p,0};
    pid_t child_pid = fork();
    pid_t w;
    if (child_pid==0)
    {
        i=execvp(argv[0], argv);
        return i;
    }
    else if (child_pid < 0)
    {
        perror("fork");
        //exit(EXIT_FAILURE);
    }
    else
    {
        do
        {
            w = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
            if (w == -1)
            {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        
        
    }
    
    if (i<=0)
        printf("error:%s\n",strerror(errno));
    // exit(EXIT_SUCCESS);
    return i;
}

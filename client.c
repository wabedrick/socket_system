#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

#define MAXIMUM 50

typedef struct Patients
{
    char patientName[100];
    char patientStatus[30];
    char patientGender[10];
    char date[50];

} patient; 

char districtName[30];
char officerName[100];
char hospitalName[100];

FILE *patientsFile;

patient patients[MAXIMUM];


//socket and  sockets related variabless
int sockfd, portno, n;
struct sockaddr_in serv_addr;
portno = 3000;
struct hostent *server;

char buffer[256];

//Tracking the indices of patients array
int count = 0;

//All commands are stored here 
//char commands[5][30] = {"1: Add Patient", "2: Add Patient List", "3: Check Status", "4: Send file", "5:Search "};

/*Function that prompts a user to enter 
a command and returns an integer depending on the command.*/
int getCommand(void)
{

    puts("\n\nCOMMANDS\n");
    int command;//Variable to store command
    printf(" 1: AddPatient\n 2: AddPatientList\n 3: CheckStatus\n 4: Search\n 5: SubmitPatientfile\n");
    printf("Select Command: ");
    scanf("%d", &command);
    return command;
}

/* Function that uses the value returned by getCommand()
call the appropriate function*/
void usecommand(int thecommand)
{

    if (thecommand == 1)
    {
        //function to add patients
        puts("\n\n\tAddPatient\n");
        addPatient();
    }
    else if (thecommand == 2)
    {
        //add patient txt file

        puts("\n\n\tAdd Patient List\n");
        addPatientList();
    }
    else if (thecommand == 3)
    {
        //check status
        puts("\n\n\tCheck Status\n");
        checkStatus();
    }
    
    else if (thecommand == 4)
    {
        //search for from patient file
        printf("\n\n\tSearch Patient Records\n");
        searchPatient();
    }
    else if (thecommand == 5)
    {
        //Send Patients file
        puts("\n\n\tSend Patient File\n");
        sendPatientFile();
    }
    else
    {
        puts("\nWrong selection\n\a");
        usecommand(getCommand());
    }
}

// Add patient function
void addPatient()
{
    char sName[30];
    char key;
    for (int i = count; i < MAXIMUM; i++)
    {
        printf("\nPatient %d\n", count + 1);
        printf("First Name :");
        scanf("%s", patients[i].patientName);
        printf("Second Name :");
        scanf("%s",sName);
        //put space btn first name and secone name
        strcat(patients[i].patientName,"  ");
        strcat(patients[i].patientName,sName);
        printf("Gender:");
        scanf("%s", patients[i].patientGender);
        printf("Date:");
        scanf("%s", patients[i].date);
        printf("Status:");
        scanf("%s", patients[i].patientStatus);
        printf("More patients? y/n:");
        scanf(" %c", &key);
        count += 1;
        if (key == 'y' || key == 'Y')
        {
            continue;
        }
        else
        {
            return;
        }
    }

    
}

void addPatientList()
{
    //Check if there are patients in the patients array
    if (count <= 0)
    {
        puts("Nothing to add\n");
        usecommand(getCommand());
    }
    else
    {
        patientsFile = fopen("patients.txt", "a+");
        if (patientsFile)
        {
            for (int i = 0; i < count; i++)
            {
                int a = fprintf(patientsFile, "%-20s\t\t%-10s\t%-6s\t%-10s\n", patients[i].patientName,
                                patients[i].date, patients[i].patientGender, officerName);
            }
            fclose(patientsFile);
            printf("Patients added successfully.");
            
            count = 0;
        }
    }
}

void checkStatus()
{
    patientsFile = fopen("patients.txt", "r");
    if (patientsFile == NULL)
    {
        printf("File Error!!");
    }
    else
    {
        int nunOfLines = 0;
        char buffer[255];
        while (fgets(buffer, 255, patientsFile) != NULL)
        {
            nunOfLines++;
        }
        if (nunOfLines == 0)
        {
            printf("No cases stored in the file\n");
        }
        else if (nunOfLines == 1)
        {
            printf("One case stored in the file\n");
        }
        else
        {
            printf("%d cases stored in the file\n", nunOfLines);
        }
    }
}
void searchPatient()
{
    char *name = "NAME";
    char *date = "DATE";
    char *gender = "GENDER";
    char *officer = "OFFICER";
    patientsFile = fopen("patients.txt", "r");
    char searchString[50];
    printf("Enter name to search: ");
    scanf("%s", searchString);
    puts("\n\n\tSearch Results\n");
    printf("%-20s\t\t%-10s\t%-6s\t%-10s\n", name, date, gender, officer);
    int available = 0;
    int totalRecords = 0;
    char buffer[255];
    while (fgets(buffer, 255, patientsFile) != NULL)
    {
        totalRecords++;
        if (strstr(buffer, searchString) != NULL)
        {
            puts(buffer);
            available++;
        }
    }
    if (totalRecords)
    {

        if (available == 1)
        {
            printf("1 record returned out of %d", totalRecords);
        }
        if (available > 1)
        {
            printf("%d records returned out of %d", available, totalRecords);
        }
    }
    else
    {
        printf("The patients file is empty");
    }
}
void sendPatientFile()
{
    int totalRecords = 0;
    patientsFile = fopen("patients.txt", "r+");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname("127.0.0.1");
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    //Check if there are cases in the patients file before connecting to the server
    totalRecords = howManyCases();
    if (totalRecords == 0)
    {
        connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        char *message = "I have nothing baby";
        printf("You don't have anything to send\nNext time be serious");
        send(sockfd, message, sizeof message, 0);
        return 0;
    }
    // connect to server with server address which is set above (serv_addr)

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR while connecting");
        return;
    }
    //send hospital name to server

    FILE *fileHd = fopen("patients.txt", "r+");
    // inside this while loop, implement communicating with read/write or send/recv function
    while (1)
    {
        
        //making sure variable buffer is initiallized to zero.
        bzero(buffer, 256);

        //Input that goes to the server is obtained and stored in buffer.

        if (fgets(buffer, 255, fileHd) == NULL)
        {

            fclose(patientsFile);
            //remove("patients.txt");
            fclose(fopen("patients.txt", "w+"));
            write(sockfd, hospitalName, strlen(hospitalName));
            printf("\n\nFile sent successfully\n\n");
            close(sockfd);

            break;
        }

        n = write(sockfd, buffer, strlen(buffer));
        //send_file(patientsFile,sockfd);
        //printf("The return value of write() is %d\n",n);
        if (n < 0)
        {
            perror("ERROR while writing to socket");
            exit(1);
        }

        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);

        if (n < 0)
        {
            perror("ERROR while reading from socket");
            exit(1);
        }

        // escape this loop, if the server sends message "quit"

        if (!bcmp(buffer, "quit", 4))
            break;
    }
}

int main()
{
    
    int command;
    puts("\n\t\tHello! Welcome\n");
    puts("Enter Hospital Name.");
    scanf("%s", hospitalName);

    puts("Enter your username");
    //gets(officerName,100);
    scanf("%s", officerName);

    command = getCommand();

    usecommand(command);
    //Run forever boss.
    while (1)
    {
        usecommand(getCommand());
        continue;
    }
}

int howManyCases()
{
    patientsFile = fopen("patients.txt", "r");
    if (patientsFile == NULL)
    {
        return NULL;
    }
    else
    {
        int nunOfLines = 0;
        char buffer[255];
        while (fgets(buffer, 255, patientsFile) != NULL)
        {
            nunOfLines++;
        }

        return nunOfLines;
    }
}

void send_file(FILE *fp, int sockfd){
  int n;
  char data[1024] = {0};

  while(fgets(data, 1024, fp) != NULL) {
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, 1024);
  }
}

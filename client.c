#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#define MAX_PATIENTS 100

//Pointer to the patients file
FILE *patientsFile;

//District name and officer names declared as
char districtName[30];
char officerName[100];
char hospitalName[100];
int sockfd, portno, n;
struct sockaddr_in serv_addr;

struct hostent *server;

char buffer[256];
portno = 8081;

typedef struct Patients
{
    char patientName[100];
    char patientStatus[30];
    char patientGender[10];
    char date[50];

} patient;

int patientNum = 0;

patient patients[MAX_PATIENTS];

char commands[5][30] = {"1: Add Patient", "2: Add Patient List", "3: Check Status", "4: Add Patient txtfile", "5:Search creteria"};
int selectCommand(void)
{

    puts("\nAll commands\n");
    char selection;
    for (int i = 0; i < 5; i++)
    {
        puts(commands[i]);
    }
    printf("Select Command: ");
    scanf("%d", &selection);
    return selection;
}
void callfunction(int selectedCommand)
{

    if (selectedCommand == 1)
    {
        //function to add patients
        puts("\n\n---------Add Patient--------\n");
        addPatient();
    }
    else if (selectedCommand == 2)
    {
        //add patient txt file

        puts("\n\n--------Add Patient List------\n");
        addPatientList();
    }
    else if (selectedCommand == 3)
    {
        //check status
        puts("-\n\n--------Check Status-------\n");
        checkStatus();
    }
    else if (selectedCommand == 4)
    {
        //Send Patients file

        // create socket and get file descriptor
        puts("\n\n-------Sending Patient File-----\n");
        sendPatientFile();
    }
    else if (selectedCommand == 5)
    {
        //search patient information
        //search by name
        printf("\n\n------Search Patient Records------\n");
        printf("---search by name or search by date(dd/mm/yyyy)---\n");
        searchPatient();
    }
    else
    {
        puts("Wrong selection\n\a");
        callfunction(selectCommand());
    }
}

// Add patient function
void addPatient()
{
    char key;
    for (int i = patientNum; i < MAX_PATIENTS; i++)
    {
        printf("\n#Patient No. %d\n", patientNum + 1);
        //fflush(stdin);
        printf("Name:");
        //int i = gets(patients[i].patientName, 100);
        scanf("%s", patients[i].patientName);
        printf("Gender:");
        //gets(patients[i].patientGender, 100);
        scanf("%s", patients[i].patientGender);
        //fflush(stdin);
        printf("Date:");
        //gets(patients[i].date, 100);
        scanf("%s", patients[i].date);
        //fflush(stdin);
        printf("Status:");
        //gets(patients[i].patientStatus, 50);
        scanf("%s", patients[i].patientStatus);
        //fflush(stdin);
        printf("More patients? y/n:");
        //key = getchar();
        //fflush(*);
        scanf(" %c", &key);
        patientNum += 1;
        if (key == 'y' || key == 'Y')
        {
            ; //printf("\n#Patient No. %d\n",patientNum+1);
        }
        else
        {
            return;
        }
    }

    // printf("total number of patients is %d\n", patientNum);
    // int i = 0;
    // puts("Name\tStatus");
    // for (; i <= patientNum; i++)
    // {
    //     printf("%s\t%s", patients[i].patientName, patients[i].patientStatus);
    // }
}

void addPatientList()
{
    //Check if there are patients in the patients array
    if (patientNum <= 0)
    {
        printf("%d", patientNum);
        puts("No patients registered yet");
        callfunction(selectCommand());
    }
    else
    {
        //printf("The patients are:-%d", patientNum);
        patientsFile = fopen("patients.txt", "a+");
        if (patientsFile)
        {
            //printf("Failed to open file patients.txt");
            for (int i = 0; i < patientNum; i++)
            {
                int a = fprintf(patientsFile, "%-20s\t%-10s\t%-6s\t%-10s\n", patients[i].patientName,
                                patients[i].date, patients[i].patientGender, officerName);
            }
            fclose(patientsFile);
            if (patientNum == 1)
            {
                printf("Only one patient added");
            }
            else
            {
                printf("%d Patients added successfully.", patientNum);
            }
            patientNum = 0;
        }
    }
}

void checkStatus()
{
    patientsFile = fopen("patients.txt", "r");
    if (patientsFile == NULL)
    {
        printf("Unable to open file \"patients.txt\" %s", patientsFile);
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
            printf("There are no cases stored yet in the file");
        }
        else if (nunOfLines == 1)
        {
            printf("There is only one case stored in the file\n");
        }
        else
        {
            printf("There are %d cases stored in the file", nunOfLines);
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
    char searchName[50];
    printf("Enter name to search: ");
    scanf("%s", searchName);
    puts("\n---------Search Results----------\n");

    printf("%-20s\t%-10s\t%-6s\t%-10s", name, date, gender, officer);
    int available = 0;
    int totalRecords = 0;
    char buffer[255];
    while (fgets(buffer, 255, patientsFile) != NULL)
    {
        totalRecords++;
        if (strstr(buffer, searchName) != NULL)
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
        else
        {
            puts("Whoops nothing matched your query boss.");
        }
    }
    else
    {
        printf("Its seems you have not added any recerds to the file");
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
    totalRecords = totalCases();
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

            return 0;
        }

        n = write(sockfd, buffer, strlen(buffer));

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

int main(int argc, char *argv[])
{

    puts("Welcome to the covid19 case reporting and management tool of ugand");
    puts("Enter Hospital Name.");
    //fflush(stdin);
    // gets(hospitalName,100);
    scanf("%s", hospitalName);
    puts("Enter your name");
    //gets(officerName,100);
    scanf("%s", officerName);

    int selectedCommand;
    selectedCommand = selectCommand();

    callfunction(selectedCommand);
    //Run forever boss.
    while (1)
    {
        callfunction(selectCommand());
        continue;
    }
}

int totalCases()
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

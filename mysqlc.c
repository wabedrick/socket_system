#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Patients
{
	char patientName[100];
	char patientStatus[30];
	char patientGender[10];
	char date[50];

} patient;

patient patients[100];

int main(int argc, char **argv)
{
	MYSQL *con = mysql_init(NULL);

	if (con == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}

	if (mysql_real_connect(con, "127.0.0.1", "root", "", "trial", 0, NULL, 0) == NULL)
	{
		printf("Failed to connect to database");
	}

	FILE *serverFile;
	serverFile = fopen("server_file.txt", "r+");
	puts("\nConnected to database server\n");

	char buffer[255], dtm[255], sqlStatement[255];
	char  secondName[50];
	int index = 0;
	while (fgets(buffer, 255, serverFile) != NULL)
	{
		//puts(buffer);
		//printf(buffer);
		strcpy(dtm, buffer);
		sscanf(dtm, "%s %s %s %s %s\n", patients[index].patientName, secondName, patients[index].date,
			   patients[index].patientGender, patients[index].patientStatus);

		//concatenate first to patients.name
		strcat(patients[index].patientName, "  ");
		strcat(patients[index].patientName, secondName);

		snprintf(sqlStatement, 300, "INSERT INTO patients(name,date,gender,officer)\
		 VALUES('%s','%s','%s','%s')",
				 patients[index].patientName, patients[index].date, patients[index].patientGender,
				 patients[index].patientStatus);

		if (mysql_query(con, sqlStatement))
		{
			fprintf(stderr, "%s\n", mysql_error(con));
		}
		//printf("Name: %s \nDate: %s \nGender: %s\nStatus: %s\n",
		//patients[index].patientName, patients[index].date, patients[index].patientGender,
		//patients[index].patientStatus);
		index++;
	}
	fclose(fopen("server_file.txt", "w+"));

	mysql_close(con);
	exit(0);
}

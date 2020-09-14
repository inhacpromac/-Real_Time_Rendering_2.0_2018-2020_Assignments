#include <stdio.h>
#include <ctype.h>

#define NUM_EMPLOYEES 5 // Simply Change this constant value to have as many number of Employee Records as you please ... 

#define NAME_LENGTH 100
#define MARITAL_STATUS 10

struct Employee
{
	char name[NAME_LENGTH];
	int age;
	char sex;
	float salary;
	char marital_status;
};

int main(void)
{
	void MyGetString(char[], int);

	struct Employee EmployeeRecord[NUM_EMPLOYEES]; //An array of <NUM_EMPLOYEES> structs - Each being type 'struct Employee'
	int i;

	// ****** USER INPUT INITIALIZATION OF ARRAY OF 'struct Employee' ******
	for (i = 0; i < NUM_EMPLOYEES; i++)
	{
		printf("\n\n\n\n");
		printf("********** DATA ENTRY FOR EMPLOYEE NUMBER %d **********\n", (i + 1));

		printf("\n\n");
		printf("Enter Employee Name : ");
		MyGetString(EmployeeRecord[i].name, NAME_LENGTH);

		printf("\n\n\n");
		printf("Enter Employee's Age (in years) : ");
		scanf("%d", &EmployeeRecord[i].age);

		printf("\n\n");
		printf("Enter Employee's Sex (M/m For Male, F/f For Female) : ");
		EmployeeRecord[i].sex = getch();
		printf("%c", EmployeeRecord[i].sex);
		EmployeeRecord[i].sex = toupper(EmployeeRecord[i].sex);

		printf("\n\n\n");
		printf("Enter Employee's Salary (in Indian Rupees) : ");
		scanf("%f", &EmployeeRecord[i].salary);

		printf("\n\n");
		printf("Is The Employee Married? (Y/y For Yes, N/n For No) : ");
		EmployeeRecord[i].marital_status = getch();
		printf("%c", EmployeeRecord[i].marital_status);
		EmployeeRecord[i].marital_status = toupper(EmployeeRecord[i].marital_status);
	}


	// *** DISPLAY ***
	printf("\n\n\n\n");
	printf("********** DISPLAYING EMPLOYEE RECORDS **********\n\n");
	for (i = 0; i < NUM_EMPLOYEES; i++)
	{
		printf("*********** EMPLOYEE NUMBER %d **********\n\n", (i + 1));
		printf("Name           : %s\n", EmployeeRecord[i].name);
		printf("Age            : %d years\n", EmployeeRecord[i].age);

		if (EmployeeRecord[i].sex == 'M')
			printf("Sex            : Male\n");
		else if(EmployeeRecord[i].sex == 'F')
			printf("Sex            : Female\n");
		else
			printf("Sex            : Invalid Data Entered\n");


		printf("Salary         : Rs. %f\n", EmployeeRecord[i].salary);

		if (EmployeeRecord[i].marital_status == 'Y')
			printf("Marital Status : Married\n");
		else if (EmployeeRecord[i].marital_status == 'N')
			printf("Marital Status : Unmarried\n");
		else
			printf("Marital Status : Invalid Data Entered\n");


		printf("\n\n");
	}

	return(0);
}

// *** SIMPLE RUDIMENTARY IMPLEMENTATION OF gets_s() ***
// *** IMPLEMENTED DUE TO DIFFERENT BEHAVIOUR OF gets_s() / fgets() / fscanf() ON DIFFERENT PLATFORMS ***
// *** BACKSPACE / CHARACTER DELETION AND ARROW KEY CURSOR MOVEMENT NOT IMPLEMENTED ***

void MyGetString(char str[], int str_size)
{
	//variable declarations
	int i;
	char ch = '\0';

	//code
	i = 0;
	do
	{
		ch = getch();
		str[i] = ch;
		printf("%c", str[i]);
		i++;
	}while ((ch != '\r') && (i < str_size));
	
	if (i == str_size)
		str[i - 1] = '\0';
	else
		str[i] = '\0';
}
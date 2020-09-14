#include <stdio.h>

#define NAME_LENGTH 100
#define MARITAL_STATUS 10

struct Employee
{
	char name[NAME_LENGTH];
	int age;
	float salary;
	char sex;
	char marital_status[MARITAL_STATUS];
};

int main(void)
{
	struct Employee EmployeeRecord[5]; //An array of 5 structs - Each being type 'struct Employee'

	char employee_rajesh[] = "Rajesh";
	char employee_sameer[] = "Sameer";
	char employee_kalyani[] = "Kalyani";
	char employee_sonali[] = "Sonali";
	char employee_shantanu[] = "Shantanu";

	int i;

	// ****** HARD-CODED INITIALIZATION OF ARRAY OF 'struct Employee' ******

	// ****** EMPLOYEE 1 ******
	strcpy(EmployeeRecord[0].name, employee_rajesh);
	EmployeeRecord[0].age = 30;
	EmployeeRecord[0].sex = 'M';
	EmployeeRecord[0].salary = 50000.0f;
	strcpy(EmployeeRecord[0].marital_status, "Unmarried");

	// ****** EMPLOYEE 2 ******
	strcpy(EmployeeRecord[1].name, employee_sameer);
	EmployeeRecord[1].age = 32;
	EmployeeRecord[1].sex = 'M';
	EmployeeRecord[1].salary = 60000.0f;
	strcpy(EmployeeRecord[1].marital_status, "Married");

	// ****** EMPLOYEE 3 ******
	strcpy(EmployeeRecord[2].name, employee_kalyani);
	EmployeeRecord[2].age = 29;
	EmployeeRecord[2].sex = 'F';
	EmployeeRecord[2].salary = 62000.0f;
	strcpy(EmployeeRecord[2].marital_status, "Unmarried");

	// ****** EMPLOYEE 4 ******
	strcpy(EmployeeRecord[3].name, employee_sonali);
	EmployeeRecord[3].age = 33;
	EmployeeRecord[3].sex = 'F';
	EmployeeRecord[3].salary = 50000.0f;
	strcpy(EmployeeRecord[3].marital_status, "Married");

	// ****** EMPLOYEE 5 ******
	strcpy(EmployeeRecord[4].name, employee_shantanu);
	EmployeeRecord[4].age = 35;
	EmployeeRecord[4].sex = 'M';
	EmployeeRecord[4].salary = 55000.0f;
	strcpy(EmployeeRecord[4].marital_status, "Married");

	// *** DISPLAY ***
	printf("\n");
	printf("****** DISPLAYING EMPLOYEE RECORDS ******\n");
	for (i = 0; i < 5; i++)
	{
		printf("****** EMPLOYEE NUMBER %d ******\n", (i + 1));
		printf("Name           : %s\n", EmployeeRecord[i].name);
		printf("Age            : %d years\n", EmployeeRecord[i].age);

		if (EmployeeRecord[i].sex == 'M' || EmployeeRecord[i].sex == 'm')
			printf("Sex            : Male\n");
		else
			printf("Sex            : Female\n");

		printf("Salary         : Rs. %f\n", EmployeeRecord[i].salary);
		printf("Marital Status : %s\n", EmployeeRecord[i].marital_status);

		printf("\n");
	}

	return(0);
}

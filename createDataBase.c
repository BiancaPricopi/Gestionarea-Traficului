#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

void eroare(MYSQL *dataBase)
{
	printf("%s\n", mysql_error(dataBase));
	mysql_close(dataBase);
	exit(1);
}
int main()
{
	MYSQL *dataBase = mysql_init(NULL);

	if (dataBase == NULL)
	{
		printf("%s\n", mysql_error(dataBase));
		exit(1);
	}
	if (mysql_real_connect(dataBase, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
	{
		eroare(dataBase);
	}
	if (mysql_query(dataBase, "DROP DATABASE IF EXISTS TrafficManagement")) 
	{
		eroare(dataBase);
	}
	if (mysql_query(dataBase, "CREATE DATABASE TrafficManagement"))
	{
		eroare(dataBase);
	}
	mysql_close(dataBase);
	exit(0);
}
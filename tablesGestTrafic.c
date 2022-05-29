#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

void errorDB(MYSQL *dataBase)
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
	if (mysql_real_connect(dataBase, "localhost", "root", "root", "TrafficManagement", 0, NULL, 0) == NULL)
	{
		errorDB(dataBase);
	}
	if (mysql_query(dataBase, "DROP TABLE IF EXISTS users")) 
	{
		errorDB(dataBase);
	}
	if (mysql_query(dataBase, "CREATE TABLE users(id INT, username VARCHAR(50), currentSpeed INT, street VARCHAR(100))"))
	{
		errorDB(dataBase);
	}
	if (mysql_query(dataBase, "DROP TABLE IF EXISTS speed")) 
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "CREATE TABLE speed(street VARCHAR(100), speedLimit INT)"))
	{
		errorDB(dataBase);
	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Stefan Cel Mare', 50)")) 
	{
      errorDB(dataBase);
  	}
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Bucium', 50)")) 
	{
      errorDB(dataBase);
  	}
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Eternitate', 50)")) 
	{
      errorDB(dataBase);
  	}
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Cucu', 50)")) 
	{
      errorDB(dataBase);
  	}
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Independentei', 50)")) 
	{
      errorDB(dataBase);
  	}
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Constantin Pop', 50)")) 
	{
      errorDB(dataBase);
  	}		
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Poitiers', 50)")) 
	{
      errorDB(dataBase);
  	}	
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Tatarasi', 50)")) 
	{
      errorDB(dataBase);
  	}
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Tudor Vladimirescu', 50)")) 
	{
      errorDB(dataBase);
  	}	
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Ion Creanga', 50)")) 
	{
      errorDB(dataBase);
  	}		
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Vantu', 50)")) 
	{
      errorDB(dataBase);
  	}		
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Lapusneanu 73', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Pacurari', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Bahlui', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Socola', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Racoteanu', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Nicolae Iorga', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Silvestru', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Tabacului', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Calea Chisinaului', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Colonel Langa', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Sararie', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Nicolina', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Primaverii', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Niceman', 50)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO speed VALUES('Sf. Lazar', 50)")) 
	{
      errorDB(dataBase);
  	}
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('N2', 90)")) 
	{
      errorDB(dataBase);
  	}	
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('N3', 90)")) 
	{
      errorDB(dataBase);
  	}
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('N5', 90)")) 
	{
      errorDB(dataBase);
  	}	
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('E87', 100)")) 
	{
      errorDB(dataBase);
  	}
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('E60', 100)")) 
	{
      errorDB(dataBase);
  	}	
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('E58', 100)")) 
	{
      errorDB(dataBase);
  	}	
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('E583', 100)")) 
	{
      errorDB(dataBase);
  	}	
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('A1', 130)")) 
	{
      errorDB(dataBase);
  	}	
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('A2', 130)")) 
	{
      errorDB(dataBase);
  	}	
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('A3', 130)")) 
	{
      errorDB(dataBase);
  	}
  	if (mysql_query(dataBase, "INSERT INTO speed VALUES('A4', 130)")) 
	{
      errorDB(dataBase);
  	}	
	if(mysql_query(dataBase, "ALTER TABLE speed ADD idStreet INT PRIMARY KEY AUTO_INCREMENT FIRST"))
	{
		errorDB(dataBase);
	}	

	//fuel option
	if (mysql_query(dataBase, "DROP TABLE IF EXISTS fuel")) 
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "CREATE TABLE fuel(gasStation VARCHAR(50), street VARCHAR(50), type VARCHAR(20), price FLOAT)"))
	{
		errorDB(dataBase);
	}	
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Lukoil', 'Pacurari', 'Gasoline', 5.92)")) 
	{
      errorDB(dataBase);
  	}	
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Lukoil', 'Bahlui', 'Gasoline', 5.92)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Lukoil', 'Socola', 'Gasoline', 5.92)")) 
	{
      errorDB(dataBase);
  	}	
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Lukoil', 'Racoteanu', 'Gasoline', 5.98)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Pacurari', 'Gasoline', 5.94)")) 
	{
      errorDB(dataBase);
  	}	
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Nicolae Iorga', 'Gasoline', 5.94)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Silvestru', 'Gasoline', 5.94)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Tabacului', 'Gasoline', 5.94)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Calea Chisinaului', 'Gasoline', 5.94)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Colonel Langa', 'Gasoline', 5.94)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Sararie', 'Gasoline', 5.96)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Racoteanu', 'Gasoline', 6.00)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Socar', 'Bahlui', 'Gasoline', 5.99)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Socar', 'Poitiers', 'Gasoline', 5.99)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Mol', 'Silvestru', 'Gasoline', 6.04)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Mol', 'Pacurari', 'Gasoline', 6.06)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Mol', 'Bucium', 'Gasoline', 6.08)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Mol', 'Nicolina', 'Gasoline', 6.08)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('OMV', 'Bucium', 'Gasoline', 6.07)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('OMV', 'Nicolina', 'Gasoline', 6.07)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('OMV', 'Pacurari', 'Gasoline', 6.07)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Rompetrol', 'Bucium', 'Gasoline', 6.07)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Rompetrol', 'Primaverii', 'Gasoline', 6.07)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Rompetrol', 'Niceman', 'Gasoline', 6.07)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Pacurari', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Nicolae Iorga', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Silvestru', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Tabacului', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Calea Chisinaului', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Colonel Langa', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Sararie', 'Diesel', 5.92)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Petrom', 'Racoteanu', 'Diesel', 5.94)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Lukoil', 'Calea Chisinaului', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Lukoil', 'Pacurari', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Lukoil', 'Sf. Lazar', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Lukoil', 'Bahlui', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Lukoil', 'Socola', 'Diesel', 5.90)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Lukoil', 'Racoteanu', 'Diesel', 5.94)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Socar', 'Bahlui', 'Diesel', 5.99)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Socar', 'Poitiers', 'Diesel', 5.99)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Mol', 'Silvestru', 'Diesel', 6.00)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Mol', 'Pacurari', 'Diesel', 6.04)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Mol', 'Nicolina', 'Diesel', 6.06)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Mol', 'Bucium', 'Diesel', 6.08)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('OMV', 'Pacurari', 'Diesel', 6.04)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('OMV', 'Nicolina', 'Diesel', 6.05)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('OMV', 'Bucium', 'Diesel', 6.07)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Rompetrol', 'Niceman', 'Diesel', 6.04)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Rompetrol', 'Primaverii', 'Diesel', 6.07)")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO fuel VALUES('Rompetrol', 'Bucium', 'Diesel', 6.07)")) 
	{
      errorDB(dataBase);
  	}

	//weather option
	if (mysql_query(dataBase, "DROP TABLE IF EXISTS weather")) 
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "CREATE TABLE weather(date VARCHAR(50), weekDays VARCHAR(50), temperature VARCHAR(10))"))
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "INSERT INTO weather VALUES('10 jan 2022', 'Monday', '1°/-4°')"))
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "INSERT INTO weather VALUES('11 jan 2022', 'Tuesday', '-2°/-8°')"))
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "INSERT INTO weather VALUES('12 jan 2022', 'Wednesday', '-4°/-9°')"))
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "INSERT INTO weather VALUES('13 jan 2022', 'Thursday', '-1°/-4°')"))
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "INSERT INTO weather VALUES('14 jan 2022', 'Friday', '3°/0°')"))
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "INSERT INTO weather VALUES('15 jan 2022', 'Saturday', '4°/-4°')"))
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "INSERT INTO weather VALUES('16 jan 2022', 'Sunday', '2°/-2°')"))
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "INSERT INTO weather VALUES('17 jan 2022', 'Monday', '4°/-2°')"))
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "INSERT INTO weather VALUES('18 jan 2022', 'Tuesday', '3°/-2°')"))
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "INSERT INTO weather VALUES('19 jan 2022', 'Wednesday', '4°/-2°')"))
	{
		errorDB(dataBase);
	}

	//sports
	if (mysql_query(dataBase, "DROP TABLE IF EXISTS sport")) 
	{
		errorDB(dataBase);
	}
	if(mysql_query(dataBase, "CREATE TABLE sport(date VARCHAR(50), name VARCHAR(50), location VARCHAR(50))"))
	{
		errorDB(dataBase);
	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('22.01.2022', 'Faget Winter Race', 'Piata Unirii, Cluj-Napoca')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('29.01.2022', 'SKV Vertical Race', 'Cabana Postavaru, Brasov')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('29.01.2022', 'Winter Wolf Race 2022', 'Saldabagiu de Munte, Paleu, Bihor')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('05.02.2022', 'Halcyon Faget Winter Trail', 'Piata Unirii, Cluj-Napoca')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('26.02.2022', 'Siria Winter Trail', 'Siria, Arad')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('05.03.2022', 'Garboavele Trail Run', 'Strada Garboavelor, Vanatori, Galati')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('09.04.2022-10.04.2022', 'Maratonul Argonautilor', 'Cheile Dobrogei, Gradina, Constanta')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('16.04.2022', 'Feleacu Running Hills 2022', 'Strada Nordului, Cluj-Napoca')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('17.04.2022', 'Semimaraton Iasi', 'Palatul Culturii, Iasi')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('01.05.2022', 'Faget MTB Marathon', 'Padurea Faget, Cluj-Napoca')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('14.05.2022', 'Subcarpati Trail Run', 'Goruna, Cocorastii Mislii, Prahova')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('21.05.2022', 'Tour de Tur 16.', 'Lacul Mujdeni, Orasu Nou, Satu Mare')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('27.05.2022-29.05.2022', 'Transylvania 100', 'Bran, Brasov')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('28.05.2022', 'msg Maraton Apuseni', 'Statiunea Muntele Baisorii, Baisoara, Cluj')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('29.05.2022', 'Semimaraton Galati 2022', 'Galati')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('04.06.2022-5.06.2022', 'Brasov Marathon', 'Piata Sfatului, Brasov')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('11.06.2022', 'Maraton Oxigen Plus 2022', 'Tersasa Micu-Padis, Pietroasa, Bihor')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('11.06.2022', 'XRace', 'Lacul Siriu, Siriu')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('18.06.2022-19.06.2022', 'Revolution Race 2.0', 'Bobalna, Cluj')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('25.06.2022', 'Vidraru Bike Challenge', 'Capatanenii Ungureni, Arefu, Arges')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('02.07.2022', 'Triathlon X-MAN ROMANIA', 'Saldabagiu, Bihor')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('02.07.2022-03.07.2022', 'Rodnei Trail Race', 'Borsa, Maramures')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('23.07.2022', 'Buila Trail Race', 'Barbatesti, Valcea')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('08.08.2022-14.08.2022', 'Transylvania 6 days', 'Reghin, Mures')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('13.08.2022', 'Transalpina Sky Race', 'Cabana Obarsia Lotrului, Valcea')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('13.08.2022', 'Maratonul Nordului MTB Pomarla', 'Pomarla, Botosani')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('14.08.2022', 'Maratonul Nordului Teioasa Trail', 'Darabani, Botosani')")) 
	{
      errorDB(dataBase);
  	}
	if (mysql_query(dataBase, "INSERT INTO sport VALUES('17.09.2022', 'Omu Marathon', 'Bran, Brasov')")) 
	{
      errorDB(dataBase);
  	}

	if (mysql_query(dataBase, "DROP TABLE IF EXISTS accident")) 
	{
		errorDB(dataBase);
	}
	if (mysql_query(dataBase, "CREATE TABLE accident(street varchar(100))"))
	{
		errorDB(dataBase);
	}
	if (mysql_query(dataBase, "DROP TABLE IF EXISTS trafficJam")) 
	{
		errorDB(dataBase);
	}
	if (mysql_query(dataBase, "CREATE TABLE trafficJam(street varchar(100))"))
	{
		errorDB(dataBase);
	}
	mysql_close(dataBase);
	exit(0);

}
#ifndef _Person_

#define _Person_

#include<iostream>
#include<fstream>

class Person
{
protected:
	char name[28];// Name only contains 24 charracters
	char number[16];// Telephone number of person
	char email[36];// email of user (komiljonovbahromiddin@gmail.com)
public:

	Person() {};

	Person(const char* PersonName, const char* TelephoneNumber,const char* Email);

	void SetData(const char* PersonName, const char* TelephoneNumber, const char* e_mail);

	void SetName(const char* PersonName);
	
	void SetEmail(const char* e_mail);

	void SetNumber(const char* TelephoneNumber);

	void SaveToFile(std::fstream& file, int index);

	void ReadFromFile(std::fstream& file, int index);

	void ShowData();

	const char* GetName()
	{
		return this->name;
	}
	const char* GetNumber()
	{
		return this->number;
	}
	const char* GetEmail()
	{
		return this->email;
	}

	friend class ContactButton;
	friend class Linked;
};

Person::Person(const char* PersonName, const char* TelephoneNumber,const char* Email )
{
	this->SetName(PersonName);
	this->SetNumber(TelephoneNumber);
	this->SetEmail(Email);
}

void Person::SetData(const char* PersonName, const char* TelephoneNumber, const char* e_mail)
{
	this->SetName(PersonName);
	this->SetNumber(TelephoneNumber);
	this->SetEmail(e_mail);
}

void Person::SetName(const char* PersonName)
{
	short i = 0;
	do
		name[i] = PersonName[i];
	while (PersonName[i++] != '\0'&&i<28);
}

void Person::SetEmail(const char* e_mail)
{
	short i = 0;
	do
		email[i] = e_mail[i];
	while (e_mail[i++] != '\0'&&i<36);
}

void Person::SetNumber(const char* TelephoneNumber)
{
	short i = 0;
	do
		number[i] = TelephoneNumber[i];
	while (TelephoneNumber[i++] != '\0'&&i<16);
}

void Person::SaveToFile(std::fstream& file, int index)// Index is useful when data of contact changed
{
	if (!file.is_open())
	{
		std::cout << "could not save" << std::endl;
		exit(1);
	}
	if (index < 0)// if index < 0 data will be written end of file
		file.seekp(0, std::ios::end);
	else
		file.seekp(index * sizeof(Person), std::ios::beg);

	file.write((char*)this, sizeof(Person));
}

void Person::ReadFromFile(std::fstream& file, int index)
{
	if (!file.is_open())
	{
		std::cout << "could not read" << std::endl;
		exit(1);
	}
	file.seekg(index * sizeof(Person), std::ios::beg);

	file.read((char*)this, sizeof(Person));
}

void Person::ShowData()
{
	std::cout << " Name = " << name << std::endl;
	std::cout << " Number = " << number << std::endl;
}

#endif // !_Person_
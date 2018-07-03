/*
 * uGlobalTableAdventure.cpp
 *
 *  Created on: 23 Jan 2017
 *      Author: Harry
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <windows.h>

typedef std::vector<std::string> vecstr;

bool selectCountry(vecstr &);

// uses a stringstream to convert any unsigned int to a string, then returns the string
std::string convertToStr(unsigned int number)
{
	std::stringstream ss;
	ss << number;
	std::string text("");
	ss >> text;
	ss.str("");
	ss.clear();

	return text;
}

// adds a leading zero to a string if length of string is less than 2 chars
// this is to add leading zeroes to the components of the date/time strings so the format is always like hh:mm:ss, dd/mm/yyyy
std::string addLeadingZero(std::string input)
{
	if (input.length() < 2)
	{
		return '0' + input;
	}

	return input;
}

// return the path of the exe
std::string getExePath()
{
	char result[MAX_PATH];
	return std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
}

// return the file directory (for countries.txt and countries_backup.txt)
std::string getFileDirectory()
{
	std::string exe_path = getExePath();
	unsigned int pos = exe_path.find_last_of('\\');                 // find the last backslash in the exe path
	std::string directory = exe_path.substr(0, pos + 1);                // take the exe path up to that position as the directory
	return directory;
}

// generates a random number between min and max (inclusive)
int getRandomNumber(int min, int max)
{
	// static is used so that fraction is only calculated once
	static const double fraction = (1.0 / (static_cast<double>(RAND_MAX) + 1.0));

	// evenly distributes the number [rand()] across the range [min, max]
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

// returns a datetime string in the format dd/mm/yyyy hh:mm:ss
std::string getCurrentTime()
{
	// time(0) is the system time, time_t is a datetime type
	time_t t = time(0);

	// struct tm allows access to each part of the datetime object, localtime() uses the local timezone
	// using a pointer here so that the dereference (->) operator can be used to access each member of the struct
	struct tm *now = localtime(&t);

	// tm_year is number of years since 1900, so add 1900 to give a conventional year format
	unsigned int int_year = now->tm_year + 1900;
	std::string str_year = convertToStr(int_year);

	// tm_month is indexed with January = 0, so add 1 to give the months numbered 1-12
	unsigned int int_month = now->tm_mon + 1;
	std::string str_month = addLeadingZero(convertToStr(int_month));

	// tm_mday is just the day of the month numbered 1-31
	unsigned int int_day = now->tm_mday;
	std::string str_day = addLeadingZero(convertToStr(int_day));

	// tm_hour uses 24h time, numbered 0-23
	unsigned int int_hour = now->tm_hour;
	std::string str_hour = addLeadingZero(convertToStr(int_hour));

	// tm_min is numbered 0-59
	unsigned int int_min = now->tm_min;
	std::string str_min = addLeadingZero(convertToStr(int_min));

	// tm_sec is numbered 0-59
	unsigned int int_sec = now->tm_sec;
	std::string str_sec = addLeadingZero(convertToStr(int_sec));

	return str_day + '/' + str_month + '/' + str_year + ' ' + str_hour + ':' + str_min + ':' + str_sec;
}

bool searchAgain()
{
	std::string input("");
	while (1)
	{
		std::cout << "Would you like to search again? [y/n] ";
		std::cin >> input;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(32767, '\n');
		}

		if (input == "n")
		{
			return false;
		}
		else if (input == "y")
		{
			return true;
		}
	}
}

bool lockChoice(vecstr &countries, unsigned int &index)
{
	std::string input("");
	while (1)
	{
		std::cout << "Would you like to lock in your choice? [y/n] ";

		std::cin >> input;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(32767, '\n');
		}

		// if choice is not locked, then user gets to decide whether to search again
		if (input == "n")
		{
			// if user chooses to search again, then a new country is selected and the process starts over
			if (searchAgain())
			{
				// if another country is selected then this loop of lockChoice() still needs to return true, so this can be passed back to main()
				// however, the ~ used flag cannot be added on this branch because a country can only be locked on the final loop of lockChoice()
				if (selectCountry(countries))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			// if user chooses to not search again, then process ends
			else
			{
				return false;
			}
		}
		// this branch happens on the final loop of lockChoice(), so the ~ used flag can be added
		else if (input == "y")
		{
			countries[index] += ("~ This country was selected on " + getCurrentTime() + '.');             // add ~ as a flag to indicate the country having been used
			return true;
		}
	}
}

// this writes the new countries.txt file at the end of the program, including the new ~ used flag for the chosen country
void writeFile(vecstr &countries)
{
	std::string filepath = getFileDirectory() + "countries.txt";           // load the filepath into a string

	//std::cout << filepath << '\n';                           // FOR TESTING ONLY

	std::ofstream write_file(filepath.c_str(), std::ios::out);             // c_str() needed to convert the string into a const char* (for fstream objects)

	write_file << countries[0];                 // write the first country in the list to the file

	unsigned int index(1);                      // index needs to start at 1 because the first country has already been written

	while (index < countries.size())            // this prevents an extra newline (\n) from being added at the end of the file
	{
		write_file << '\n' << countries[index];
		++index;
	}

	write_file.close();
}

bool selectCountry(vecstr &countries)
{
	// this will be the index of the country selected. size() - 1 is necessary because the vector is [0] indexed
	unsigned int index = getRandomNumber(0, (countries.size() - 1));

	// check if the country has already been used, if it has then select a new one
	while (countries[index].find('~') != std::string::npos)
	{
		index = getRandomNumber(0, (countries.size() - 1));
	}
	std::cout << "You will be tasting the flavours of " << countries[index] << " tonight!\n";

	// if the choice is locked, then a country has been selected; this is returned to main()
	if (lockChoice(countries, index))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// returns true if every country has been used (as indicated by ~ flags)
bool allCountriesUsed(vecstr &countries)
{
	bool all_countries_used(true);
	unsigned int index(0);

	while (index < countries.size())
	{
		// if ~ is not found on any entry then set all_countries_used to false
		if (countries[index].find('~') == std::string::npos)
		{
			all_countries_used = false;
		}
		++index;
	}

	return all_countries_used;
}

void restoreFile()
{
	std::string input("");
	while (1)
	{
		std::cout << "Warning: Are you sure you want to restore countries.txt from the backup?\n";
		std::cout << "(This process is irreversible and will reset all progress!) [y/n] ";
		std::cin >> input;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(32767, '\n');
		}

		if (input == "n")
		{
			break;
		}
		else if (input == "y")
		{
			std::string backuppath = getFileDirectory() + "countries_backup.txt";

			std::ifstream read_backup(backuppath.c_str());
			unsigned int line(0);
			std::string this_line("");

			// temporarily store all the countries from the backup file
			vecstr temp;

			if (!read_backup)
			{
				std::cout << "Error: could not find countries_backup.txt.\n";
				read_backup.close();
				break;
			}
			else
			{
				while (!read_backup.eof())
				{
					// assign the current line to the string
					getline(read_backup, this_line);
					temp.push_back("");
					// assign the string to the vector element
					temp[line] = this_line;
					++line;
				}
				read_backup.close();

				std::string filepath = getFileDirectory() + "countries.txt";

				std::ofstream write_new(filepath.c_str(), std::ios::out);
				write_new << temp[0];                 // write the first country in the list to the file
				unsigned int index(1);                // index needs to start at 1 because the first country has already been written
				while (index < temp.size())           // this prevents an extra newline (\n) from being added at the end of the file
				{
					write_new << '\n' << temp[index];
					++index;
				}

				write_new.close();
				break;
			}
		}
	}
}

bool userChoiceRestoreFile()
{
	std::string input("");
	while (1)
	{
		std::cout << "Do you want to restore countries.txt from countries_backup.txt? [y/n] ";
		std::cin >> input;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(32767, '\n');
		}

		if (input == "n")
		{
			return false;
		}
		else if (input == "y")
		{
			return true;
		}
	}
}

// this reads the current countries.txt file and initialises the countries vector with its contents
// if all countries have been used (with ~ flag) then this returns false (so the program can terminate)
bool readFile(vecstr &countries)
{
	std::string filepath = getFileDirectory() + "countries.txt";

	std::ifstream read_file(filepath.c_str());
	unsigned int line(0);                         // keep track of the current line
	std::string this_line("");                    // store the current line

	if (!read_file)
	{
		std::cout << "Error: could not find countries.txt.\n";
		return false;
	}
	else
	{
		while (!read_file.eof())
		{
			// assign the current line to the string
			getline(read_file, this_line);
			countries.push_back("");
			// assign the string to the vector element
			countries[line] = this_line;
			++line;
		}

		// if all the countries have been used, then return false to main()
		if (allCountriesUsed(countries))
		{
			read_file.close();
			std::cout << "All countries have been used. Wow, that's a lot of cooking!\n";
			std::cout << "If you want to go again, please supply a new countries.txt or restore from the backup.\n";
			return false;
		}

		read_file.close();
		std::cout << "countries.txt has been loaded successfully.\n";
		return true;
	}
}

// this leaves the user with a prompt on screen to let them know the program has finished
// this version of the function is called when readFile() returns true to main() - this means a country will have been selected
void endProgram()
{
	std::cin.clear();
	std::cin.ignore(32767, '\n');
	std::cout << "Press any key to close...";
	std::cin.get();
}

// this version of the function is called when readFile() returns false to main()
// the cin.ignore() line needs to be omitted because no user input will have occurred
void endProgramFalse()
{
	std::cin.clear();
	std::cout << "Press any key to close...";
	std::cin.get();
}

void printCountries(vecstr &countries)
{
	unsigned int index(0);
	while (index < countries.size())
	{
		std::cout << countries[index] << '\n';
		++index;
	}
}

int main()
{
	// this is set first so that getRandomNumber() will work
	srand(static_cast<unsigned int>(time(0)));          // initial seed is set by system clock
	rand();                                             // initial call to rand() discards first result

	// create the vector to store all the available countries
	vecstr countries;

	std::cout << "Welcome to NERDATLAS!\n";

	// give the user a choice to restore the input file before running the program
	if (userChoiceRestoreFile())
	{
		restoreFile();
	}

	// if the input file exists, proceed with the program
	if (readFile(countries))
	{

		//printCountries(countries);                 // print the vector contents (for testing purposes only)

		//std::cout << getExePath() << '\n';           // print the path to the exe (for testing only)
		//std::cout << getFileDirectory() << '\n';     // print the directory (testing only)

		//std::cout << getCurrentTime() << '\n';       // print the current time (testing only)

		// if a country is selected (and locked in), then end with this text
		if (selectCountry(countries))
		{
			writeFile(countries);
			std::cout << "Congratulations! Enjoy your meal.\n";
		}
		// if no country is locked in, then end with this text
		else
		{
			writeFile(countries);
			std::cout << "Nothing taking your fancy? No worries, maybe we can cook up a storm tomorrow.\n";
		}
	}
	// terminate program if the countries.txt file does not exist, or if all countries have been used
	else
	{
		endProgram();
		return -1;
	}

	endProgram();
	return 0;
}

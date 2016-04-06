
// ReadersWriters.cpp : Defines the entry point for the console application.
//
#pragma comment(lib, "SDL2_image.lib")

#include "stdafx.h"
#include <windows.h>
#include <SDL.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <list>

using namespace std;

int Reader(void *data);
int Writer(void *data);

// number of active readers
int nr = 0;
// lock for access to the database
SDL_sem* rw = NULL; 
//lock for reader access to nr
SDL_sem* mutexR = NULL; 

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

int main(int argc, char* argv[])
{

	rw = SDL_CreateSemaphore(1);
	mutexR = SDL_CreateSemaphore(1);
	//The thread that will be used
	list<SDL_Thread*> readerThreads;
	list<SDL_Thread*> writerThreads;

	for (int i = 0; i < 10; i++)//create 10 reader threads
	{
		readerThreads.push_back(SDL_CreateThread(Reader, "reader", NULL));
		//Wait randomly
		SDL_Delay(16 + rand() % 640);
	}
	int i = 0;
	while (true) /*create one writer thread then waits for users to press enter and creates another*/
	{

		std::stringstream ss;
		ss << "achievement " << i << "\n";
		std::string data = ss.str();
		writerThreads.push_back(SDL_CreateThread(Writer, "writer", (void*)&data));
		i = i + 1;
		system("pause");

	}

	//Free semaphore
	SDL_DestroySemaphore(rw);
	rw = NULL;
	return 0;
}

int Reader(void *data)
{
	while (true)
	{

		SDL_SemWait(mutexR);
		nr = nr++;
		if (nr == 1)/*if first get lock*/
		{
			SDL_SemWait(rw);
		}
		SDL_SemPost(mutexR);
		/*Read from file*/
		cout << "Reading text file" << endl;
		string line;
		ifstream myfile("Achievements.txt");
		std::stringstream stringStr;
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				stringStr << line << '\n';
			}
			myfile.close();
		}
		else 
			cout << "Unable to open file";
		cout << stringStr.str();
		SDL_SemWait(mutexR);
		nr = nr--;
		if (nr == 0)/*if last , release lock*/
		{
			SDL_SemPost(rw);
		}
		SDL_SemPost(mutexR);
		//Wait randomly
		SDL_Delay(16 + rand() % 640);
	}
	return 0;
}
int Writer(void *data)
{
	std::string message = *((std::string*) data);
	SDL_SemWait(rw);
	/*Write to the file*/
	cout << "Writing to textfile" << endl;

	std::ofstream myFile;
	myFile.open("Achievements.txt");
	myFile << message;
	myFile.close();

	SDL_SemPost(rw);
	//Wait randomly
	SDL_Delay(16 + rand() % 640);
	return 0;
}


//Fork and pipe by Leighton Covington
//To compile: g++ -o3 -w lcovingtAssignment1.cpp -o lcovingtAssignment1
//To run: ./lcovingtAssignment1 filepath numberOfChildren

//NOTE: Children have an assigned section denoted by their start and end. Even though there is overlap in assigned values, the way the for loop for the child summation is written there is no double counting
/*
for(int j = start; j < end; j++) <- as you can see, we iterate to just before end, so start is included in the calculation, but end isn't. With the way that we are accessing the vector,
we do in fact calculate the right values, and do not double count
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

vector<int> fileValues;//each entry in this vector holds a line value from the file passed in after initialize is called

void initialize(char* filePath){//code in this function was inspired by the User: Javi R at this site https://stackoverflow.com/questions/5476616/stringstream-string-to-int as I had problems using stoi to convert strings to ints

    ifstream ifs(filePath);//set up a stream to read file in
    stringstream converter;//string stream to convert string to int
    string line;//variable to hold each line

    while(getline(ifs,line)){//while there is a line
	converter.str(line);//save the line in the converter
        int temp;//have a temporary int
        converter >> temp;//send converter string value into the temp holding place, this automatically converts the string to an int
        fileValues.push_back(temp);//add the int value to fileValues
    }
}

int main(int argc, char** argv)
{
    char* filePath = argv[1];//path to file we are summing
    int numberOfChildren = atoi(argv[2]);//get number of children processes we want

    if(!(numberOfChildren == 1 || numberOfChildren == 2 || numberOfChildren == 4)){//if the child process argument is not 1, 2, or 4 don't run the program
	cout << "\n" << "Number of children argument must equal 1, 2, or 4";
    }

    int buf;//buf used to hold values from pipe
    int ParentToChildrenStartFD[numberOfChildren][2];//pipe fd for parent to send children the starting index of their sum
    int ParentToChildrenEndFD[numberOfChildren][2];//pipe fd for parent to send children the ending index of their sum
    int ChildrenToParentFD[numberOfChildren][2];//pipe fd for children to send parent their local sums
    initialize(filePath);//reads file at that path and populates a vector of ints to store each lines value
    int numberOfLines = fileValues.size();//number of lines the file has
    int childrensWorkAmount = numberOfLines / numberOfChildren;//the size of the section of work each child will do, it is assumed that -> file passed size % numberOfChildren = 0

    for(int i = 0; i < numberOfChildren; i++){//setting up the necessary pipes
        pipe(ParentToChildrenStartFD[i]);
	pipe(ParentToChildrenEndFD[i]);
        pipe(ChildrenToParentFD[i]);
    }

    int childStart = 0;//value that will be written for pipe child start
    int childEnd = childStart + childrensWorkAmount;//value that will be written for pipe for child end

    for(int i = 0; i < numberOfChildren; i++){//here the parent writes the starts and ends for each child into their corresponding pipes
	write(ParentToChildrenStartFD[i][1], &childStart, sizeof(childStart));
	write(ParentToChildrenEndFD[i][1], &childEnd, sizeof(childEnd));
        childStart = childEnd;//increment to new section for next child
        childEnd = childStart + childrensWorkAmount;//increment to new section for next child
    }

    for(int i = 0; i < numberOfChildren; i++){//here the children code runs
	if(fork() == 0){//if we are a child
	    read(ParentToChildrenStartFD[i][0], &buf, sizeof(buf));//read our start value
            int start = buf;//save our start value
            read(ParentToChildrenEndFD[i][0], &buf, sizeof(buf));//read our end value
	    int end = buf;//save our end value
	    int sectionTotal = 0;//local sum to be sent to parent

	    cout << "Child: " << i << "'s start index to their section is: " << start << " and their end index to their section is: " << end - 1 << "\n";

	    for(int j = start; j < end; j++){//iterate over our section of the vector and save the sum of that section
		sectionTotal = sectionTotal + fileValues[j];
	    }

	    write(ChildrenToParentFD[i][1], &sectionTotal, sizeof(sectionTotal));//write our result to pipe for parent to read
	    exit(0);
	}
    }

    wait();//wait for children to be done

    int grandTotal = 0;//final total of the file
    for(int i = 0; i < numberOfChildren; i++){//iterate over the pipes that the children wrote to
        read(ChildrenToParentFD[i][0], &buf, sizeof(buf));//read a childs local sum
	grandTotal = grandTotal + buf;//add the local sum to our grand total
    }

    cout << "The grand total of the file is: " << grandTotal << "\n";//print the grand total

    return 0;//exit

}

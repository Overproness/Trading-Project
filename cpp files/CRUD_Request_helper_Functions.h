#include<iostream>
#include<sstream>
#include<vector>



//Parameters:
// rowData: vector of strings containing the data to be written to the file
//Returns:
// void
void writeToCSV(std::vector<std::string> rowData);


//Parameters:
// Buffer: vector of strings containing the data to be written to the file
//Returns:
// void
void writeToLog(std::vector<std::string> Buffer);


//Parameters:
// filename: name of the file to be read
//Returns:
// vector of strings containing the data read from the file
std::vector<std::string> readEnvFile(const std::string& filename);


//Parameters:
// None
//Returns:
// string containing the current date and time
std::stringstream generateTimeStamp();


//Parameters:
// message: string to be hashed
//Returns:
// string containing the hashed message
std::string createSignature(const std::string& message);


//Parameters:
// contents: pointer to the data to be written
// size: size of each element to be written
// nmemb: number of elements to be written
// data: pointer to the data to be written to
//Returns:
// size_t containing the number of bytes written
size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* data);


//Parameters:
// url: url to be sent the GET request
//Returns:
// string containing the response from the GET request
std::string handleGetRequests(std::string url);


//Parameters:
// value: value to be rounded
// precision: number of decimal places to round to
//Returns:
// double containing the rounded value
double round_to(double value, double precision);


//Parameters:
// a: vector to be pushed to
// b: vector to push
//Returns:
// vector containing the pushed vectors
std::vector<double> array_push_before(std::vector<double> a, std::vector<double> b);

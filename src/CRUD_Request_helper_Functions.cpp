#include <iostream>
#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <iomanip>
#include<sstream>
#include <chrono>
#include <string>
#include<vector>
#include <fstream>



// Function to write to the log file
void writeToLog(std::vector<std::string> Buffer) {                
    std::ofstream file;          // Create an output file stream
    file.open("Logs.txt", std::ios_base::app); // Open the file in append mode

    // Check if the file is open
    if (!file.is_open()) {                              
        file.open("Logs.txt", std::ios_base::app);              // Open the file in append mode
        if (!file.is_open())       
            return;
    }

    for(const auto& data : Buffer) {                // Write the data into the file
		file << data << "\n";
	}
    file.close();                  // Close the file
}


// Function to write to the CSV file
void writeToCSV(std::vector<std::string> rowData) {
    std::fstream file;           // Create an output file stream
    file.open("Trades.csv", std::ios::in | std::ios::out | std::ios_base::app); // Open the file in append mode

    // Check if the file is open
    if (!file.is_open()) {
        file.open("Trades.csv", std::ios_base::app);                // Open the file in append mode
        if (!file.is_open())                               
        return;
    }

    file.seekg(0, std::ios::end);                          //Move to the end of file
    // Check the file's size
    if (file.tellg() == 0) {
        // File is empty, write the header info
        file << "Date/Time,Side,Symbol,Price,Amount,OrderType,Leverage,Status\n";
    }
    else {
        // File has content, move the pointer back to the last newline
        file.seekg(-1, std::ios::cur);
        char ch;
        file.get(ch);                              // Get the current byte data
        while (ch != '\n') {                 // Check if the current byte is a newline
            file.seekg(-2, std::ios::cur);         // Move the pointer back by 2 bytes
            file.get(ch);                                 // Get the current byte data
        }
    }

    // Write the data into the file
    for (const auto& data : rowData) {
        file << data << ",";
    }
    file << "\n";

    // Close the file
    file.close();
}


// Function to read the API key and secret key from the .env file
std::vector<std::string> readEnvFile(const std::string& filename) {
    std::ifstream file(filename);                        // Open the file

    
    if (file.is_open()) {               // Check if the file is open
        std::string line;                  // String to store each line
        std::vector<std::string> envVars;  // Vector to store the environment variables
        while (std::getline(file, line)) {           // Read each line
            std::istringstream iss(line);      // Create an input string stream
            std::string key, value;             // Strings to store the key and value
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {                    // Read the key and value
                if (key == "API_KEY") {                // Check if the key is API_KEY
                    envVars.push_back(value);          // Add the value to the vector
                }
                else if (key == "SECRET_KEY") { 	// Check if the key is SECRET_KEY 
                    envVars.push_back(value);         // Add the value to the vector
                }
            }
        }
        return envVars;         // Return the vector
        file.close();     // Close the file
    }
    else {
        std::string error = "Couldn't extract Keys from env files. ";               // Print an error message
        std::vector<std::string> errorVector;
        errorVector.push_back(error);
        writeToLog(errorVector);                  //Writing to log file and return help message
        exit(0);
    }
	
}


// Read the API key and secret key from the .env file
std::vector<std::string> envVars = readEnvFile(".env");
static const std::string API_KEY = envVars[0];
static const std::string SECRET_KEY = envVars[1];


//static const std::string API_KEY = "19f74104f0add7cf96ee56f3aab50c81b21ed39fb7459cbf3ac19240f689b3e0";
//static const std::string SECRET_KEY= "fab3fe088dece05de08239d1f434b23261cdfa2c20ac49a3d554e212d27b8d7d";


// Function to generate a timestamp for the request
std::stringstream generateTimeStamp() {
    std::stringstream timestamp;           // String stream to store the timestamp
    timestamp << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();      // Get the current time in milliseconds
    return timestamp;
}


// Function to create a signature for the request
std::string createSignature(const std::string& message) {              
    unsigned char digest[SHA256_DIGEST_LENGTH];              // Buffer to store the digest
    HMAC(EVP_sha256(), SECRET_KEY.c_str(), SECRET_KEY.length(), (unsigned char*)message.c_str(), message.length(), digest, NULL);         // Create the HMAC SHA256 digest

    std::stringstream ss;      // String stream to store the signature
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)                              // Convert the digest to a hex string
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];   // Add each byte to the stream
    return ss.str();
}


// Callback function to receive HTTP response data
size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)contents, size * nmemb);             // Append the response data to the string
    return size * nmemb;                            // Return the number of bytes received
}


// Function for GET Requests
std::string handleGetRequests(std::string url) {
    CURL* curl;                // Curl object
    CURLcode res;               // Curl response code
    char errorBuffer[CURL_ERROR_SIZE]; // Buffer to store error messages
    std::string readBuffer;	 // Buffer to store response data
    curl = curl_easy_init();      // Initialize curl

    try {
        if (curl) {       // Check if curl is initialized
            // Set request headers (including API key)
            struct curl_slist* headers = NULL;                                            // Headers to be sent with the request
            headers = curl_slist_append(headers, ("X-MBX-APIKEY: " + API_KEY).c_str());     // Add API key to headers
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);                             // Set headers for the request

            // Set up libcurl for the GET request
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());                           // Set the URL
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);                   // Set the callback function
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);                    // Set the data object

            // Set error buffer and verbose mode
            curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // 1L to enable verbose mode
            // Perform the request
            res = curl_easy_perform(curl);

            // Check for errors and process the response
            if (res != CURLE_OK) {

                std::vector<std::string> error;                  // Vector to store the error message
                error.push_back({ errorBuffer });                     // Add the error message to the vector
                writeToLog(error);                              // Write the error message to the log file
                return errorBuffer;                            // Return the error message
            }
            else {
                return readBuffer;                          // Return the response data
            }
            // Clean up
            curl_easy_cleanup(curl);                        // Clean up curl
            curl_slist_free_all(headers);                   // Free the headers
        }
        else {
            throw std::runtime_error("Error: Could not initialize curl");              // Throw an error message
        }
    }
    catch(const std::exception& e) {
        std::vector<std::string> error;         
        error.push_back({ e.what() });
        writeToLog(error);                                // Write the error message to the log file and return the error message
        return e.what();
	}
}


//Function for round to precision
double round_to(double value, double precision)
{
    return std::round(value / precision) * precision;                   // Example: round_to(0.123456, 0.0001) = 0.1235
}


//Function for pushing the elements of one vector to another vector
std::vector<double> array_push_before(std::vector<double> a, std::vector<double> b) {
    if (a.size() == 0) {
        return b;
    }
    else {                                     //In this function the elements of vector b are put before the elements of vector a and returned
        for (auto& i : a) {
            b.push_back(i);
        }
        return b;
    }
}


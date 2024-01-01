#include<nlohmann/json.hpp>
#include"CRUD_Request_helper_Functions.h"
#include<chrono>
#include<curl/curl.h>
#include <iomanip>

//Setting Up the api keys through the values provided in the env file. This function is defined in 'CRUD_Request_helper_Functions.cpp' file. 
std::vector<std::string> envVariables = readEnvFile(".env");

//Making them static so they can be accessed only within this file. 
static const std::string API_KEY = envVariables[0];
static const std::string SECRET_KEY = envVariables[1];
const long RECV_WINDOW = 60000;    // This basically defines the amount of time...........................................
const std::string BASE_URL = "https://testnet.binancefuture.com";  // Base URL for the Binance Futures Testnet API

//static const std::string API_KEY = "19f74104f0add7cf96ee56f3aab50c81b21ed39fb7459cbf3ac19240f689b3e0";
//static const std::string SECRET_KEY = "fab3fe088dece05de08239d1f434b23261cdfa2c20ac49a3d554e212d27b8d7d";



using json = nlohmann::json;              //This is part of 3rd party library that we are using to parse JSON responses.


//This function is used to write timestamps in the trades.csv file. This file contains the records of all of our Trades. 
std::string getCurrentTimestamp() {         //Generates a timestamp in the 'Year-Month-Day Hour:Min:Sec' format
    auto now = std::chrono::system_clock::now();                   //Getting the current time
    auto currentTime = std::chrono::system_clock::to_time_t(now);     //Converting the time to a readable format

    std::stringstream ss;
    ss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");           //Formatting the time
    return ss.str();
}


//It takes a basic string and a string vector. It then searches for the string within the vector. 
int findIndex(std::vector<std::string> vec, std::string searchStr) {       //It returns the index if the string is found otherwise returns -1
    auto it = std::find(vec.begin(), vec.end(), searchStr);            
    if (it != vec.end()) {
        // Element found, return its index
        return std::distance(vec.begin(), it);
    }
    else {
        // Element not found
        return -1;
    }
}


// Function to fetch available symbols for trading
std::vector<std::string> fetchAvailableSymbols() {
    //For storing the response
    std::string readBuffer;

    // Binance Futures Testnet API endpoint for retrieving available symbols
    std::string url = BASE_URL + "/fapi/v1/exchangeInfo";

    //Making the request
    readBuffer = handleGetRequests(url);


    try {
        //Parsing the JSON response
        json json_string = json::parse(readBuffer);


        std::vector<std::string> symbols;
        if (json_string.contains("symbols") && json_string["symbols"].is_array()) {    //Checking if response is what we want it to be
            for (const auto& symbol : json_string["symbols"]) {                       //Storing symbols into a string vector. 
                symbols.push_back(symbol["symbol"].get<std::string>());
            }
            std::vector<std::string> result;
            result.push_back("Symbols List Fetched Successfully. ");               //Writing Logs
            writeToLog(result);
            return symbols;
        }
        else {        //Checking for possible errors
            throw std::runtime_error("Error fetching symbols!");
        }

    }
    catch (const std::exception& e) {
        std::vector<std::string> error;
        error.push_back({ e.what() });
        writeToLog(error);                               //Writing Logs and returning helpful responses
        std::vector<std::string> symbols;
        symbols.push_back("Reload");
        return symbols;
    }
}


//Funtion to fetch the open posiitons
std::vector<std::string> fetchOpenPositions() {

    //For storing the response
    std::string readBuffer;

    // Binance Futures Testnet API endpoint for retrieving account information
    std::string url = BASE_URL + "/fapi/v2/account";

    // Construct the query string for the request
    std::string queryString = "timestamp=" + generateTimeStamp().str();

    // Create the signature for the request
    std::string signature = createSignature(queryString);

    // Construct the request URL with query parameters and signature
    url += "?" + queryString + "&signature=" + signature;

    //Making the request
    readBuffer = handleGetRequests(url);

    std::vector<std::string> open_positions;                   //For storing the data that we will be returning
    try {
        // Parse JSON response using nlohmann::json
        auto json_response = json::parse(readBuffer);

        // Check if 'positions' array exists in the response
        if (json_response.contains("positions") && json_response["positions"].is_array()) {

            open_positions.push_back(json_response["availableBalance"]);
            for (const auto& position : json_response["positions"]) {                     //Storing required data
                std::string positionAmtStr = position["positionAmt"].get<std::string>();
                double positionAmt = std::stod(positionAmtStr);

                if (positionAmt != 0) {
                    open_positions.push_back(position["symbol"]);
                    open_positions.push_back(position["initialMargin"]);
                    open_positions.push_back(position["unrealizedProfit"]);
                    open_positions.push_back(position["leverage"]);
                    open_positions.push_back(position["entryPrice"]);
                    open_positions.push_back(position["positionAmt"]);
                }
            }

            std::vector<std::string> result;
            result.push_back("Open Positions Fetched Successfully. ");      //Writing logs and returning data
            writeToLog(result);
            if(open_positions.size()== 0)
			{
				open_positions.push_back("No Open Positions. ");
				return open_positions;
			}
			else
			{
				return open_positions;
			}
        }
        else {
            throw std::runtime_error("Error (Request): Couldn't Fetch the Open Positions.");
        }
    }
    catch (const std::exception&e) {
        std::vector<std::string> error;
        error.push_back({ e.what() });
        writeToLog(error);                            //Writing logs and returning helpful messages.
        open_positions.push_back("Try Reloading");
        return open_positions;
    }
}


// Function to fetch open orders
std::vector<std::string> fetchOpenOrders() {
    std::string readBuffer;

    // Binance Futures API endpoint for retrieving open orders
    std::string url = BASE_URL + "/fapi/v1/openOrders";


    // Construct the query string for the request
    std::string queryString = "timestamp=" + generateTimeStamp().str();

    // Create the signature for the request
    std::string signature = createSignature(queryString);

    // Construct the request URL
    url += "?" + queryString + "&signature=" + signature;

    readBuffer = handleGetRequests(url);

    try {
        //Parsing the JSON Response
        json j = json::parse(readBuffer);

        std::vector<std::string> open_orders;

        // Iterate through each object in the array and find required information
        if (j.is_array()) {
            if (j.size() > 0)        //Checking if there are any open Orders.
            {
                for (const auto& obj : j) {
                    open_orders.push_back(obj["symbol"]);
                    open_orders.push_back(obj["price"]);
                    open_orders.push_back(obj["origQty"]);
                    open_orders.push_back(obj["side"]);
                    unsigned int orderId = obj["orderId"];
                    std::string orderId1 = std::to_string(orderId);
                    open_orders.push_back(orderId1);
                }

                std::vector<std::string> result;
                result.push_back("Open Orders Fetched Successfully!");
                writeToLog(result);                                    //Writing Logs and returning data
                if (open_orders.size() == 0) {
                    open_orders.push_back("No Open Orders. ");
                }
                return open_orders;
            }
            else {
                std::vector<std::string> result;
                result.push_back("No Open Orders. ");
                open_orders.push_back("No Open Orders. ");
                writeToLog(result);                                     //Writing Logs and returning data
                return open_orders;
            }
        }
        else {
            std::vector<std::string> result;
            result.push_back("The response of Open Orders was not decipherable. ");
            open_orders.push_back("No Open Orders. ");
            writeToLog(result);                                     //Writing Logs and returning data
            return open_orders;
        }
    }
    catch (const std::exception&e) {
        std::vector<std::string> error;
        error.push_back({ e.what() });
        writeToLog(error);                         ////Writing Logs and helpful responses
        std::vector<std::string> open_orders;
        open_orders.push_back("Press the Next Button or Reload. ");
        return open_orders;
    }

}


//Fetches the leverage brackets for any symbol
std::string fetchAllowedLeverage(const std::string& symbol) {       //Takes a symbol as an argument and returns its maximum allowed leverage
    //For storing the response
    std::string readBuffer;

    // Binance Futures Testnet API endpoint for retrieving allowed leverage
    std::string url = BASE_URL + "/fapi/v1/leverageBracket";

    // Construct the query string for the request
    std::string queryString = "symbol=" + symbol + "&timestamp=" + generateTimeStamp().str();

    // Create the signature for the request
    std::string signature = createSignature(queryString);

    // Construct the request URL
    url += "?" + queryString + "&signature=" + signature;

    //Making the request
    readBuffer = handleGetRequests(url);

    try {
        //Parsing JSON response
        json json_string = json::parse(readBuffer);

        int leverage;
        std::string actualLeverage;
        // Check if the response is an array and contains the required data
        if (json_string.is_array() && json_string[0].contains("brackets")) {
            leverage = json_string[0]["brackets"][0]["initialLeverage"];
            std::vector<std::string> result;                                          //Finding the required data. Saving logs and sending data back
            result.push_back("Leverage Brackets Fetched Successfully. ");
            writeToLog(result);
            actualLeverage = std::to_string(leverage);
            return actualLeverage;
        }
        else {
            throw std::runtime_error("Error Fetching Leverage Brackets. ");                //Throwing errors if something goes wrong in the response
        }
    }
    catch (const std::exception&e) {
        std::vector<std::string> error;
        error.push_back({ e.what() });                 //Writing logs and returning helpful responses
        writeToLog(error);
        return "Error occurred. Try Reloading. ";
    }
}


int getSymbolPrecision(const std::string& symbol) {
    std::string readBuffer;
    int precision = -1; // Default precision value

    std::string url = BASE_URL + "/fapi/v1/exchangeInfo";

    readBuffer = handleGetRequests(url);

    try {
        json exchangeInfo = json::parse(readBuffer);

        for (const auto& symbolInfo : exchangeInfo["symbols"]) {
            if (symbolInfo["symbol"] == symbol) {
                precision = symbolInfo["quantityPrecision"];
                break;
            }
        }
    }
	catch (const std::exception& e) {
		std::vector<std::string> error;
		error.push_back({ e.what() });
		writeToLog(error);
	}


    return precision;
}

//Function to place a trade or an order. And can also be used intelligently to close a trade
std::string placeTrade(const std::string& symbol, const std::string& side, double quantity, const std::string& orderType, double price, int leverage) {
    CURL* curl;
    CURLcode res;
    char errorBuffer[CURL_ERROR_SIZE]; // Buffer to store error messages
    std::string readBuffer;
    curl = curl_easy_init();

    try {
        if (curl) {
            std::string url = BASE_URL + "/fapi/v1/order";

            int symbolPrecision = getSymbolPrecision(symbol); // Function to retrieve precision for the symbol from Binance's API

            // Adjust quantity precision if needed
            quantity = round_to(quantity, pow(10, -symbolPrecision));

            std::string queryString = "symbol=" + symbol + "&side=" + side + "&type=" + orderType +
                "&quantity=" + std::to_string(quantity) +
                "&timestamp=" + generateTimeStamp().str() + "&recvWindow=" + std::to_string(RECV_WINDOW);

            if (orderType == "LIMIT") {
                // Include additional parameters for limit orders
                queryString += "&timeInForce=GTC";
                queryString += "&price=" + std::to_string(price);
            }

            if (leverage > 1) {
                // Include leverage if it's higher than default (1)
                queryString += "&leverage=" + std::to_string(leverage);
            }
            std::string signature = createSignature(queryString);

            url += "?" + queryString + "&signature=" + signature;

            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, ("X-MBX-APIKEY: " + API_KEY).c_str());
            headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            // Assemble the POST data for the request
            std::string postData = "";  // Include any necessary data here if needed

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size());

            // Set error buffer and verbose mode
            curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // 1L to enable verbose mode

            res = curl_easy_perform(curl);

            if (res == CURLE_OK) {

                try {

                    json json_string = json::parse(readBuffer);
                    if (json_string.contains("orderId")) {
                        std::vector<std::string> order;
                        order.push_back(getCurrentTimestamp());
                        order.push_back(side);
                        order.push_back(symbol);
                        order.push_back(std::to_string(price));
                        order.push_back(std::to_string(quantity));
                        order.push_back(orderType);
                        order.push_back(std::to_string(leverage));
						order.push_back("New");
                        writeToCSV(order);
                        std::vector<std::string> result;
                        result.push_back("Trade placed successfully!");
						writeToLog(result);
                        return "Trade placed successfully!";
                    }
                    else {
                        std::string error = json_string["msg"].get<std::string>();
                        throw std::runtime_error(error);
                    }
                }
                catch (const std::exception& e) {
                    std::vector<std::string> error;
                    std::string result = { e.what() };
                    error.push_back(result);
                    writeToLog(error);
                    return result;
                }
            }
            else {
                throw curl_easy_strerror(res);
            }

            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
        }
        else {
            throw std::runtime_error("Error: Couldn't initialize the CURL Object. Try Again After Reloading. ");
        }
    }
	catch (const std::exception& e) {
		std::vector<std::string> error;
        std::string result = { e.what() };
		error.push_back(result);
		writeToLog(error);
		return result;
	}
}


//Function to fetch the realtime value of a symbol
std::string fetchRealTimeValue(const std::string& symbol) {
    std::string readBuffer;

    // Binance API endpoint for retrieving the real-time price of a symbol
    std::string url = "https://api.binance.com/api/v3/ticker/price?symbol=" + symbol;

    readBuffer = handleGetRequests(url);


    try {

        // Parse the JSON response to extract the real-time value
        json json_result = json_result.parse(readBuffer);


        // Check if the "price" field exists in the response
        if (json_result.contains("price") && json_result["price"].is_string()) {
            std::string realTimeValue = json_result["price"];
            std::vector<std::string> result;
            result.push_back("Real Time Price Fetched Successfully. ");
			writeToLog(result);
            return realTimeValue;
        }
        else {
            std::vector<std::string> error;
            error.push_back("Problem faced with Fetching Real Time Price");
            writeToLog(error);
            throw std::runtime_error("Error Fetching Real Time Price. ");
        }
    }
    catch (const std::exception&e) {
        std::vector<std::string> error;
        error.push_back({ e.what() });
        writeToLog(error);
        return "Try Again After Reloading. ";
    }
}


//Function to cancle an order
std::string cancelOrder(unsigned int orderId, std::string symbol) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    char errorBuffer[CURL_ERROR_SIZE]; // Buffer to store error messages
    curl = curl_easy_init();
    try {
        if (curl) {

            // ... (code for initializing curl and constructing URL headers, similar to your existing code)
            std::string url = BASE_URL + "/fapi/v1/order";
            std::cout << orderId << std::endl;
            std::string queryString = "symbol=" + symbol + "&orderId=" + std::to_string(orderId) + "&timestamp=" + generateTimeStamp().str();
            std::string signature = createSignature(queryString);

            url += "?" + queryString + "&signature=" + signature;

            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, ("X-MBX-APIKEY: " + API_KEY).c_str());

            // Perform DELETE request to cancel the order
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // Set headers

            // Set error buffer and verbose mode
            curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // 1L to enable verbose mode

            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                throw curl_easy_strerror(res);
            }
            else {
                try {
                    json json_string = json::parse(readBuffer);
                    if (json_string.contains("orderId")) {
                        std::vector<std::string> order;
                        order.push_back(getCurrentTimestamp());
                        order.push_back(json_string["side"]);
                        order.push_back(json_string["symbol"]);
                        order.push_back(json_string["price"]);
                        order.push_back(json_string["origQty"]);
						order.push_back(json_string["type"]);
                        order.push_back("");
                        order.push_back("Canceled");
                        writeToCSV(order);
                        std::vector<std::string> result;
                        result.push_back("Order canceled successfully!");
						writeToLog(result);
                        return "Order canceled successfully!";
                    }
                    else if (json_string.contains("msg")) {
                        throw json_string["msg"];
                    }
                    else {
                        throw "Error canceling order! Try Again After Reloading. ";
                    }
                }
                catch (const std::exception&e) {
                    throw e.what();
                }
            }
        }
        else {
            throw std::runtime_error("Error: Couldn't initialize the CURL Object. Try Again After Reloading. ");
        }
    }
    catch(const std::exception& e)
	{
		std::vector<std::string> error;
		error.push_back({ e.what() });
		writeToLog(error);
		return "Try Again After Reloading. ";
	}
    curl_easy_cleanup(curl);
}


//Function to close a position
std::string closePosition(std::string symbol, double positionAmt, double currentPrice, int leverage) {

    // Close position using a market order
    std::string side = (positionAmt > 0) ? "SELL" : "BUY";
    std::string result = placeTrade(symbol, side, std::abs(positionAmt), "MARKET", currentPrice, leverage);
    if (result == "Trade placed successfully!") {
        std::vector<std::string> error;
        error.push_back("Trade closed successfully!");              //For sending the correct response back to the user
        writeToLog(error);                                                  // Writing logs
        return "Trade closed successfully!";
    }
    else if (result == "The counterparty's best price does not meet the PERCENT_PRICE filter limit.") {
  //For sending the correct response back to the user
		std::vector<std::string> error;
		error.push_back("The counterparty's best price does not meet the PERCENT_PRICE filter limit.");
		writeToLog(error);                                                  // Writing logs
		return "Try Again Later. Exchange does not have enough liquidity. ";
    }
    else {
        std::vector<std::string> error;
        error.push_back(result);
        writeToLog(error);                        // Writing logs and sending the correct response back to the user
        return result;
    }
}


//Function to fetch the order Book of a symbol
std::vector<std::string> fetchOrderBook(std::string symbol) {
    //For storing the response
    std::string readBuffer;

    // Binance API endpoint for retrieving the order book
    std::string url = BASE_URL + "/fapi/v1/depth";

    std::string limit = "5"; // Number of order book entries to retrieve

    // Construct the query string for the request
    std::string queryString = "symbol=" + symbol + "&limit=" + limit;


    url += "?" + queryString;

    //Making the request
    readBuffer = handleGetRequests(url);

    try {
        //Parsing the JSON response
        json j = json::parse(readBuffer);
        // Get the bids and asks arrays
        auto bids = j["bids"];
        auto asks = j["asks"];
        std::vector<std::string> order_book;
        for (const auto& bid : bids) {

            order_book.push_back(bid[0].get<std::string>());
            order_book.push_back(bid[1].get<std::string>());                 //Storing the required data in appropriate vectors
        }

        for (const auto& ask : asks) {
            order_book.push_back(ask[0].get<std::string>());
            order_book.push_back(ask[1].get<std::string>());                    //Storing the required data in appropriate vectors
        }
        std::vector<std::string> result;
        result.push_back("Order Book Fetched Successfully. ");
        writeToLog(result);                                                      //Writing logs and returning data
        return order_book;
    }
    catch (const std::exception&e) {
        std::vector<std::string> error;
        error.push_back(e.what());                         //Writing logs and returning helpful responses
        writeToLog(error);
        std::vector<std::string> order_book;
        order_book.push_back("Try Reloading.");
        return order_book;
    }
}


//Function to fetch the historical data of a symbol. And returns all of the data that it has fetched.
std::vector<double> fetchHistoricalData(std::string timeFrame, std::string symbol) {
    //For storing the response
    std::string readBuffer;

    std::string url = BASE_URL + "/fapi/v1/klines";             // Binance Futures Testnet API endpoint for retrieving historical data
    std::string limit = "500";                                // Number of data points to retrieve
    std::string current_timestamp = generateTimeStamp().str();         // Current timestamp

    // Construct the query string for the request
    std::string queryString = "symbol=" + symbol + "&limit=" + limit + "&interval=" + timeFrame + "&endTime=" + current_timestamp;

    url += "?" + queryString;

    //Making the request
    readBuffer = handleGetRequests(url);

    std::vector<double> currentData;                                       //For storing the data that we will be returning
    try {
        json json_string = json::parse(readBuffer);
        long long timestamp = json_string[0][0];
        current_timestamp = std::to_string(timestamp);
        for (auto& i : json_string) {
            currentData.push_back(std::stod(i[1].get<std::string>()));
            currentData.push_back(std::stod(i[2].get<std::string>()));
            currentData.push_back(std::stod(i[3].get<std::string>()));
            currentData.push_back(std::stod(i[4].get<std::string>()));               //Storing the required data in appropriate vectors
        }
        std::vector<std::string> result;
        result.push_back("Historical Data Fetched Successfully. ");
        writeToLog(result);                                                   //Writing logs and returning data
        return currentData;
    }
    catch (const std::exception&e) {
        std::vector<std::string> error;
        error.push_back(e.what());                               //Writing logs and returning helpful responses
		writeToLog(error);
		return currentData;
    }
}


//Fetches only the most recent 'historical data' of a symbol and only returns the data of the latest candle.
std::vector<double> historicalDataUpdater(std::string timeFrame, std::string symbol) {
    //For storing the response
    std::string readBuffer;

    std::string url = BASE_URL + "/fapi/v1/klines";                         // Binance Futures Testnet API endpoint for retrieving historical data
    std::string current_timestamp = generateTimeStamp().str();                   // Current timestamp

    // Construct the query string for the request
    std::string queryString = "symbol=" + symbol + "&interval=" + timeFrame;

    url += "?" + queryString;

    //Making the request
    readBuffer = handleGetRequests(url);

    std::vector<double> currentData;                                //For storing the data that we will be returning
    try {
        //Parsing the JSON response
        json json_string = json::parse(readBuffer);
        double* arr = new double[2000];
        long long timestamp = json_string[0][0];
        current_timestamp = std::to_string(timestamp);
        for (int i = 0; i < 2000; i+=4) {
            *(arr + i) = std::stod(json_string[i][0].get<std::string>());
            *(arr + i+1) = std::stod(json_string[i][1].get<std::string>());
            *(arr + i+2) = std::stod(json_string[i][2].get<std::string>());                         //Storing the required data in appropriate vectors
            *(arr + i+3) = std::stod(json_string[i][3].get<std::string>());
        }

        currentData.push_back(arr[0]);
        currentData.push_back(arr[1]);
        currentData.push_back(arr[2]);
        currentData.push_back(arr[3]);                                //Storing the required data in appropriate vectors
        std::vector<std::string> result;
		result.push_back("Historical Data Fetched Successfully. ");
        writeToLog(result);
        return currentData;                                 //Writing logs and returning data
    }
    catch (const std::exception&e) {
        std::vector<std::string> error;
        error.push_back(e.what());
		writeToLog(error);                            //Writing logs and returning helpful responses
        return currentData;
    }
}
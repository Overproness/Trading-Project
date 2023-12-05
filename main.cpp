#include <iostream>
#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <rapidjson/document.h>
#include <nlohmann/json.hpp>

// Binance API credentials for the Testnet
const std::string API_KEY = "19f74104f0add7cf96ee56f3aab50c81b21ed39fb7459cbf3ac19240f689b3e0";
const std::string SECRET_KEY = "fab3fe088dece05de08239d1f434b23261cdfa2c20ac49a3d554e212d27b8d7d";
const long RECV_WINDOW = 60000;
const std::string BASE_URL = "https://testnet.binancefuture.com";  // Base URL for the Binance Futures Testnet

// Function to create a signature for the request
std::string createSignature(const std::string& message) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    HMAC(EVP_sha256(), SECRET_KEY.c_str(), SECRET_KEY.length(), (unsigned char*)message.c_str(), message.length(), digest, NULL);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    return ss.str();
}

// Callback function to receive HTTP response data
static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function for GET Requests
std::string handleGetRequests(std::string url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        // Set request headers (including API key)
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("X-MBX-APIKEY: " + API_KEY).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up libcurl for the GET request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors and process the response
        if (res != CURLE_OK) {
            std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            return readBuffer;
        }
            // Clean up
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
    }   
}


// Function to fetch open positions
void fetchOpenPositions() {

        std::string readBuffer;
        // Binance Futures Testnet API endpoint for retrieving account information
        std::string url = BASE_URL + "/fapi/v2/account";

        // Timestamp for the request (in milliseconds)
        std::stringstream timestamp;
        timestamp << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        // Construct the query string for the request
        std::string queryString = "timestamp=" + timestamp.str();

        // Create the signature for the request
        std::string signature = createSignature(queryString);

        // Construct the request URL with query parameters and signature
        url += "?" + queryString + "&signature=" + signature;

        readBuffer = handleGetRequests(url);
            // Parse JSON response using nlohmann::json
            try {
                auto json_response = nlohmann::json::parse(readBuffer);

                // Check if 'positions' array exists in the response
                if (json_response.contains("positions") && json_response["positions"].is_array()) {

                    // Loop through the 'positions' array and display position details
                    for (const auto& position : json_response["positions"]) {
                        std::string positionAmtStr = position["positionAmt"].get<std::string>();
                        double positionAmt = std::stod(positionAmtStr);

                        if (positionAmt > 0) {
                            std::cout << "Symbol: " << position["symbol"].get<std::string>()
                                << ", Position Amount: " << positionAmtStr
                                << ", Entry Price: " << position["entryPrice"].get<std::string>() << std::endl;
                            // Add display for other relevant position details as needed
                        }
                    }
                    std::cout << std::endl << std::endl;
                }
                else {
                    std::cerr << "Error: 'positions' data not found in the response." << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "JSON parsing error: " << e.what() << std::endl;
            }
}


// Function to fetch open orders
void fetchOpenOrders() {
    std::string readBuffer;

        // Binance Futures API endpoint for retrieving open orders
        std::string url = BASE_URL + "/fapi/v1/openOrders";

        // Timestamp for the request (in milliseconds)
        std::stringstream timestamp;
        timestamp << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        // Construct the query string for the request
        std::string queryString = "timestamp=" + timestamp.str();

        // Create the signature for the request
        std::string signature = createSignature(queryString);

        // Construct the request URL
        url += "?" + queryString + "&signature=" + signature;

        readBuffer = handleGetRequests(url);
            std::cout << "Received open orders: " << readBuffer << std::endl;

}

// Function to fetch order book
void fetchOrderBook() {
    std::string readBuffer;

        // Binance API endpoint for retrieving the order book
        std::string url = BASE_URL + "/fapi/v1/depth";

        // Request parameters (example for BTCUSDT with limit 10)
        std::string symbol = "BTCUSDT";
        std::string limit = "10"; // Number of order book entries to retrieve

        // Construct the query string for the request
        std::string queryString = "symbol=" + symbol + "&limit=" + limit;

        
        url += "?" + queryString;
        
        readBuffer = handleGetRequests(url);


            std::cout << "Received order book data: " << readBuffer << std::endl;
}

void placeTrade(const std::string& symbol, const std::string& side, double quantity, const std::string& orderType) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        std::string url = BASE_URL + "/fapi/v1/order";

        std::stringstream timestamp;
        timestamp << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        std::string queryString = "symbol=" + symbol + "&side=" + side + "&type=" + orderType +
            "&quantity=" + std::to_string(quantity) +
            "&timestamp=" + timestamp.str() + "&recvWindow=" + std::to_string(RECV_WINDOW);

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

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            std::cout << "Trade placed successfully!" << std::endl;
            std::cout << "Response: " << readBuffer << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();
}


// Function to get user inputs and place a trade
void getUserInputAndTrade() {
    std::string symbol="BTCUSDT", direction="LONG", orderType="MARKET";
    double leverage=10;
    double  quantity;

    // Get user inputs
    std::cout << "Enter the symbol you want to trade (e.g., BTCUSDT): ";
    std::cin >> symbol;

    std::cout << "Enter the trade direction (LONG or SHORT): ";
    std::cin >> direction;

    std::cout << "Enter the leverage you want to use: ";
    std::cin >> leverage;

    std::cout << "Enter the quantity you want to trade: ";
    std::cin >> quantity;

    quantity /= 125;

    std::cout << "Enter the order type (e.g., MARKET or LIMIT): ";
    std::cin >> orderType;

    std::string orderSide = direction == "LONG" ? "BUY" : "SELL"; // Assuming "LONG" means "BUY" and "SHORT" means "SELL"

    // Place the trade based on user inputs
    placeTrade(symbol, orderSide, quantity, orderType);
}

// Function to fetch available symbols for trading
void fetchAvailableSymbols() {
    std::string readBuffer;

        // Binance Futures Testnet API endpoint for retrieving available symbols
        std::string url = BASE_URL + "/fapi/v1/exchangeInfo";

        readBuffer = handleGetRequests(url);

            // Parse JSON response using RapidJSON
            rapidjson::Document document;
            document.Parse(readBuffer.c_str());

            if (!document.IsObject()) {
                std::cerr << "Error: Received data format is not an object." << std::endl;
                return;
            }

            // Display available symbols for trading
            const rapidjson::Value& symbols = document["symbols"];
            std::cout << "Available Symbols for Trading:" << std::endl;
            for (rapidjson::SizeType i = 0; i < symbols.Size(); ++i) {
                std::cout << symbols[i]["symbol"].GetString() << std::endl;
            }
}

void fetchAllowedLeverage(const std::string& symbol) {
    std::string readBuffer;

        // Binance Futures Testnet API endpoint for retrieving allowed leverage
        std::string url = BASE_URL + "/fapi/v1/leverageBracket";

        // Construct the query string for the request
        std::stringstream timestamp;
        timestamp << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::string queryString = "symbol=" + symbol + "&timestamp=" + timestamp.str();

        // Create the signature for the request
        std::string signature = createSignature(queryString);

        // Construct the request URL
        url += "?" + queryString + "&signature=" + signature;
        
        readBuffer = handleGetRequests(url);

            // Parse JSON response using RapidJSON
            rapidjson::Document document;
            document.Parse(readBuffer.c_str());

            // Output the entire JSON response
            //std::cout << "Full JSON Response: " << readBuffer << std::endl;

            // Check if the response is an array and contains the required data
            if (document.IsArray() && document.Size() > 0 &&
                document[0].HasMember("brackets") && document[0]["brackets"].IsArray()) {

                auto& brackets = document[0]["brackets"];
                for (const auto& bracket : brackets.GetArray()) {
                    if (bracket.IsObject() && bracket.HasMember("bracket") && bracket["bracket"].GetInt() == 1) {
                        if (bracket.HasMember("initialLeverage")) {
                            int initialLeverage = bracket["initialLeverage"].GetInt();
                            std::cout << "Initial Leverage for Bracket 1 of Symbol " << symbol << ": " << initialLeverage << std::endl;
                            break; // Assuming you only want the value for bracket 1
                        }
                    }
                }
            }
            else {
                std::cerr << "Error: Required data not found in the response." << std::endl;
            }
}


// Function to fetch account balance
void fetchAccountBalance(const std::string& symbol) {
    std::string readBuffer;
        // Binance Futures Testnet API endpoint for getting account balance
        std::string url = BASE_URL + "/fapi/v2/balance";

        // Timestamp for the request (in milliseconds)
        std::stringstream timestamp;
        timestamp << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        // Construct the query string for the request
        std::string queryString = "timestamp=" + timestamp.str();

        // Create the signature for the request
        std::string signature = createSignature(queryString);

        // Construct the request URL
        url += "?" + queryString + "&signature=" + signature;

        readBuffer = handleGetRequests(url);

            // Parse JSON response using RapidJSON
            rapidjson::Document document;
            document.Parse(readBuffer.c_str());

            if (!document.IsArray()) {
                std::cerr << "Error: Received data format is not an array." << std::endl;
                return;
            }

            // Find the balance for the entered symbol
            for (rapidjson::SizeType i = 0; i < document.Size(); ++i) {
                const rapidjson::Value& entry = document[i];
                if (entry.HasMember("asset") && entry["asset"].IsString() && entry.HasMember("balance") && entry["balance"].IsString()) {
                    if (entry["asset"].GetString() == symbol) {
                        // Check if the balance value is a number
                        if (std::isdigit(entry["balance"].GetString()[0])) {
                            double availableBalance = std::stod(entry["balance"].GetString());
                            std::cout << "Available balance for " << symbol << ": " << availableBalance << std::endl;
                            return; // Exit function after retrieving the balance
                        }
                        else {
                            std::cerr << "Error: Invalid balance format." << std::endl;
                            return;
                        }
                    }
                }
            }
            std::cerr << "Error: Symbol not found or balance data unavailable." << std::endl;
}


// Function to fetch the real-time value of a symbol
void fetchRealTimeValue(const std::string& symbol) {
    std::string readBuffer;

        // Binance API endpoint for retrieving the real-time price of a symbol
        std::string url = "https://api.binance.com/api/v3/ticker/price?symbol=" + symbol;

        readBuffer = handleGetRequests(url);

        
            // Parse the JSON response to extract the real-time value
            rapidjson::Document document;
            document.Parse(readBuffer.c_str());

            if (!document.IsObject()) {
                std::cerr << "Error: Received data format is not an object." << std::endl;
                return;
            }

            // Check if the "price" field exists in the response
            if (document.HasMember("price") && document["price"].IsString()) {
                std::string realTimeValue = document["price"].GetString();
                std::cout << "Real-time value for " << symbol << ": " << realTimeValue << std::endl;
            }
            else {
                std::cerr << "Error: Could not retrieve real-time value for " << symbol << std::endl;
            }
}




int main() {

    curl_global_init(CURL_GLOBAL_ALL);

    fetchRealTimeValue("BTCUSDT");
    fetchAvailableSymbols();

    //// Fetch allowed leverage for a selected symbol
    std::string symbol;
    std::cout << "Enter the symbol to check allowed leverage: ";
    std::cin >> symbol;
    fetchAllowedLeverage(symbol);

    // Fetch open positions
    fetchOpenPositions();

    //// Fetch open orders
    fetchOpenOrders();

    // Fetch order book
    fetchOrderBook();

    //// Fetch user's account balance
    fetchAccountBalance("USDT");

    // Place a futures market order
    getUserInputAndTrade();


    curl_global_cleanup();

    return 0;
}

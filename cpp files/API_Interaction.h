#include<vector>
#include<iostream>


//Parameters:
// None
//Returns:
// vector of strings containing all the symbols available for trading
std::vector<std::string> fetchAvailableSymbols();


//Parameters:
// None
//Returns:
//vector of string containing all the open positions
std::vector<std::string> fetchOpenPositions();


//Parameters:
// None
//Returns:
//vector of string containing all the open orders
std::vector<std::string> fetchOpenOrders();


//Parameters:
// symbol: string containing the symbol of the coin
// Returns:
// string containing the leverage allowed for the symbol
std::string fetchAllowedLeverage(const std::string& symbol);


//Parameters:
// symbol: string containing the symbol of the coin
// side: string containing the side of the order (BUY/SELL)
// quantity: double containing the quantity of the order
// orderType: string containing the type of the order (MARKET/LIMIT)
// price: double containing the price of the order
// leverage: int containing the leverage of the order
// Returns:
// string containing the response of the order placed
std::string placeTrade(const std::string& symbol, const std::string& side, double quantity, const std::string& orderType, double price, int leverage);


//Parameters:
// symbol: string containing the symbol of the coin
// Returns:
// string containing the value of the symbol in real time
std::string fetchRealTimeValue(const std::string& symbol);


//Parameters:
// symbol: string containing the symbol of the coin
// orderId: unsigned int containing the order id of the order to be cancelled
// Returns:
// string containing the response of the order cancelled
std::string cancelOrder(unsigned int orderId, std::string symbol);


//Parameters:
// symbol: string containing the symbol of the coin
// positionAmt: double containing the position amount of the order to be closed
// currentPrice: double containing the current price of the symbol
// leverage: int containing the leverage of the order
// Returns:
// string containing the response of the position closed
std::string closePosition(std::string symbol, double positionAmt, double currentPrice, int leverage);


//Parameters:
// symbol: string containing the symbol of the coin
// Returns:
// vector of strings containing the order book of the symbol
std::vector<std::string> fetchOrderBook(std::string symbol);


//Parameters:
// symbol: string containing the symbol of the coin
// timeFrame: string containing the time frame of the historical data
// Returns:
// vector of doubles containing the historical data of the symbol
std::vector<double> fetchHistoricalData(std::string timeFrame, std::string symbol);


//Parameters:
// symbol: string containing the symbol of the coin
// timeFrame: string containing the time frame of the historical data
// Returns:
// vector of doubles containing only the data of the most recent candle of the symbol
std::vector<double> historicalDataUpdater(std::string timeFrame, std::string symbol);


//Parameters:
// vec: vector of strings containing the data to be searched
// searchStr: string containing the data to be searched
// Returns:
// int containing the index of the data to be searched
int findIndex(std::vector<std::string> vec, std::string searchStr);
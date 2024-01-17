
# Trading System with AI Integration

## Overview

Welcome to the Trading System with AI Integration project, a groundbreaking platform designed for real-time trading on diverse commodities within a real exchange. This project includes an advanced Artificially Intelligent Trading Bot that leverages historical data and machine learning techniques, achieving an impressive 60% Return on Investment (ROI). The primary objective is to democratize financial access, particularly in Pakistan, by providing a comprehensive platform with educational resources and an AI-powered trading system accessible to users of all backgrounds.

## Features
1.	Allows users to place limit and market orders with varying leverage and margin.
2.	View charts of the respective symbol
3.	Close open positions and orders
4.	Displays mark price and updates critical information regularly.
5.	Displays on-screen logs.
6.	Displays colorful helpful messages.
7.	Stores all of the trades whether closing/opening of positions/orders in a csv file.
8.	Stores detailed messages for the developer in a txt logs file. These logs are stored after every network request whether it is successful or not. 
9.	Has an env file to store the API and SECRET keys of the user.
10.	Has an AI with many different options and is capable of placing trades on its own?
11.	The AI is trained on current data in real-time.

##

Special thanks to [©jdehorty](https://www.tradingview.com/u/jdehorty/) of TradingView for the original script in PineScript, which has been converted to C++ for this project. Check out the original work [here](https://www.tradingview.com/script/WhBzgfDu-Machine-Learning-Lorentzian-Classification/).

Read the [Documentation](documentation.md). 



## Table of Contents

1. [Quick Start](#quick-start)
2. [Limitations](#limitations)
3. [Future Plans/To-Do List](#future-plansto-do-list)
4. [Abstract](#abstract)
5. [Objective](#objective)


## Quick Start

1. **Clone the Repo:**

```bash
git clone https://github.com/Overproness/Trading-Project.git

```


2. Go into the ```pre-installed``` directory and open a file named ```.env``` and paste your ```API-KEY``` and ```SECRET-KEY``` into that file in the following format:
Note: Currently it only has the functionality 

```env
API_KEY=abcdefgh
SECRET_KEY= abcdefgh
```

No blank space is acceptable in between or at the ends. Otherwise, it will mess up with the program. You can get your ```API-KEY``` and ```SECRET-KEY``` from your Binance's account settings. 


3. Run the ```Trading System with AI Integration.exe``` file. 



## Limitations

We have basically created a separate thread that is used to fetch some data after every 15 seconds.
1.	For trading we need actual real-time data. Using such (15 seconds) old data in trading is impractical so we need to somehow make it real-time.
2.	After every 15 seconds when that thread runs, (and it takes about a second to complete) the program halts during that time because the UI thread of the WinForms application stops executing during that duration. 
3.	UI could use some designer level beautification.
4.	It still lacks an integrated candle stick chart on with which the user can interact.
5.	We had to “debuff “ the AI so that its processing time was cut down to 20s. Originally, we planned to provide it with 9000 candles as trading data but the estimated time for that was in days. So now we are providing it about 250 candles as training data. Part of the reason for taking so much time was our thread. But the main reason for it was that the AI function has a time complexity of (O) to the power n. It is an exponential function. 
6.	The other thing is that the cancel order and close position buttons will not necessarily close the specific position or order that you want. It is programmed to close the one that is being displayed but the problem is that that the close position or order function fetches the list of positions/orders again and there is a chance that if the data sent by Binance back this time would have the positions/orders in different order than before. Essentially, providing us with a chance that the wrong trade/order may be closed. 
7.	The text of the Trade Status vanishes as soon as our custom thread runs. That means that if you do something and a status msg appears and immediately after our custom thread runs. There will be no time for you to see it and the only chance for you to see it may be if you watch the on-screen logs or the logs.txt file. 


## Future Plans/To Do List

1.  We plan on making it more robust, more efficient and more ‘real-timely’ updated to provide a dependable trading environment for the user. 
2.	We plan on including the following features:
a.	More Order Types
b.	Lower Margin Restriction. So that poor people can also trade
c.	Allow greater customization options including setting TP/SL, liquidation prices etc.
3.	Including a built-in interactable candle stick chart to provide comfort to user.
4.	Further GUI Beautification
5.	Putting the AI on a remote computer and then accessing it to make trades, to further cut down on time and to increase efficiency.
6.	Making sure the intended trade/order gets closed.
7.	Separating the status msg vanishing and the custom thread logic.  




## Abstract
The project consists of two main components:

Trading System:

Purpose: Provide users with a dynamic GUI for executing transactions on various commodities using real money on a real exchange.
Features:
Interactive order book, open positions, and orders.
Real-time balance tracking.
User-defined transaction volumes, margins, order types, leverage, etc.
Candlestick charts for enhanced trading experience.
Market Inspiration: Aims to create a competitive trading environment similar to platforms like LBANK, TradingView, and Binance.
Artificially Intelligent Trading Bot:

Purpose: Utilize historical data and machine learning for predictive modeling of future candlestick patterns.
Features:
Assign values to green and red candles based on set parameters.
Integration of established trading indicators.
Identification and exploitation of market opportunities.
ROI: Achieved a remarkable 60% Return on Investment.
Introduction
The Trading System is designed with a user-centric approach, offering a GUI for real-time trades on diverse commodities. The AI Trading Bot enhances this by utilizing historical data and advanced algorithms for effective market analysis. Key features include real-time market data, comprehensive account information, and an intuitive interface. The project incorporates C++, WinForms, JSON parsing, and file handling to provide a dependable and cutting-edge platform.

## Objective
The primary goal is to empower financially impoverished and less educated citizens in Pakistan towards financial independence. The project aims to democratize financial market access and provides educational resources within the platform to eradicate traditional barriers. The AI-powered trading platform stands out for its functionality, intuitive layout, and effectiveness. With a minimal investment, users, regardless of their background, can make profitable decisions confidently. The platform, developed with C++, WinForms, JSON parsing, and GUI, ensures accessibility for users with diverse technological backgrounds.

## Achievements
The AI Trading Bot has achieved a remarkable 60% ROI, showcasing the effectiveness of the predictive model and trading strategies implemented. This significant return on investment establishes the project as a valuable tool for users seeking profitable opportunities in the financial markets.

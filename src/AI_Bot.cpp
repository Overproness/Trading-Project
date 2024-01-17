#include <iostream>
#include<cmath>
#include<vector>

// Firstly I would like to give ©jdehorty on TradingView credit for making this script. I have just converted it to C++. This was orignially in PineScript, a language very similar to python, syntax-wise. 
//  
// PineScript, on the other hand, is more limited in terms of data structures.It primarily revolves around time - series data, focusing on organizing and manipulating financial market data 
// such as OHLC(open, high, low, close) prices, volume, and other trading - related information.It provides built - in functions and structures specifically tailored for handling financial 
// data, like series, plots, and simple array - like structures for storing historical market data.
// In PineScript, the series data structure is fundamental for handling time-series data, particularly in the context of financial markets. It represents a sequence of values over time, typically 
// comprising historical price data, indicators, or any numerical information related to trading. The series data structure in PineScript is implicitly managed and is designed to handle time-specific 
// data efficiently.
//
//Comparatively, in C++, there isn't a direct equivalent data structure named series. However, C++ provides various data structures that can be used to represent time-series data, such as 
// arrays, vectors, or custom-designed classes.
//
//The primary difference lies in the implicit handling and functionality tailored specifically for financial time - series data that PineScript's series offers. In PineScript, the series data 
// structure incorporates specific built-in functions and behaviors optimized for handling time-series data, making it easy to apply calculations, indicators, and visualize data within the 
// TradingView platform seamlessly.
//
//C++ lacks the specialized built - in features and syntax specifically designed for financial time - series data, unlike PineScript.However, C++ offers more flexibility and control over data 
// structures.Developers can create custom classes or structures in C++ to mimic some functionalities of PineScript's series, but it would require more manual implementation and lack the seamless 
// integration that PineScript offers within the TradingView platform for financial analysis.

//=================================================================

// Explanation of this ML script:
// ====================
// ==== Background ====
// ====================

// When using Machine Learning algorithms like K-Nearest Neighbors, choosing an
// appropriate distance metric is essential. Euclidean Distance is often used as
// the default distance metric, but it may not always be the best choice. This is
// because market data is often significantly impacted by proximity to significant
// world events such as FOMC Meetings and Black Swan events. These major economic
// events can contribute to a warping effect analogous a massive object's 
// gravitational warping of Space-Time. In financial markets, this warping effect 
// operates on a continuum, which can analogously be referred to as "Price-Time".

// To help to better account for this warping effect, Lorentzian Distance can be
// used as an alternative distance metric to Euclidean Distance. The geometry of
// Lorentzian Space can be difficult to visualize at first, and one of the best
// ways to intuitively understand it is through an example involving 2 feature
// dimensions (z=2). For purposes of this example, let's assume these two features
// are Relative Strength Index (RSI) and the Average Directional Index (ADX). In
// reality, the optimal number of features is in the range of 3-8, but for the sake
// of simplicity, we will use only 2 features in this example.

// Fundamental Assumptions:
// (1) We can calculate RSI and ADX for a given chart.
// (2) For simplicity, values for RSI and ADX are assumed to adhere to a Gaussian 
//     distribution in the range of 0 to 100.
// (3) The most recent RSI and ADX value can be considered the origin of a coordinate 
//     system with ADX on the x-axis and RSI on the y-axis.

// Distances in Euclidean Space:
// Measuring the Euclidean Distances of historical values with the most recent point
// at the origin will yield a distribution that resembles Figure 1 (below).
//                        [RSI]
//                          |                      
//                          |                   
//                          |                 
//                      ...:::....              
//                .:.:::••••••:::•::..             
//              .:•:.:•••::::••::••....::.            
//             ....:••••:••••••••::••:...:•.          
//            ...:.::::::•••:::•••:•••::.:•..          
//            ::•:.:•:•••••••:.:•::::::...:..         
//  |--------.:•••..•••••••:••:...:::•:•:..:..----------[ADX]    
//  0        :•:....:•••••::.:::•••::••:.....            
//           ::....:.:••••••••:•••::••::..:.          
//            .:...:••:::••••••••::•••....:          
//              ::....:.....:•::•••:::::..             
//                ..:..::••..::::..:•:..              
//                    .::..:::.....:                
//                          |            
//                          |                   
//                          |
//                          |
//                         _|_ 0        
//                          
//        Figure 1: Neighborhood in Euclidean Space

// Distances in Lorentzian Space:
// However, the same set of historical values measured using Lorentzian Distance will 
// yield a different distribution that resembles Figure 2 (below).
//                         
//                         [RSI] 
//  ::..                     |                    ..:::  
//   .....                   |                  ......
//    .••••::.               |               :••••••. 
//     .:•••••:.             |            :::••••••.  
//       .•••••:...          |         .::.••••••.    
//         .::•••••::..      |       :..••••••..      
//            .:•••••••::.........::••••••:..         
//              ..::::••••.•••••••.•••••••:.            
//                ...:•••••••.•••••••••::.              
//                  .:..••.••••••.••••..                
//  |---------------.:•••••••••••••••••.---------------[ADX]          
//  0             .:•:•••.••••••.•••••••.                
//              .••••••••••••••••••••••••:.            
//            .:••••••••••::..::.::••••••••:.          
//          .::••••••::.     |       .::•••:::.       
//         .:••••••..        |          :••••••••.     
//       .:••••:...          |           ..•••••••:.   
//     ..:••::..             |              :.•••••••.   
//    .:•....                |               ...::.:••.  
//   ...:..                  |                   :...:••.     
//  :::.                     |                       ..::  
//                          _|_ 0
//
//       Figure 2: Neighborhood in Lorentzian Space 


// Observations:
// (1) In Lorentzian Space, the shortest distance between two points is not 
//     necessarily a straight line, but rather, a geodesic curve.
// (2) The warping effect of Lorentzian distance reduces the overall influence  
//     of outliers and noise.
// (3) Lorentzian Distance becomes increasingly different from Euclidean Distance 
//     as the number of nearest neighbors used for comparison increases.

//=====================================================================================

// Since PineScript has a built-in function for calculating getting ohlc values , I have used vectors to store the values of the ohlc values.
// So basically we have four vector<double> arrays for storing the values of open, high, low and close.
// So to get the value of open at a particular index, we can use open[index] and similarly for high, low and close.
// In pineScript the latest value of any of the ohlc series is accessed using the 0 index, but in C++ the latest value is accessed using the last index.
// So if we want to get the latest value of open, we can use open[open.size()-1] and similarly for high, low and close.
// The following functions are used to calculate the technical indicators that are prebuilt in pineScript.

//=====================================================================================

// Returns Bar index of the last chart bar. Bar indices begin at zero on the first bar.
int last_bar_index(std::vector<double> src) {
	return src.size() - 1;
}


//Returns the length of a non-dynamic array.
int arrary_length(bool arr[]) {
	return sizeof(arr) / sizeof(arr[0]);
}

//Removes the first element of a vector and returns the vector.
std::vector<double> array_shift(std::vector<double> src) {
	src.erase(src.begin());
	return src;
}


//Adds the elements of the second vector after the elements of the first vector and returns the first vector.
std::vector<double> array_push_array(std::vector<double> src, std::vector<double> value) {
	src.insert(src.end(), value.begin(), value.end());             //Inserts the elements of value vector at the end of src vector.
	return src;
}


//Returns the maximum value of the two double values.
double math_max(double one, double two) {
	if (one > two)
		return one;                  //The built-in max function cannot deal with double values.
	else
		return two;
}


//Returns the maximum value of the three double values.
double math_maxThree(double one, double two, double three) {
	if (one > two && one > three)
		return one;
	else if (two > one && two > three)                       //The built-in max function cannot deal with double values.
		return two;
	else
		return three;
}


//Returns the minimum value of the two double values.
double math_min(double one, double two) {
	if (one < two)
		return one;
	else                            //The built-in min function cannot deal with double values.
		return  two;
}


//Returns the value of number  raised to the power of e, where e is Euler's number.
double math_exp(double number) {
	double e = 2.71828;
	return pow(e, number);
}


//Returns 0 if the value is NULL, else returns the value.
double nz(double value) {
	if (value == NULL)
		return 0;
	else
		return value;
}


//Returns the second argument, 'replacingValue', if the first argument, 'value', is NULL, else returns the first argument, 'value'.
double nzReplacer(double value, double replacingValue) {
	if (value == NULL)
		return replacingValue;
	else
		return value;
}


//Gets the value of the array at nth index, if the index is out of bounds, returns 0.
double array_get(std::vector<double> src, int index) {
	if (index < src.size())
		return src[index];
	else
		return 0;
}

//Returns true if value is NULL, else returns false.
bool na(double value) {
	return value == NULL;
}


//Returns a bool vector with values ture at any nth index if at that index the value of the first argument is greater than the value of the second argument at the same index
// and on the previous, nth-1 index the value of the first argument is less than the value of the second argument, else returns false at that nth index.
std::vector<bool> ta_crossover(std::vector<double> src1, std::vector<double> src2) {
	int src1Size = src1.size();
	int src2Size = src2.size();
	std::vector<bool> crossover;
	if (src1[src1Size - 1] > src2[src2Size - 1]) {                //Checking the current index
		if (src1[src1Size - 2] <= src2[src2Size - 2])               //Checking the previous index
			crossover.push_back(true);
		else
			crossover.push_back(false);
	}
	else
		crossover.push_back(false);
	return crossover;
}


//Returns a bool vector with values ture at any nth index if at that index the value of the first argument is less than the value of the second argument and on the previous, nth-1 index
// the value of the first argument is greater than the value of the second argument, else returns false at that nth index.
std::vector<bool> ta_crossunder(std::vector<double> src1, std::vector<double> src2) {
	int src1Size = src1.size();
	int src2Size = src2.size();
	std::vector<bool> crossunder;
	if (src1[src1Size - 1] < src2[src2Size - 1]) {
		if (src1[src1Size - 2] >= src2[src2Size - 2])
			crossunder.push_back(true);
		else
			crossunder.push_back(false);
	}
	else
		crossunder.push_back(false);
	return crossunder;
}


//Returns the sum of the values of the vector.
double ta_cum(std::vector<double> src) {
	double sum = 0;
	for (double i : src)
		sum += i;
	return sum;
}


//Returns the values of hlc (Meaning high + low + close) divided by 3 at the provided index.
double ta_hlc3(int location, std::vector<double> high, std::vector<double> low, std::vector<double> close) {
	return (low[location] + high[location] + close[location]) / 3;
}


//Returns the values of ohlc (Meaning open+ high+ low + close) divided by 4 at the provided index.
double ta_ohlc4(int location, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open) {
	return (open[location] + low[location] + high[location] + close[location]) / 4;
}


//Returns a vector in which any index n has the value of hlc3 (high + low + close) divided by 3 at that index.
std::vector<double> array_ta_hlc3(std::vector<double> high, std::vector<double> low, std::vector<double> close) {
	std::vector<double> hlc3;
	for (int i = 0; i < low.size(); i++) {
		hlc3.push_back((high[i] + low[i] + close[i]) / 3);
	}
	return hlc3;
}


//Returns a vector in which any index n has the value of ohlc4 (open + high + low + close) divided by 4 at that index.
std::vector<double> array_ta_ohlc4(std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open) {
	std::vector<double> hlc4;
	for (int i = 0; i < low.size(); i++) {
		hlc4.push_back((high[i] + low[i] + close[i] + open[i]) / 4);
	}
	return hlc4;
}


//Returns a vector containing the sma values according to the given length.
std::vector<double> array_ta_sma(std::vector<double> src, int length) {
	std::vector<double> sma;                             // SMA is the simple moving average
	for (int i = 0; i < src.size(); i++) {             //There are two approaches to calculate the sma, one is to push the sma values after the length-1 index and the other is to push the sma 
		if (i < length - 1)                            //values before the length-1 index. I have used the first approach here. Later I have made use of the 2nd appraoch as well. 
			sma.push_back(src[i]);
		else
		{
			double sum = 0;
			for (int j = 0; j < length; j++) {
				if (i - j >= 0)
					sum += src[i - j];
				else
					sum += src[0];
			}
			sma.push_back(sum / length);
		}
	}
	return sma;
}


//Returns a vector containing the sma values according to the given length.
std::vector<double> arrary_ta_reverse_sma(std::vector<double> src, int length) {
	std::vector<double> sma;
	for (int i = 0; i < src.size() - length + 2; i++) {             //Here I have used the 2nd approach to calculate the sma values.
		double sum = 0;
		for (int j = 0; j < length; j++) {
			if (i + j < src.size())
				sum += src[i + j];
			else
			{
				sum += src[src.size() - 1];
			}
		}
		sma.push_back(double(sum / length));
	}
	for (int i = src.size() - length + 2; i < src.size(); i++) {
		sma.push_back(src[i]);
	}
	return sma;
}


//Returns a vector containing the mean Deviation of the sma values according to the given length.
std::vector<double> array_meanDeviation_ta_sma(std::vector<double> src, int length, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open) {
	std::vector<double> sma = array_ta_sma(src, length);
	std::vector<double> meanDeviation;
	for (int i = 0; i < src.size(); i++) {
		if (i < length - 1)
			meanDeviation.push_back(src[i]);                 // Doesn't do anything if index is less than length
		else {
			double sum = 0;
			for (int j = 0; j < length; j++)
				sum += (ta_ohlc4(i, high, low, close, open) - sma[i]);       //Calculates the mean deviation by subtracting the sma value from the ohlc4 value and then dividing it by length, lenth 
			meanDeviation.push_back(sum / length);                           // amount of times. 
		}
	}
	return meanDeviation;
}


//Returns a vector containing the ema values according to the given length.
std::vector<double> array_ta_ema(std::vector<double> src, int length) {
	double alpha = double(2 / (length + 1));         //There are many versions of ema (Exponential Moving Average) but I have used the one which is used in pineScript.
	double sum = 0;                                  // In that method (and in most of the methods) we take the alpha value as 2/(length+1).
	std::vector<double> ema;                          //EMA is basically a Moving Average but is more sensitive to recent price changes SMA.
	for (int i = 0; i < src.size(); i++) {
		sum = 0;
		if (i < length - 1) {                      //Doesn't do anything if index is less than length
			ema.push_back(src[i]);
		}
		else if (i == length - 1) {            //Calculates the ema value by adding the values of the src vector from index i = length and then dividing it by length, length amount of times.
			for (int j = 0; j < length; j++) {
				if (i - j >= 0)
					sum += src[i - j];
				else
					sum += src[0];
			}
			ema.push_back(sum / length);
		}
		else {
			for (int j = 0; j < length; j++) {       //Calculates the ema value by multiplying the alpha value with the src value and adding it to the previous ema value multiplied by 1 - alpha.
				if (i > 0)
					ema.push_back(src[i] * alpha + ema[i - 1] * (1 - alpha));
				else
					ema.push_back(src[0] * alpha + ema[i] * (1 - alpha));             //If the index is 0, then the previous ema value is 0.
			}
		}
	}
	return ema;
}


//Returns a vector containing the rma values according to the given length.
std::vector<double> array_ta_rma(std::vector<double> src, int length) {
	double alpha = double(1 / (length));        //There are many versions of rma (Relative Moving Average) but I have used the one which is used in pineScript.
	double sum = 0;                                         // In that method (and in most of the methods) we take the alpha value as 1/(length).
	std::vector<double> ema = array_ta_ema(src, length);
	std::vector<double> rma;                           //RMA is basically a Moving Average but is more sensitive to recent price changes than EMA.
	for (int i = 0; i < src.size(); i++) {
		sum = 0;
		if (i < length - 1) {                         //Doesn't do anything if index is less than length
			rma.push_back(src[i]);
		}
		else if (i == length - 1) {
			for (int j = 0; j < length; j++) {        //Calculates the rma value by adding the values of the src vector from index i = length and then dividing it by length, length amount of times.
				if (i - j >= 0)
					sum += src[i - j];
				else
					sum += src[0];          //If-else is to avoid errors for when the index-1 is less than 0.
			}
			rma.push_back(sum / length);
		}
		else {
			for (int j = 0; j < length; j++) {
				if (i > 0)								//Calculates the rma value by multiplying the alpha value with the src value and adding it to the previous ema value multiplied by 1 - alpha.
					rma.push_back(src[i] * alpha + ema[i - 1] * (1 - alpha));     
				else
					rma.push_back(src[0] * alpha + ema[i] * (1 - alpha));
			}
		}
	}
	return rma;

}


//Returns the value of the rma for the given values
double ta_rma(double src, int length, double previous_src) {
	double alpha = double(1 / length);            //Here we have taken the alpha value as 1/length.
	return src * alpha + previous_src * (1 - alpha);      //Calculates the rma value by multiplying the alpha value with the src value and adding it to the previous ema value multiplied by 1 - alpha.

}


//Returns a vector containing the rsi values according to the given length.
std::vector<double> array_ta_rsi(std::vector<double> src, int length) {
	std::vector<double> rsi;  	               //RSI is a momentum indicator used in technical analysis that measures the magnitude of recent price changes to evaluate overbought or oversold 
	for (int i = 0; i < src.size(); i++) {          //conditions in the price of a stock or other asset.
		if (i > 1) {          //Calculates the rsi value by dividing the rma of the positive values by the rma of the negative values and then subtracting it from 100.
			double u = math_max((src[i] - nz(src[i - 1])), 0);
			double d = math_max((nz(src[i - 1]) - src[i]), 0);
			double rs = ta_rma(u, length, math_max((nz(src[i - 1]) - nz(src[i - 2])), 0)) / ta_rma(d, length, math_max((nz(src[i - 2]) - nz(src[i - 1])), 0));
			double  res = 100 - (100 / (1 + rs));
			rsi.push_back(res);
		}
		else if (i == 1)            //Same functionaity. If-else is used to get best results while avoiding errors.
		{            
			double u = math_max((src[i] - nz(src[i - 1])), 0);
			double d = math_max((nz(src[i - 1]) - src[i]), 0);
			double rs = ta_rma(u, length, math_max((nz(src[i - 1]) - nz(src[i])), 0)) / ta_rma(d, length, math_max((nz(src[i]) - nz(src[i - 1])), 0));
			double  res = 100 - (100 / (1 + rs));
			rsi.push_back(res);

		}
		else {                                  //Same functionaity. If-else is used to get best results while avoiding errors.
			double u = math_max((src[i + 1] - nz(src[i])), 0);
			double d = math_max((nz(src[i]) - src[i]), 0);
			double rs = ta_rma(u, length, math_max((nz(src[i]) - nz(src[i])), 0)) / ta_rma(d, length, math_max((nz(src[i]) - nz(src[i])), 0));
			double  res = 100 - (100 / (1 + rs));
			rsi.push_back(res);
		}
	}
	return rsi;
}


//Returns the True Range value for the given values.
double ta_tr(int location, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open) {
	int i = location;  //Calculates the True Range value by taking the maximum of the difference between the high and low values, the difference between the high value and the previous close value
	return math_maxThree(high[i] - low[i], abs(high[i] - nz(close[i - 1])), abs(low[i] - nz(close[i - 1])));   
}                                        //True Range is a measure of volatility introduced by Welles Wilder in his book: New Concepts in Technical Trading Systems.


//Returns a vector containing the Average of True Range values according to the given length.
std::vector<double> array_ta_atr(int length, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open) {
	std::vector<double> atr;   //The Average True Range (ATR) is a technical analysis indicator that measures market volatility by decomposing the entire range of an asset price for that period.
	double sum = 0;
	for (int i = 0; i < length; i++) {
		if (i != 0)                             // If the index is not 0, then we calculate the sum by adding the True Range value at the index i to the sum.
			sum += math_maxThree(high[i] - low[i], abs(high[i] - nz(close[i - 1])), abs(low[i] - nz(close[i - 1])));
		else                                                                                                 //If the index is 0, then the the 0 index is used.
			sum += math_maxThree(high[i] - low[i], abs(high[i] - nz(close[i])), abs(low[i] - nz(close[i])));
	}
	atr.push_back(sum / length);
	for (int i = 1; i < high.size(); i++) {
		if (i > 0)                                 //Here we are finding the average of the true range values that we found above.
			atr.push_back((atr[i - 1] * (length - 1) + ta_tr(i, high, low, close, open)) / length);
		else                                                                 //If-else is used to get best possible results while avoiding errors.
		{
			atr.push_back((atr[i] * (length - 1) + ta_tr(i, high, low, close, open)) / length);
		}
	}
	return atr;
}


//Returns the Average of True Range values according to the given length.
double ta_atr(int length, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open) {
	std::vector<double> atr = array_ta_atr(length, high, low, close, open);
	double ta_atr = 0;                              //Uses the array_ta_atr function to calculate the atr value. 
	for (int i = 0; i < high.size(); i++) {
		ta_atr += atr[i];
	}
	ta_atr /= high.size();                     //Calculates the average of the atr values.
	return ta_atr;
}


//Returns a vector array containing the Commodity Channel Index​ (CCI) values according to the given length. 
std::vector<double> array_ta_cci(std::vector<double> src, int length, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open) {
	std::vector<double> cci;   //The CCI is a momentum-based oscillator used to help determine when an investment vehicle is reaching a condition of being overbought or oversold.
	std::vector<double> sma = array_ta_sma(src, length);        //Calculates the sma values.
	std::vector<double> meanDeviation = array_meanDeviation_ta_sma(src, length, high, low, close, open);        //Calculates the mean deviation values.
	for (int i = 0; i < src.size(); i++) {
		double typicalPrice = ta_ohlc4(i, high, low, close, open);
		cci.push_back((typicalPrice - sma[i]) / (0.015 * meanDeviation[i]));   //Uses this formula for every index to calculate the cci value.
	}
	return cci;
}


//Rescales a source value with a bounded range to anther bounded range
//Parameters :
//src:  The input series
//oldMin : The minimum value of the range to rescale from
//oldMax : The maximum value of the range to rescale from
//newMin : The minimum value of the range to rescale to
//newMax : The maximum value of the range to rescale to
//Returns : The rescaled series
std::vector<double> rescale(std::vector<double> src, double oldMin, double oldMax, double newMin, double newMax) {
	for (int i = 0; i < src.size(); i++)         //Uses this formula to calculate the rescaled value at every index. 
		src[i] = newMin + (newMax - newMin) * (src[i] - oldMin) / math_max(oldMax - oldMin, 10e-10);
	return src;
}


//Rescales a source value with an unbounded range to a target range.
//Parameters:
//src:  The input series
//min : The minimum value of the unbounded range
//max : The maximum value of the unbounded range
//Returns : The normalized series
std::vector<double> normalize(std::vector<double> src, double min, double max) {
	double _historicMin = 10e10;                 //Considered the minimum value to be 10e10.
	double _historicMax = -10e10;                   //Considered the maximum value to be -10e10.
	for (int i = 0; i < src.size(); i++) {
		_historicMin = math_min(nzReplacer(src[i], _historicMin), _historicMin);              //Used the minimum value between these three values.
		_historicMax = math_max(nzReplacer(src[i], _historicMax), _historicMax);                //Used the maximum value between these three values.
		src[i] = min + (max - min) * (src[i] - _historicMin) / math_max(_historicMax - _historicMin, 10e-10);             //Uses this formula to calculate the normalized value at every index.
	}
	return src;
}



//Returns the normalized RSI ideal for use in ML algorithms.
//Parameters:
//src: The input series(i.e., the result of the RSI calculation).
//n1 : The length of the RSI.
//n2 : The smoothing length of the RSI.
//Returns : signal vector<double> The normalized RSI.
std::vector<double> n_rsi(std::vector<double> src, double n1, double n2) {
	std::vector<double> rsi = array_ta_rsi(src, n1);         //Uses the array_ta_rsi function to calculate the rsi values.
	std::vector<double> ema = array_ta_ema(rsi, n2);             //Uses the array_ta_ema function to calculate the ema values.
	std::vector<double> n_rsi = rescale(ema, 0, 100, 0, 1);        //Uses the rescale function to calculate the normalized rsi values.
	return n_rsi;
}



//Returns the normalized WaveTrend Classic series ideal for use in ML algorithms.
//Parameters:
//src: <series float> The input series(i.e., the result of the WaveTrend Classic calculation).
//n1
//n2
//Returns : signal <series float> The normalized WaveTrend Classic series.
std::vector<double> n_wt(std::vector<double> src, double n1, double n2) { //WT is the WaveTrend Classic series. Which tells us if the market is in a bullish or bearish trend.
	std::vector<double> ema1 = array_ta_ema(src, n1);           //Uses the array_ta_ema function to calculate the ema1 values.
	std::vector<double> diff;
	for (int i = 0; i < src.size(); i++) {
		diff.push_back(abs(src[i] - ema1[i]));              //Calculates the difference between the src value and the ema1 value at every index.
	}
	std::vector<double> ema2 = array_ta_ema(diff, n2);                       //Uses the array_ta_ema function to calculate the ema2 values.
	std::vector<double> ci;
	for (int i = 0; i < src.size(); i++) {
		ci.push_back((src[i] - ema1[i]) / (0.015 * ema2[i]));         //Uses this formula to calculate the ci value at every index.
	}
	std::vector<double> wt1 = array_ta_ema(ci, n2);             //Uses the array_ta_ema function to calculate the wt1 values.
	std::vector<double> wt2 = array_ta_ema(wt1, 4);              //Uses the array_ta_ema function to calculate the wt2 values.
	for (int i = 0; i < src.size(); i++) {
		diff[i] = wt1[i] - wt2[i];                          //Calculates the difference between the wt1 value and the wt2 value at every index.
	}
	std::vector<double> wt = normalize(diff, 0, 1);                   //Uses the normalize function to calculate the normalized wt values.
	return wt;
}




//Returns the normalized CCI ideal for use in ML algorithms.
//Parameters:
//src: The input series(i.e., the result of the CCI calculation).
//n1 : The length of the CCI.
//n2 : The smoothing length of the CCI.
//Returns : signal vector<double> The normalized CCI.
std::vector<double> n_cci(std::vector<double> src, double n1, double n2, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open) {
	std::vector<double> cci = array_ta_cci(src, n1, high, low, close, open);           //Uses the array_ta_cci function to calculate the cci values.
	std::vector<double> ema = array_ta_ema(cci, n2);                            //Uses the array_ta_ema function to calculate the ema values.
	std::vector<double> n_cci = normalize(ema, 0, 1);                      //Uses the normalize function to calculate the normalized cci values.
	return n_cci;
}



//Returns the normalized ADX ideal for use in ML algorithms.
//Parameters:
//highSrc: The input series for the high price.
//lowSrc : The input series for the low price.
//closeSrc : The input series for the close price.
//n1 : The length of the ADX.
std::vector<double> n_adx(std::vector<double> high, std::vector<double> low, std::vector<double> close, double n1) {
	double length = n1;
	double th = 20;
	std::vector<double> tr;
	std::vector<double> directionalMovementPlus;
	std::vector<double> negMovement;
	std::vector<double>	diPositive;
	std::vector<double> diNegative;
	std::vector<double> dx;
	std::vector<double> adx;
	for (int i = 0; i < high.size(); i++) {
		if (i > 0) {                          //Calculates the tr, directionalMovementPlus, negMovement, diPositive, diNegative and dx values at every index.
			tr.push_back(math_max(math_max(high[i] - low[i], abs(high[i] - nz(close[i - 1]))), abs(low[i] - nz(close[i - 1]))));                 
			directionalMovementPlus.push_back(high[i] - nz(high[i - 1]) > nz(low[i - 1]) - low[i] ? math_max(high[i] - nz(high[i - 1]), 0) : 0);
			negMovement.push_back(nz(low[i - 1]) - low[i] > high[i] - nz(high[i - 1]) ? math_max(nz(low[i - 1]) - low[i], 0) : 0);
		}
		else            //Goto this link for further understanding of the ADX indicator: 
		{                        //https://www.investopedia.com/articles/trading/07/adx-trend-indicator.asp#:~:text=The%20average%20directional%20index%20(ADX,as%20a%20trend%20strength%20indicator.
			tr.push_back(math_max(math_max(high[i] - low[i], abs(high[i] - nz(close[i]))), abs(low[i] - nz(close[i]))));
			directionalMovementPlus.push_back(high[i] - nz(high[i + 1]) > nz(low[i]) - low[i] ? math_max(high[i] - nz(high[i]), 0) : 0);
			negMovement.push_back(nz(low[i]) - low[i] > high[i] - nz(high[i]) ? math_max(nz(low[i]) - low[i], 0) : 0);
		}
		diPositive.push_back(directionalMovementPlus[i] / tr[i] * 100);
		diNegative.push_back(negMovement[i] / tr[i] * 100);
		dx.push_back(abs(diPositive[i] - diNegative[i]) / (diPositive[i] + diNegative[i]) * 100);
	}
	adx = array_ta_rma(dx, length);
	return rescale(adx, 0, 100, 0, 1);
}


//Custom function to calculate the normalized values of the indicators Depending upon parameters.
//Parameters:
//feature_string: The name of the indicator.
//close: The input series for the close price.
//high: The input series for the high price.
//low: The input series for the low price.
//open: The input series for the open price.
//f_paramA: The first parameter of the indicator.
//f_paramB: The second parameter of the indicator.
//Returns : signal vector<double> The normalized indicator.
std::vector<double> series_from(std::string feature_string, std::vector<double> close, std::vector<double> high, std::vector<double> low, std::vector<double> open, double f_paramA, double f_paramB) {
	if (feature_string == "RSI" || feature_string == "rsi")
		return n_rsi(close, f_paramA, f_paramB);
	if (feature_string == "WT" || feature_string == "wt")
		return n_wt(array_ta_hlc3(high, low, close), f_paramA, f_paramB);
	if (feature_string == "CCI" || feature_string == "cci")
		return n_cci(close, f_paramA, f_paramB, high, low, close, open);
	if (feature_string == "ADX" || feature_string == "adx")
		return n_adx(high, low, close, f_paramA);
}


//Calculates the Lorenzian Distance between the given values.
//Parameters:
//i: The index of the value.
//f1: The first input series.
//f2: The second input series.
//f3: The third input series.
//f4: The fourth input series.
//f5: The fifth input series.
//Returns : signal vector<double> The Lorenzian Distance.
double get_lorentzian_distance(int i, std::vector<double> f1, std::vector<double> f2, std::vector<double> f3, std::vector<double> f4, std::vector<double> f5) {
	return double(log(1 + abs(f1.at(f1.size() - 1) - array_get(f1, i))) +
		log(1 + abs(f2.at(f2.size() - 1) - array_get(f2, i))) +
		log(1 + abs(f3.at(f3.size() - 1) - array_get(f3, i))) +
		log(1 + abs(f4.at(f4.size() - 1) - array_get(f4, i))) +
		log(1 + abs(f5.at(f5.size() - 1) - array_get(f5, i))));
}


//Filters Volatility
//Parameters :
//minLength: <int> The minimum length of the ATR.
//maxLength : <int> The maximum length of the ATR.
//useVolatilityFilter : <bool> Whether to use the volatility filter.
// high : <series float> The input series for the high price.
// low : <series float> The input series for the low price.
// close : <series float> The input series for the close price.
// open : <series float> The input series for the open price.
//Returns : <bool> Boolean indicating whether or not to let the signal pass through the filter.
bool volatility_filter(int minLength, int maxLength, bool useVolatilityFilter, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open) {
	double recentAtr = ta_atr(minLength, high, low, close, open);
	double historicalAtr = ta_atr(maxLength, high, low, close, open);
	return useVolatilityFilter ? recentAtr > historicalAtr:true;
}



//  src <series float> The source series.
// threshold <float> The threshold.
// useRegimeFilter <bool> Whether to use the regime filter.
// returns <bool> Boolean indicating whether or not to let the signal pass through the filter.
bool regime_filter(std::vector<double> src, double threshold, bool useRegimeFilter, std::vector<double> high, std::vector<double> low) {
	// Calculate the slope of the curve.
	std::vector<double> value1(src.size());
	std::vector<double> value2(src.size());
	std::vector<double> klmf(src.size());
	std::vector<double> absCurveSlope(src.size());
	for (int i = 0; i < src.size(); i++) {
		if (i > 0) {
			value1[i] = 0.2 * (src[i] - src[i - 1]) + 0.8 * nz(value1[i - 1]);
			value2[i] = 0.1 * (high[i] - low[i]) + 0.8 * nz(value2[i - 1]);
			double omega = abs(value1[i] / value2[i]);
			double alpha = (-pow(omega, 2) + sqrt(pow(omega, 4) + 16 * pow(omega, 2))) / 8;
			klmf[i] = alpha * src[i] + (1 - alpha) * nz(klmf[i - 1]);
			absCurveSlope[i] = abs(klmf[i] - nz(klmf[i - 1]));
		}
		else {
			value1[i] = 0.2 * (src[i] - src[i]) + 0.8 * nz(value1[i]);
			value2[i] = 0.1 * (high[i] - low[i]) + 0.8 * nz(value2[i]);
			double omega = abs(value1[i] / value2[i]);
			double alpha = (-pow(omega, 2) + sqrt(pow(omega, 4) + 16 * pow(omega, 2))) / 8;
			klmf[i] = alpha * src[i] + (1 - alpha) * nz(klmf[i]);
			absCurveSlope[i] = abs(klmf[i] - nz(klmf[i]));
		}
	}
	std::vector<double> ema = array_ta_ema(absCurveSlope, 200);
	std::vector<double> normalized_slope_decline(src.size());
	for (int i = 0; i < src.size(); i++) {
		double exponentialAverageAbsCurveSlope = 1.0 * ema[i];
		normalized_slope_decline[i] = (absCurveSlope[i] - exponentialAverageAbsCurveSlope) / exponentialAverageAbsCurveSlope;
	}
	// Calculate the slope of the curve.
	std::vector<bool> isSlopePositive;
	for (int i = 0; i < src.size(); i++) {
		if (useRegimeFilter ? normalized_slope_decline[i] >= threshold : true)
			isSlopePositive.push_back(true);
	}
	// Calculate the slope of the curve.
	return isSlopePositive.size() > src.size() / 2;
}


// filters adx
// src The source series.
// length The length of the ADX.
// adxThreshold The ADX threshold.
// useAdxFilter Whether to use the ADX filter.
// returns bool: The ADX.
bool adx_filter(std::vector<double> src, double length, double adxThreshold, bool useAdxFilter, std::vector<double> high, std::vector<double> low) {
	std::vector<double> tr;
	std::vector<double> directionalMovementPlus;
	std::vector<double> negMovement;
	std::vector<double> dx;
	for (int i = 0; i < src.size(); i++) {
		if (i > 0) {
			tr.push_back(math_max(math_max(high[i] - low[i], abs(high[i] - nz(src[i - 1]))), abs(low[i] - nz(src[i - 1]))));
			directionalMovementPlus.push_back(high[i] - nz(high[i - 1]) > nz(low[i - 1]) - low[i] ? math_max(high[i] - nz(high[i - 1]), 0) : 0);
			negMovement.push_back(nz(low[i - 1]) - low[i] > high[i] - nz(high[i - 1]) ? math_max(nz(low[i - 1]) - low[i], 0) : 0);
		}
		else {
			tr.push_back(math_max(math_max(high[i] - low[i], abs(high[i] - nz(src[i]))), abs(low[i] - nz(src[i]))));
			directionalMovementPlus.push_back(high[i] - nz(high[i]) > nz(low[i]) - low[i] ? math_max(high[i] - nz(high[i]), 0) : 0);
			negMovement.push_back(nz(low[i]) - low[i] > high[i] - nz(high[i]) ? math_max(nz(low[i]) - low[i], 0) : 0);
		}
		double diPositive = directionalMovementPlus[i] / tr[i] * 100;
		double diNegative = negMovement[i] / tr[i] * 100;
		dx.push_back(abs(diPositive - diNegative) / (diPositive + diNegative) * 100);
	}
	std::vector<double> adx = array_ta_rma(dx, length);

	std::vector<bool> isSlopePositive;
	for (int i = 0; i < src.size(); i++) {
		if (useAdxFilter ? adx[i] > adxThreshold : true)
			isSlopePositive.push_back(true);
	}
	return (isSlopePositive.size() > src.size() / 2);
}



//Returns true if the given value is different from the previous value. otherwise returns false.
bool change(std::vector<bool> arr, bool value) {
	for (int i = 1; i < arr.size(); i++) {
		if (arr[i] == value) {
			if (arr[i - 1] != value)
				return true;
			else
				return false;
		}
	}
	return false;
}


//Returns the difference between the given value and the previous value.
int int_change(std::vector<int> arr, int value) {
	for (int i = 1; i < arr.size(); i++) {
		if (arr[i] == value) {
			if (i > 0)
				return arr[i] - arr[i - 1];
			else
				return 0;
		}
	}
	return 0;
}


//Allow float for relativeWeight of the Rational Quadratic Kernel
//rationalQuadratic(_src, _lookback, _relativeWeight, _startAtBar)
//Rational Quadratic Kernel - An infinite sum of Gaussian Kernels of different length scales.
//Parameters:
//_src: The source series.
//_lookback : The number of bars used for the estimation.This is a sliding value that represents the most recent historical bars.
//_relativeWeight : Relative weighting of time frames.Smaller values resut in a more stretched out curve and larger values will result in a more wiggly curve.As this value approaches zero, 
//      the longer time frames will exert more influence on the estimation.As this value approaches infinity, the behavior of the Rational Quadratic Kernel will become identical to the Gaussian kernel.
//_startAtBar : Bar index on which to start regression.The first bars of a chart are often highly volatile, and omission of these initial bars often leads to a better overall fit.
//Returns : yhat The estimated values according to the Rational Quadratic Kernel.
std::vector<double> rationalQuadratic(std::vector<double> src, int _lookback, double _relativeWeight, int startAtBar) {
	std::vector<double> yhat;
	double _currentWeight = 0;
	double _cumulativeWeight = 0;
	for (int i = 0; i < src.size(); i++) {
		double y = src[i];
		double w = pow(1 + (pow(i, 2) / ((pow(_lookback, 2) * 2 * _relativeWeight))), -_relativeWeight);
		_currentWeight += y * w;
		_cumulativeWeight += w;
		yhat.push_back(_currentWeight / _cumulativeWeight);
	}
	return yhat;
}



//Gaussian Kernel - A weighted average of the source series.The weights are determined by the Radial Basis Function(RBF).
//Parameters:
//_src: The source series.
//_lookback : The number of bars used for the estimation.This is a sliding value that represents the most recent historical bars.
//_startAtBar : Bar index on which to start regression.The first bars of a chart are often highly volatile, and omitting these initial bars often leads to a better overall fit.
//Returns : yhat The estimated values according to the Gaussian Kernel.
std::vector<double> gaussian(std::vector<double> src, int _lookback, int startAtBar) {
	std::vector<double> yhat;
	double _currentWeight = 0;
	double _cumulativeWeight = 0;
	for (int i = 0; i < src.size(); i++) {
		double y = src[i];
		double w = exp(-pow(i, 2) / (2 * pow(_lookback, 2)));
		_currentWeight += y * w;
		_cumulativeWeight += w;
		yhat.push_back(_currentWeight / _cumulativeWeight);
	}
	return yhat;
}


//Returns the number of bars since the given value was last same.
std::vector<int> array_ta_barssince(std::vector<bool> src) {
	std::vector<int> barssince(src.size(), 0);
	for (int i = 0; i < src.size(); i++) {
		for (int j = src.size() - 2 - i; j >= 0; j--) {
			if (src[src.size() - 1 - i] == src[j])
				barssince[src.size() - 1 - i] = j - i;
		}
	}
	return barssince;
}


//==========================================================================================================================================================================


//This is our main function which has all the logic of the AI.
//Parameters:
//historicalData: The input series.
//Returns : signal vector<bool> The signal vector. This vector contains the buy and sell signals. It has the size of 4. The first value tells if we should open a LONG Trade
//     second value tells if we should close a LONG Trade. The third value tells if we should open a SHORT Trade and the fourth value tells if we should close a SHORT Trade.
std::vector<bool> ai(std::vector<double> historicalData) {

	if (historicalData.size() % 4) {
		historicalData.erase(historicalData.begin());
	}
	if (historicalData.size() % 4) {
		historicalData.erase(historicalData.begin());       //Makes sure that the input data is correct. like how we need it to be
	}
	if (historicalData.size() % 4) {
		historicalData.erase(historicalData.begin());
	}


	std::vector<double> open;
	std::vector<double> close;
	std::vector<double> low;
	std::vector<double> high;

	for (int i = 0; i < historicalData.size(); i += 4) {
		open.push_back((historicalData[i]));
		high.push_back((historicalData[i + 1]));
		low.push_back((historicalData[i + 2]));            //Splits the input data into 4 different vectors. Which will be used for calculations.
		close.push_back((historicalData[i + 3]));
	}

	if (open.size() == 0) {
		std::vector<bool> trades = { false, false, false,false };
		return trades; //If the input data is not correct.
	}

	std::vector<double> hlc3 = array_ta_hlc3(high, low, close);               //Calculates the hlc3 values.
	std::vector<double> ohlc4 = array_ta_ohlc4(high, low, close, open);         //Calculates the ohlc4 values.


	// ================  
// ==== Inputs ==== 
// ================ 

//General User-Defined Inputs


	std::string settings_source = "close";
	int settings_neighborsCount = 8;
	int settings_maxBarsBack = 500;
	int settnigs_featureCount = 5;
	int settings_colorCompression = 1;
	bool settings_showExits = false;
	bool settings_useDynamicExits = false;

	// Trade Stats Settings
// Note: The trade stats section is NOT intended to be used as a replacement for proper backtesting. It is intended to be used for calibration purposes only.
	bool showTradeStats = true;
	bool useWorstCase = false;


	// for user-defined settings
	bool filterSettings_useSmaFilter = true;
	bool filterSettings_useVolatilityFilter = true;
	bool filterSettings_useRegimeFilter = true;
	bool filterSettings_useAdxFilter = true;
	double filterSettings_smaThreshold = 200;
	double filterSettings_regimeThreshold = -0.1;
	double filterSettings_adxThreshold = 20;

	// for filtering the ML predictions
	bool filter_volatility = volatility_filter(1, 10, filterSettings_useVolatilityFilter, high, low, close, open);
	bool filter_regime = regime_filter(ohlc4, filterSettings_regimeThreshold, filterSettings_useRegimeFilter, high, low);
	bool filter_adx = adx_filter(close, 14, filterSettings_adxThreshold, filterSettings_useAdxFilter, high, low);

	// Feature Variables: User-Defined Inputs for calculating Feature Series. 
	std::string f1_string = "RSI";
	std::string f2_string = "WT";
	std::string f3_string = "CCI";
	std::string f4_string = "ADX";
	std::string f5_string = "RSI";

	// Feature Parameters: User-Defined Inputs for calculating Feature Series.
	double f1_paramA = 14;
	double f1_paramB = 1;
	double f2_paramA = 10;
	double f2_paramB = 11;
	double f3_paramA = 20;
	double f3_paramB = 1;
	double f4_paramA = 20;
	double f4_paramB = 2;
	double f5_paramA = 9;
	double f5_paramB = 1;


	// Calculated Feature Series based on Feature Variables
	std::vector<double> f1 = series_from(f1_string, close, high, low, hlc3, f1_paramA, f1_paramB);
	std::vector<double> f2 = series_from(f2_string, close, high, low, hlc3, f2_paramA, f2_paramB);
	std::vector<double> f3 = series_from(f3_string, close, high, low, hlc3, f3_paramA, f3_paramB);
	std::vector<double> f4 = series_from(f4_string, close, high, low, hlc3, f4_paramA, f4_paramB);
	std::vector<double> f5 = series_from(f5_string, close, high, low, hlc3, f5_paramA, f5_paramB);

	// Used for classifying historical data as training data for the ML Model
	double direction_long = 1;
	double direction_short = -1;
	double direction_neutral = 0;


	// Derived from General Settings
	double maxBarsBackIndex = last_bar_index(close) >= settings_maxBarsBack ? last_bar_index(close) - settings_maxBarsBack : 0;

	// EMA Settings 
	bool useEmaFilter = false;
	int emaPeriod = 200;
	bool useSmaFilter = true;
	int smaPeriod = 200;

	// Nadaraya-Watson Kernel Regression Settings
	bool useKernelFilter = true;
	bool showKernelEstimate = true;
	bool useKernelSmoothing = false;
	int h = 8;
	double r = 8;
	int x = 25;
	int lag = 2;

	// Display Settings. AI can be made to display the results of the calculations.
	bool showBarColors = true;
	bool showBarPredictions = true;
	bool useAtrOffset = false;
	double barPredictionsOffset = 0;


	std::vector<bool> isEmaUptrend(4);
	std::vector<bool> isEmaDowntrend(4);
	std::vector<bool> isSmaUptrend(4);
	std::vector<bool> isSmaDowntrend(4);
	std::vector<int> signal;
	std::vector<double> yhat1 = rationalQuadratic(close, h, r, x);
	std::vector<double> yhat2 = gaussian(close, h - lag, x);
	std::vector<double> kernelEstimate = yhat1;
	std::vector<double> y_train_array;
	std::vector<double> closeReplication = close;
	std::vector<double> src = closeReplication;
	std::vector<double> predictions;
	std::vector<double> distances;
	std::vector<bool> startLongTrade;
	std::vector<bool> startShortTrade;
	std::vector<bool> lastSignalWasBullish;
	std::vector<bool> lastSignalWasBearish;
	std::vector<int> barsSinceRedEntry;
	std::vector<int> barsSinceRedExit;
	std::vector<int> barsSinceGreenEntry;
	std::vector<int> barsSinceGreenExit;
	std::vector<bool> isBullishCrossAlert;
	std::vector<bool> isBearishCrossAlert;
	std::vector<bool> alertBullish;
	std::vector<bool> alertBearish;
	std::vector<bool> wasBearishRate;
	std::vector<bool> wasBullishRate;
	std::vector<bool> isBearishRate;
	std::vector<bool> isBullishRate;
	std::vector<bool> isBearishChange;
	std::vector<bool> isBullishChange;
	std::vector<bool> isBullish;
	std::vector<bool> isBearish;
	std::vector<bool> isBullishSmooth;
	std::vector<bool> isBearishSmooth;
	std::vector<bool> isValidShortExit;
	std::vector<bool> isValidLongExit;;
	std::vector<bool> endLongTradeDynamic;
	std::vector<bool> endShortTradeDynamic;
	std::vector<bool> endLongTradeStrict;
	std::vector<bool> endShortTradeStrict;
	std::vector<bool> endLongTrade;
	std::vector<bool> endShortTrade;
	std::vector<int> barsHeld;
	std::vector<bool> isHeldFourBars;
	std::vector<bool> isHeldLessThanFourBars;


	for (int j = 0; j < close.size(); j++) {
		for (int i = 3; i > j; i--) {
			closeReplication.pop_back();
		}
		isEmaUptrend.push_back(useEmaFilter ? closeReplication[last_bar_index(closeReplication)] > array_ta_ema(closeReplication, emaPeriod)[last_bar_index(closeReplication)] : true);
		isEmaDowntrend.push_back(useEmaFilter ? closeReplication[last_bar_index(closeReplication)] < array_ta_ema(closeReplication, emaPeriod)[last_bar_index(closeReplication)] : true);
		isSmaUptrend.push_back(useSmaFilter ? closeReplication[last_bar_index(closeReplication)] > array_ta_sma(closeReplication, smaPeriod)[last_bar_index(closeReplication)] : true);
		isSmaDowntrend.push_back(useSmaFilter ? closeReplication[last_bar_index(closeReplication)] < array_ta_sma(closeReplication, smaPeriod)[last_bar_index(closeReplication)] : true);
		for (int i = 0; i < src.size(); i++) {
			if (i > 3) {
				y_train_array.push_back(nz(src[i - 4]) < src[i] ? direction_short : nz(src[i - 4]) > src[i] ? direction_long : direction_neutral);
			}
			else if (i == 3) {
				y_train_array.push_back(nz(src[i - 3]) < src[i] ? direction_short : nz(src[i - 3]) > src[i] ? direction_long : direction_neutral);
			}
			else if (i == 2) {
				y_train_array.push_back(nz(src[i - 2]) < src[i] ? direction_short : nz(src[i - 2]) > src[i] ? direction_long : direction_neutral);
			}
			else if (i == 1) {
				y_train_array.push_back(nz(src[i - 1]) < src[i] ? direction_short : nz(src[i - 1]) > src[i] ? direction_long : direction_neutral);
			}
			else {
				y_train_array.push_back(nz(src[i]) < src[i] ? direction_short : nz(src[i]) > src[i] ? direction_long : direction_neutral);
			}
		}

			// ==================================
			// ==== Next Bar Classification ====
			// =================================

			// This model specializes specifically in predicting the direction of price action over the course of the next 4 bars. 
			// To avoid complications with the ML model, this value is hardcoded to 4 bars but support for other training lengths may be added in the future.

		double prediction = 0;
		double lastDistance = -1.0;
		double size = math_min(settings_maxBarsBack - 1, y_train_array.size() - 1);
		double sizeLoop = math_min(settings_maxBarsBack - 1, size);
		if (src.size() >= maxBarsBackIndex) {
			for (int i = 0; i < sizeLoop; i++) {
				double d = get_lorentzian_distance(i, f1, f2, f3, f4, f5);
				if (d >= lastDistance && i % 4) {
					lastDistance = d;
					distances.push_back(d);
					predictions.push_back(round(y_train_array[i]));
					if (predictions.size() > settings_neighborsCount) {
						lastDistance = distances[round(settings_neighborsCount * 3 / 4)];
						distances.erase(distances.begin());
						predictions.erase(predictions.begin());
					}
				}
			}
			prediction = ta_cum(predictions);
		}

		// ============================
		// ==== Prediction Filters ====
		// ============================

		// User Defined Filters: Used for adjusting the frequency of the ML Model's predictions

		bool filter_all = filter_volatility && filter_regime && filter_adx;

		// Filtered Signal: The model's prediction of future price movement direction with user-defined filters applied
		if (j == 0) {
			signal.push_back(prediction > 0 && filter_all ? direction_long : prediction < 0 && filter_all ? direction_short : 0);
		}
		else {
			signal.push_back(prediction > 0 && filter_all ? direction_long : prediction < 0 && filter_all ? direction_short : nz(signal[j - 1]));
		}
	}

		// ======================== =
		// ====  Core ML Logic  ====
		// =========================

		// Approximate Nearest Neighbors Search with Lorentzian Distance:
		// A novel variation of the Nearest Neighbors (NN) search algorithm that ensures a chronologically uniform distribution of neighbors.

		// In a traditional KNN-based approach, we would iterate through the entire dataset and calculate the distance between the current bar 
		// and every other bar in the dataset and then sort the distances in ascending order. We would then take the first k bars and use their 
		// labels to determine the label of the current bar. 

		// There are several problems with this traditional KNN approach in the context of real-time calculations involving time series data:
		// - It is computationally expensive to iterate through the entire dataset and calculate the distance between every historical bar and
		//   the current bar.
		// - Market time series data is often non-stationary, meaning that the statistical properties of the data change slightly over time.
		// - It is possible that the nearest neighbors are not the most informative ones, and the KNN algorithm may return poor results if the
		//   nearest neighbors are not representative of the majority of the data.

		// Previously, the user @capissimo attempted to address some of these issues in several of his PineScript-based KNN implementations by:
		// - Using a modified KNN algorithm based on consecutive furthest neighbors to find a set of approximate "nearest" neighbors.
		// - Using a sliding window approach to only calculate the distance between the current bar and the most recent n bars in the dataset.

		// Of these two approaches, the latter is inherently limited by the fact that it only considers the most recent bars in the overall dataset. 

		// The former approach has more potential to leverage historical price action, but is limited by:
		// - The possibility of a sudden "max" value throwing off the estimation
		// - The possibility of selecting a set of approximate neighbors that are not representative of the majority of the data by oversampling 
		//   values that are not chronologically distinct enough from one another
		// - The possibility of selecting too many "far" neighbors, which may result in a poor estimation of price action

		// To address these issues, a novel Approximate Nearest Neighbors (ANN) algorithm is used in this indicator.

		// In the below ANN algorithm:
		// 1. The algorithm iterates through the dataset in chronological order, using the modulo operator to only perform calculations every 4 bars.
		//    This serves the dual purpose of reducing the computational overhead of the algorithm and ensuring a minimum chronological spacing 
		//    between the neighbors of at least 4 bars.
		// 2. A list of the k-similar neighbors is simultaneously maintained in both a predictions array and corresponding distances array.
		// 3. When the size of the predictions array exceeds the desired number of nearest neighbors specified in settings.neighborsCount, 
		//    the algorithm removes the first neighbor from the predictions array and the corresponding distance array.
		// 4. The lastDistance variable is overriden to be a distance in the lower 25% of the array. This step helps to boost overall accuracy 
		//    by ensuring subsequent newly added distance values increase at a slower rate.
		// 5. Lorentzian distance is used as a distance metric in order to minimize the effect of outliers and take into account the warping of 
		//    "price-time" due to proximity to significant economic events.

	for (int j = 0; j < close.size()/2; j++) {
		// Bar-Count Filters: Represents strict filters based on a pre-defined holding period of 4 bars
		if (j == 0) {
			barsHeld.push_back(int_change(signal, signal[j]) ? 0 : 1);
		}
		else {
			barsHeld.push_back(int_change(signal, signal[j]) ? 0 : barsHeld[j - 1] + 1);
		}
		isHeldFourBars.push_back(barsHeld[j] == 4);
		isHeldLessThanFourBars.push_back(0 < barsHeld[j] && barsHeld[j] < 4);

		// Fractal Filters: Derived from relative appearances of signals in a given time series fractal/segment with a default length of 4 bars
		int isDifferentSignalType[1];
		bool isBuySignal[1];
		bool isSellSignal[1];
		if (j > 3) {
			isDifferentSignalType[0] = int_change(signal, signal[j - 3]);
			bool isEarlySignalFlip = int_change(signal, signal[j - 3]) && (int_change(signal, signal[j - 2]) || int_change(signal, signal[j - 1]) || int_change(signal, signal[j]));
			isBuySignal[0] = signal[j - 4] == direction_long && isEmaUptrend[j - 3] && isSmaUptrend[j - 3];
			isSellSignal[0] = signal[j - 4] == direction_short && isEmaDowntrend[j - 3] && isSmaDowntrend[j - 3];
		}
		else if (j == 3) {
			isDifferentSignalType[0] = int_change(signal, signal[j - 3]);
			bool isEarlySignalFlip = int_change(signal, signal[j - 3]) && (int_change(signal, signal[j - 2]) || int_change(signal, signal[j - 1]) || int_change(signal, signal[j]));
			isBuySignal[0] = signal[j - 3] == direction_long && isEmaUptrend[j - 3] && isSmaUptrend[j - 3];
			isSellSignal[0] = signal[j - 3] == direction_short && isEmaDowntrend[j - 3] && isSmaDowntrend[j - 3];
		}
		else if (j == 2) {
			isDifferentSignalType[0] = int_change(signal, signal[j - 2]);
			bool  isEarlySignalFlip = int_change(signal, signal[j - 2]) && (int_change(signal, signal[j - 2]) || int_change(signal, signal[j - 1]) || int_change(signal, signal[j]));
			isBuySignal[0] = signal[j - 2] == direction_long && isEmaUptrend[j - 2] && isSmaUptrend[j - 2];
			isSellSignal[0] = signal[j - 2] == direction_short && isEmaDowntrend[j - 2] && isSmaDowntrend[j - 2];
		}
		else if (j == 1) {
			isDifferentSignalType[0] = int_change(signal, signal[j - 1]);
			bool isEarlySignalFlip = int_change(signal, signal[j - 1]) && (int_change(signal, signal[j - 1]) || int_change(signal, signal[j - 1]) || int_change(signal, signal[j]));
			isBuySignal[0] = signal[j - 1] == direction_long && isEmaUptrend[j - 1] && isSmaUptrend[j - 1];
			isSellSignal[0] = signal[j - 1] == direction_short && isEmaDowntrend[j - 1] && isSmaDowntrend[j - 1];
		}
		else {
			isDifferentSignalType[0] = int_change(signal, signal[j]);
			bool isEarlySignalFlip = int_change(signal, signal[j]) && (int_change(signal, signal[j]) || int_change(signal, signal[j]) || int_change(signal, signal[j]));
			isBuySignal[0] = signal[j] == direction_long && isEmaUptrend[j] && isSmaUptrend[j];
			isSellSignal[0] = signal[j] == direction_short && isEmaDowntrend[j] && isSmaDowntrend[j];
		}

		// Kernel Regression Filters: Filters based on Nadaraya-Watson Kernel Regression using the Rational Quadratic Kernel
		// For more information on this technique refer to my other open source indicator located here: 
		// https://www.tradingview.com/script/AWNvbPRM-Nadaraya-Watson-Rational-Quadratic-Kernel-Non-Repainting/
		// Note: The kernel regression filters are not used in the current version of the AI, but may be used in future versions.
		bool isLastSignalBuy = signal[j] == direction_long && isEmaUptrend[j] && isSmaUptrend[j];
		bool isLastSignalSell = signal[j] == direction_short && isEmaDowntrend[j] && isSmaDowntrend[j];
		bool isNewBuySignal = isBuySignal[0] && isDifferentSignalType[0];
		bool isNewSellSignal = isSellSignal[0] && isDifferentSignalType[0];

		// Kernel Rates of Change
		if (j == close.size() - 1) {
			wasBearishRate.push_back(yhat1.at(yhat1.size() - j - 1) > yhat1.at(yhat1.size() - j - 0));
			wasBullishRate.push_back(yhat1.at(yhat1.size() - j - 1) < yhat1.at(yhat1.size() - j - 0));
			isBearishRate.push_back(yhat1.at(yhat1.size() - j - 1) > yhat1.at(yhat1.size() - j - 0));
			isBullishRate.push_back(yhat1.at(yhat1.size() - j - 1) < yhat1.at(yhat1.size() - j - 0));
		}
		else if (j == close.size() - 2) {
			wasBearishRate.push_back(yhat1.at(yhat1.size() - j - 2) > yhat1.at(yhat1.size() - j - 1));
			wasBullishRate.push_back(yhat1.at(yhat1.size() - j - 2) < yhat1.at(yhat1.size() - j - 1));
			isBearishRate.push_back(yhat1.at(yhat1.size() - j - 2) > yhat1.at(yhat1.size() - j - 1));
			isBullishRate.push_back(yhat1.at(yhat1.size() - j - 2) < yhat1.at(yhat1.size() - j - 1));
		}
		else {
			wasBearishRate.push_back(yhat1.at(yhat1.size() - j - 3) > yhat1.at(yhat1.size() - j - 2));
			wasBullishRate.push_back(yhat1.at(yhat1.size() - j - 3) < yhat1.at(yhat1.size() - j - 2));
			isBearishRate.push_back(yhat1.at(yhat1.size() - j - 2) > yhat1.at(yhat1.size() - j - 1));
			isBullishRate.push_back(yhat1.at(yhat1.size() - j - 2) < yhat1.at(yhat1.size() - j - 1));
		}

		// Kernel Crossovers
		isBearishChange.push_back(wasBullishRate[j] && isBearishRate[j]);
		isBullishChange.push_back(wasBearishRate[j] && isBullishRate[j]);
		isBullishCrossAlert = ta_crossover(yhat1, yhat2);
		isBearishCrossAlert = ta_crossunder(yhat1, yhat2);
		isBullishSmooth.push_back(yhat2.at(yhat2.size() - j - 1) >= yhat1.at(yhat1.size() - j - 1));
		isBearishSmooth.push_back(yhat2.at(yhat2.size() - j - 1) <= yhat1.at(yhat1.size() - j - 1));

		// Alert Variables
		alertBullish.push_back(useKernelSmoothing ? isBullishCrossAlert[j] : isBullishChange[j]);
		alertBearish.push_back(useKernelSmoothing ? isBearishCrossAlert[j] : isBearishChange[j]);

		// Bullish and Bearish Filters based on Kernel
		isBullish.push_back(useKernelFilter ? (useKernelSmoothing ? isBullishSmooth[j] : isBullishRate[j]) : true);
		isBearish.push_back(useKernelFilter ? (useKernelSmoothing ? isBearishSmooth[j] : isBearishRate[j]) : true);

		// ===========================
		// ==== Entries and Exits ====
		// ===========================

		// Entry Conditions: Booleans for ML Model Position Entries

		startLongTrade.push_back(isNewBuySignal && isBullish[j] && isEmaUptrend.at(isEmaUptrend.size() - j - 1) && isSmaUptrend.at(isSmaUptrend.size() - j - 1));
		startShortTrade.push_back(isNewSellSignal && isBearish[j] && isEmaDowntrend.at(isEmaDowntrend.size() - j - 1) && isSmaDowntrend.at(isSmaDowntrend.size() - j - 1));

		// Dynamic Exit Conditions: Booleans for ML Model Position Exits based on Fractal Filters and Kernel Regression Filters
		lastSignalWasBullish.push_back(array_ta_barssince(startLongTrade)[j] < array_ta_barssince(startShortTrade)[j]);
		lastSignalWasBearish.push_back(array_ta_barssince(startShortTrade)[j] < array_ta_barssince(startLongTrade)[j]);
		barsSinceRedEntry.push_back(array_ta_barssince(startShortTrade)[j]);
		barsSinceRedExit.push_back(array_ta_barssince(alertBullish)[j]);
		barsSinceGreenEntry.push_back(array_ta_barssince(startLongTrade)[j]);
		barsSinceGreenExit.push_back(array_ta_barssince(alertBearish)[j]);
		isValidShortExit.push_back(barsSinceRedExit[j] > barsSinceRedEntry[j]);
		isValidLongExit.push_back(barsSinceGreenExit[j] > barsSinceGreenEntry[j]);
		if (j == 0) {
			endLongTradeDynamic.push_back((isBearishChange[j] && isValidLongExit[j]));
			endShortTradeDynamic.push_back((isBullishChange[j] && isValidShortExit[j]));
		}
		else {
			endLongTradeDynamic.push_back((isBearishChange[j] && isValidLongExit[j - 1]));
			endShortTradeDynamic.push_back((isBullishChange[j] && isValidShortExit[j - 1]));
		}

		if (j > 3) {
			endLongTradeStrict.push_back(((isHeldFourBars[j] && isLastSignalBuy) || (isHeldLessThanFourBars[j] && isNewSellSignal && isLastSignalBuy)) && startLongTrade[j - 4]);
			endShortTradeStrict.push_back(((isHeldFourBars[j] && isLastSignalSell) || (isHeldLessThanFourBars[j] && isNewBuySignal && isLastSignalSell)) && startShortTrade[j - 4]);
		}
		else if (j == 3) {
			endLongTradeStrict.push_back(((isHeldFourBars[j] && isLastSignalBuy) || (isHeldLessThanFourBars[j] && isNewSellSignal && isLastSignalBuy)) && startLongTrade[j - 3]);
			endShortTradeStrict.push_back(((isHeldFourBars[j] && isLastSignalSell) || (isHeldLessThanFourBars[j] && isNewBuySignal && isLastSignalSell)) && startShortTrade[j - 3]);
		}
		else if (j == 2) {
			endLongTradeStrict.push_back(((isHeldFourBars[j] && isLastSignalBuy) || (isHeldLessThanFourBars[j] && isNewSellSignal && isLastSignalBuy)) && startLongTrade[j - 2]);
			endShortTradeStrict.push_back(((isHeldFourBars[j] && isLastSignalSell) || (isHeldLessThanFourBars[j] && isNewBuySignal && isLastSignalSell)) && startShortTrade[j - 2]);
		}
		else if (j == 1) {
			endLongTradeStrict.push_back(((isHeldFourBars[j] && isLastSignalBuy) || (isHeldLessThanFourBars[j] && isNewSellSignal && isLastSignalBuy)) && startLongTrade[j - 1]);
			endShortTradeStrict.push_back(((isHeldFourBars[j] && isLastSignalSell) || (isHeldLessThanFourBars[j] && isNewBuySignal && isLastSignalSell)) && startShortTrade[j - 1]);
		}
		else {
			endLongTradeStrict.push_back(((isHeldFourBars[j] && isLastSignalBuy) || (isHeldLessThanFourBars[j] && isNewSellSignal && isLastSignalBuy)) && startLongTrade[j]);
			endShortTradeStrict.push_back(((isHeldFourBars[j] && isLastSignalSell) || (isHeldLessThanFourBars[j] && isNewBuySignal && isLastSignalSell)) && startShortTrade[j]);
		}
		bool isDynamicExitValid = (!useEmaFilter && !useSmaFilter && !useKernelSmoothing);


		// Fixed Exit Conditions: Booleans for ML Model Position Exits based on a Bar-Count Filters
		endLongTrade.push_back(settings_useDynamicExits && isDynamicExitValid ? endLongTradeDynamic[j] : endLongTradeStrict[j]);
		endShortTrade.push_back(settings_useDynamicExits && isDynamicExitValid ? endShortTradeDynamic[j] : endShortTradeStrict[j]);

	}
	
	//This is out main boolean vector that returns true to indicate the opening and closing of a trade. And false to indicate that we should do nothing for the time being.
	std::vector<bool> trades = { startLongTrade.at(startLongTrade.size() - 1), startShortTrade.at(startShortTrade.size() - 1), endLongTrade.at(endLongTrade.size() - 1), endShortTrade.at(endShortTrade.size() - 1) };

	return trades;
}

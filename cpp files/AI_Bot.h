
//Parameter:
//src: vector of double
//Return:
//int
int last_bar_index(std::vector<double> src);


//Parameter:
//src: array of bool
//Return:
//int
int arrary_length(bool arr[]);


//Parameter:
//src: vector of double
//Return:
//vector of double
std::vector<double> array_shift(std::vector<double> src);


//Parameter:
//src: vector of double
//value: vector of double
//Return:
//vector of double
std::vector<double> array_push_array(std::vector<double> src, std::vector<double> value);


//Parameter:
//one: double
//two: double
//Return:
//double
double math_max(double one, double two);


//Parameter:
//one: double
//two: double
//three: double
//Return:
//double
double math_maxThree(double one, double two, double three);


//Parameter:
//one: double
//two: double
//Return:
//double
double math_min(double one, double two);


//Parameter:
//number: double
//Return:
//double
double math_exp(double number);


//Parameter:
//value: double
//Return:
//double
double nz(double value);


//Parameter:
//value: double
//replacingValue: double
//Return:
//double
double nzReplacer(double value, double replacingValue);


//Parameter:
//src: vector of double
//index: int
//Return:
//double
double array_get(std::vector<double> src, int index);


//Parameter:
//value: double
//Return:
//bool
bool na(double value);


//Parameter:
//src1: vector of double
//src2: vector of double
//Return:
//vector of bool
std::vector<bool> ta_crossover(std::vector<double> src1, std::vector<double> src2);


//Parameter:
//src1: vector of double
//src2: vector of double
//Return:
//vector of bool
std::vector<bool> ta_crossunder(std::vector<double> src1, std::vector<double> src2);


//Parameter:
//src: vector of double
//Return:
//double
double ta_cum(std::vector<double> src);


//Parameter:
//location: int
//high: vector of double
//low: vector of double
//close: vector of double
//Return:
//double
double ta_hlc3(int location, std::vector<double> high, std::vector<double> low, std::vector<double> close);


//Parameter:
//location: int
//high: vector of double
//low: vector of double
//close: vector of double
//open: vector of double
//Return:
//double
double ta_ohlc4(int location, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open);


//Parameter:
//high: vector of double
//low: vector of double
//close: vector of double
//Return:
//vector of double
std::vector<double> array_ta_hlc3(std::vector<double> high, std::vector<double> low, std::vector<double> close);


//Parameter:
//high: vector of double
//low: vector of double
//close: vector of double
//open: vector of double
//Return:
//vector of double
std::vector<double> array_ta_ohlc4(std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open);


//Parameter:
//src: vector of double
//length: int
//Return:
//vector of double
std::vector<double> array_ta_sma(std::vector<double> src, int length);


//Parameter:
//src: vector of double
//length: int
//Return:
//vector of double
std::vector<double> arrary_ta_reverse_sma(std::vector<double> src, int length);


//Parameter:
//src: vector of double
//length: int
//high: vector of double
//low: vector of double
//close: vector of double
//open: vector of double
//Return:
//vector of double
std::vector<double> array_meanDeviation_ta_sma(std::vector<double> src, int length, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open);


//Parameter:
//src: vector of double
//length: int
//Return:
//vector of double
std::vector<double> array_ta_ema(std::vector<double> src, int length);


//Parameter:
//src: vector of double
//length: int
//Return:
//vector of double
std::vector<double> array_ta_rma(std::vector<double> src, int length);


//Parameter:
//src: double
//length: int
//previous_src: double
//Return:
//double
double ta_rma(double src, int length, double previous_src);


//Parameter:
//src: vector of double
//length: int
//Return:
//vector of double
std::vector<double> array_ta_rsi(std::vector<double> src, int length);


//Parameter:
//location: int
//high: vector of double
//low: vector of double
//close: vector of double
//open: vector of double
//Return:
//double
double ta_tr(int location, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open);


//Parameter:
//length: int
//high: vector of double
//low: vector of double
//close: vector of double
//open: vector of double
//Return:
//vector of double
std::vector<double> array_ta_atr(int length, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open);


//Parameter:
//length: int
//high: vector of double
//low: vector of double
//close: vector of double
//open: vector of double
//Return:
//double
double ta_atr(int length, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open);


//Parameter:
//src: vector of double
//length: int
//high: vector of double
//low: vector of double
//close: vector of double
//open: vector of double
//Return:
//vector of double
std::vector<double> array_ta_cci(std::vector<double> src, int length, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open);


//Parameter:
//src: vector of double
//oldMin: double
//oldMax: double
//newMin: double
//newMax: double
//Return:
//vector of double
std::vector<double> rescale(std::vector<double> src, double oldMin, double oldMax, double newMin, double newMax);


//Parameter:
//src: vector of double
//min: double
//max: double
//Return:
//vector of double
std::vector<double> normalize(std::vector<double> src, double min, double max);


//Parameter:
//src: vector of double
//n1: double
//n2: double
//Return:
//vector of double
std::vector<double> n_rsi(std::vector<double> src, double n1, double n2);


//Parameter:
//src: vector of double
//n1: double
//n2: double
//Return:
//vector of double
std::vector<double> n_wt(std::vector<double> src, double n1, double n2);


//Parameter:
//src: vector of double
//n1: double
//n2: double
//high: vector of double
//low: vector of double
//close: vector of double
//open: vector of double
//Return:
//vector of double
std::vector<double> n_cci(std::vector<double> src, double n1, double n2, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open);


//Parameter:
//high: vector of double
//low: vector of double
//close: vector of double
//n1: double
//Return:
//vector of double
std::vector<double> n_adx(std::vector<double> high, std::vector<double> low, std::vector<double> close, double n1);


//Parameter:
//feature_string: string
//close: vector of double
//high: vector of double
//low: vector of double
//open: vector of double
//f_paramA: double
//f_paramB: double
//Return:
//vector of double
std::vector<double> series_from(std::string feature_string, std::vector<double> close, std::vector<double> high, std::vector<double> low, std::vector<double> open, double f_paramA, double f_paramB);


//Parameter:
//i: int
//f1: vector of double
//f2: vector of double
//f3: vector of double
//f4: vector of double
//f5: vector of double
//Return:
//double
double get_lorentzian_distance(int i, std::vector<double> f1, std::vector<double> f2, std::vector<double> f3, std::vector<double> f4, std::vector<double> f5);


//Parameter:
//minLength: int
//maxLength: int
//useVolatilityFilter: bool
//high: vector of double
//low: vector of double
//close: vector of double
//open: vector of double
//Return:
//bool
bool volatility_filter(int minLength, int maxLength, bool useVolatilityFilter, std::vector<double> high, std::vector<double> low, std::vector<double> close, std::vector<double> open);


//Parameter:
//src: vector of double
//threshold: double
//useRegimeFilter: bool
//high: vector of double
//low: vector of double
//Return:
//bool
bool regime_filter(std::vector<double> src, double threshold, bool useRegimeFilter, std::vector<double> high, std::vector<double> low);


//Parameter:
//src: vector of double
//length: double
//adxThreshold: double
//useAdxFilter: bool
//high: vector of double
//low: vector of double
//Return:
//bool
bool adx_filter(std::vector<double> src, double length, double adxThreshold, bool useAdxFilter, std::vector<double> high, std::vector<double> low);


//Parameter:
//arr: vector of bool
//value: bool
//Return:
//bool
bool change(std::vector<bool> arr, bool value);


//Parameter:
//arr: vector of int
//value: int
//Return:
//int
int int_change(std::vector<int> arr, int value);


//Parameter:
//src: vector of double
//lookback: int
//_relativeWeight: double
//startAtBar: int
//Return:
//vector of double
std::vector<double> rationalQuadratic(std::vector<double> src, int _lookback, double _relativeWeight, int startAtBar);


//Parameter:
//src: vector of double
//lookback: int
//startAtBar: int
//Return:
//vector of double
std::vector<double> gaussian(std::vector<double> src, int _lookback, int startAtBar);


//Parameter:
//src: vector of bool
//Return:
//vector of int
std::vector<int> array_ta_barssince(std::vector<bool> src);


//Parameter:
//src: vector of double
//Return:
//vector of double
std::vector<bool> ai(std::vector<double> historicalData);
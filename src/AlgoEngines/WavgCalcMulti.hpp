//============================================================================
// Name        	: WavgCalcMulti.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 1, 2023 10:21:50 AM
//============================================================================

#ifndef SRC_WEIGHTEDAVERAGECALCULATOR2_HPP_
#define SRC_WEIGHTEDAVERAGECALCULATOR2_HPP_

#include <string>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <iostream>
#include <unordered_map>

#include <KT01/Core/Log.hpp>

#include <AlgoCalculators/TimerCallback.hpp>
#include <AlgoEngines/MatrixGen.hpp>

#include <Orders/OrderEnumsV2.hpp>
using namespace KTN::ORD;

#include <KT01/DataStructures/DepthBook.hpp>
using namespace KT01::DataStructures::MarketDepth;



using namespace std;

/**
 * @class WavgCalcMulti
 * @brief Class for calculating weighted averages and performing related operations.
 */
class WavgCalcMulti {
public:
    /**
     * @brief Constructor for WavgCalcMulti class.
     * @param cb The TimerCallback object to be used for timing operations.
     */
    WavgCalcMulti(TimerCallback & cb);

    /**
     * @brief Test function for WavgCalcMulti class.
     */
    void Test();

    /**
     * @brief Start the calculation process.
     */
    void Start();

    /**
     * @brief Stop the calculation process.
     */
    void Stop();

    /**
     * @brief Set the start and end time for the calculation.
     * @param startTime The start time in string format.
     * @param endTime The end time in string format.
     * @return True if the start and end time are successfully set, false otherwise.
     */
    bool setStartEndTime(const std::string& startTime, const std::string& endTime);

    /**
     * @brief Perform the calculation for a given set of parameters.
     * @param n The value of n.
     * @param j The value of j.
     * @param price The price value.
     * @param size The size value.
     */
    void calculate(int n, int j, int64_t price, int32_t size);

    /**
     * @brief Calculate the settlements.
     */
    void CalculateSettlements();

    /**
     * @brief Calculate the Stpr value for a given n.
     * @param n The value of n.
     * @return The calculated Stpr value.
     */
    double CalcStpr(int n);

    /**
     * @brief Get the current time in string format.
     * @return The current time in string format.
     */
    std::string getCurrentTime();

    /**
     * @brief Get the Stpr value for a given n and j.
     * @param n The value of n.
     * @param j The value of j.
     * @return The Stpr value.
     */
    double getStpr(int n, int j);

    /**
     * @brief Get the weighted average value for a given n and j.
     * @param n The value of n.
     * @param j The value of j.
     * @return The weighted average value.
     */
    double getWavg(int n, int j);

    /**
     * @brief Get the volume value for a given n and j.
     * @param n The value of n.
     * @param j The value of j.
     * @return The volume value.
     */
    double getVolume(int n, int j);

    /**
     * @brief Get the count value for a given n and j.
     * @param n The value of n.
     * @param j The value of j.
     * @return The count value.
     */
    double getCount(int n, int j);

    /**
     * @brief Check if the calculation is triggered.
     * @return True if the calculation is triggered, false otherwise.
     */
    bool Triggered();

    /**
     * @brief Check if the calculation is started.
     * @return True if the calculation is started, false otherwise.
     */
    bool Started();

    /**
     * @brief Initialize the WavgCalcMulti object with the given parameters.
     * @param product The product string.
     * @param startmonth The start month string.
     * @param startyear The start year value.
     * @param nummonths The number of months value.
     */
    void Init(string product, string startmonth, int startyear, int nummonths);

private:
    /**
     * @brief Log a message with optional color.
     * @param szMsg The message to be logged.
     * @param iColor The color of the log message (default: LOG_WHITE).
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

    /**
     * @brief Format the given time value.
     * @param currentTime The time value to be formatted.
     * @return The formatted time string.
     */
    std::string formatTime(std::time_t currentTime);

    /**
     * @brief Timer thread function.
     * @param arg The argument for the timer thread.
     * @return The result of the timer thread.
     */
    static void *timer_thread(void * arg);

    /**
     * @brief Timer loop function.
     */
    void TimerLoop();

    /**
     * @brief Generate Stpr pairs.
     */
    void GenStprPairs();

    /**
     * @brief Register a leg by row and column with the given product type.
     * @param n The value of n.
     * @param j The value of j.
     * @param prodtype The product type.
     */
    void RegisterLegByRowCol(int n, int j, KOrderProdType::Enum prodtype);

    /**
     * @brief Calculate the TV value for a given j.
     * @param j The value of j.
     * @return The TV value.
     */
    double TV( int j);

    /**
     * @brief Calculate the WF value for a given n and j.
     * @param n The value of n.
     * @param j The value of j.
     * @return The WF value.
     */
    double WF(int n, int j);

    /**
     * @brief Calculate the MDist value for a given n and j.
     * @param n The value of n.
     * @param j The value of j.
     * @return The MDist value.
     */
    inline int MDist(int n, int j)
    {
					return((j - n) + 1);
    }

private:
    /**
     * @struct wavelet
     * @brief Struct for storing wavelet information.
     */
    struct wavelet
	{
					double wavg; /**< The weighted average value. */
					double wsum; /**< The weighted sum value. */
					int volume; /**< The volume value. */
					int count; /**< The count value. */
					int row; /**< The row value. */
					int col; /**< The column value. */
					string symb; /**< The symbol string. */
					double stpr; /**< The Stpr value. */
	};

    /**
     * @struct stprpair
     * @brief Struct for storing Stpr pairs.
     */
    struct stprpair
    {
						std::pair<int,int> m1; /**< The first pair. */
			std::pair<int, int> m2; /**< The second pair. */
			int cnt; /**< The count value. */

						/**
       * @brief Add a pair to the Stpr pair.
       * @param p The pair to be added.
       */
						void add(std::pair<int,int> p)
						{
							if (cnt == 0){
								m1 = p;
							}
							else{
								m2 = p;
							}

				cnt++;
						}
    };

    /**
     * @struct pair_hash
     * @brief Struct for hashing pairs.
     */
    struct pair_hash {
         template <class T1, class T2>
         std::size_t operator () (const std::pair<T1, T2>& p) const {
             auto hash1 = std::hash<T1>{}(p.first);
             auto hash2 = std::hash<T2>{}(p.second);
             return hash1 ^ hash2;
         }
					};

    vector<stprpair> _stpairs[24]; /**< List of map coords/pairs for quick Stpr calculation and iteration. */

    TimerCallback& _cb; /**< The TimerCallback object. */
    volatile bool _bActive; /**< Flag indicating if the calculation is active. */
    volatile bool _bTimerTriggered; /**< Flag indicating if the timer is triggered. */
    std::string StartTime; /**< The start time string. */
    std::string EndTime; /**< The end time string. */
    std::unordered_map<std::pair<int,int>, wavelet, pair_hash> _waves; /**< The map of wavelets. */

    const int _INFINITE_COL = 24; /**< The infinite column value. */
    map<pair<int, int>, std::string> m_GraphMap; /**< The map of graph coordinates. */
    int _indices[24][24]; /**< The indices array. */
    DepthBook _mdimp[24][24]; /**< The mdimp array. */
    int _rows; /**< The number of rows. */
    int _cols; /**< The number of columns. */
    const int INF = 1e9; /**< The infinite value. */

    instrument _Legs[36]; /**< The array of legs. */
    int _LegCount; /**< The leg count value. */
    int _NumMonths; /**< The number of months value. */

    std::unordered_map<int, std::pair<int,int>> _IndexToPathMap; /**< The map of index to path. */

    /**
     * @brief Get the path name for the given row and column.
     * @param row The row value.
     * @param col The column value.
     * @return The path name.
     */
    inline string pn(int row, int col)
    {
        ostringstream oss;
        oss << "(" << row <<"," << col << ")";
        return oss.str();
    }
};

#endif /* SRC_ALGOCALCULATORS_WEIGHTEDAVERAGECALCULATOR_HPP_ */

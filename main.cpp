#ifndef __PROGTEST__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <queue>
#include <stack>
#include <algorithm>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#if defined (__cplusplus) && __cplusplus > 199711L /* C++ 11 */

#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>

#endif /* C++ 11*/
using namespace std;

struct CExchange {
	CExchange(int id,
	          const string& from,
	          const string& to)
			: m_ProblemID(id), m_From(from), m_To(to),
			  m_BankIdx(0), m_Result(false), m_Rate(0)
	{ }

	void AddBank(const string& bank)
	{
		m_Banks.push_back(bank);
	}

	const int m_ProblemID;
	const string m_From;
	const string m_To;
	vector<string> m_Banks;

	int m_BankIdx;
	bool m_Result;
	vector<string> m_Currency;
	double m_Rate;
};

struct CArbitrage {
	CArbitrage(int id,
	           const string& rates)
			: m_ProblemID(id), m_Rates(rates), m_Arbitrage(false), m_Rate(0)
	{
	}

	const int m_ProblemID;
	const string m_Rates;

	bool m_Arbitrage;
	vector<string> m_Currency;
	double m_Rate;
};

#endif /* __PROGTEST__ */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// <my code>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CustomException {
};

// CUSTOM 2D ARRAY /////////////////////////////////////////////////////////////////////////////////////////////////////

class CConsultantDataStructure {
public:
	CConsultantDataStructure(const string& rates);
	~CConsultantDataStructure();

	void setData(int x, int y, double val);
	void setData(const string& from, const string& to, double val);
	double getData(int x, int y) const;
	void setPath(int x, int y, double val);
	double getPath(int x, int y) const;
	string getCurrency(int pos) const;

	pair<int, int> findArbitrage(); // return rate or -1
	pair<int, int> findExchange(const string& from, const string& to);
	void reconstructPath(int x, int y, vector<string>& result) const;
private:
	int size;
	double* currencyArray;
	double* pathArray;
	vector<string> currencyPosition;

};

CConsultantDataStructure::CConsultantDataStructure(const string& rates)
{
	// temp variables
	set<string> currencies;
	map<pair<string, string>, double> conversions;
	string exchange, currency1, currency2, rateTemp;
	double rate;

	// remove all the whitespaces
	string fullString = rates;
	fullString.erase(remove(fullString.begin(), fullString.end(), ' '), fullString.end());

	// loop through the string
	istringstream issFull(fullString);
	while(getline(issFull, exchange, ',')) {
		istringstream iss(exchange);
		getline(iss, currency1, '-');
		currencies.insert(currency1);
		iss.get(); // get the '>' char
		getline(iss, currency2, ':');
		currencies.insert(currency2);
		getline(iss, rateTemp);
		rate = stod(rateTemp);
		conversions.insert(make_pair(make_pair(currency1, currency2), rate));
	}


	// set up the data
	size = currencies.size();

	currencyArray = new double[size * size];
	pathArray = new double[size * size];

	for(int i = 0; i < (size * size); i++) {
		currencyArray[i] = -1;
	}

	for(auto it = currencies.begin(); it != currencies.end(); ++it) {
		currencyPosition.push_back(*it);
	}

	for(auto it = conversions.begin(); it != conversions.end(); ++it) {
		setData(it->first.first, it->first.second, it->second);
	}

	for(int x = 0; x < size; x++) {
		for(int y = 0; y < size; y++) {
			if(x == y || getData(x, y) == -1) {
				setPath(x, y, -1);
			} else {
				setPath(x, y, x);
			}
		}
	}
}

CConsultantDataStructure::~CConsultantDataStructure()
{
	delete[] currencyArray;
	delete[] pathArray;
}

void CConsultantDataStructure::setData(int x, int y, double val)
{
	if(x >= size || y >= size) {
		throw new CustomException();
	} else {
		currencyArray[x * size + y] = val;
	}
}

void CConsultantDataStructure::setData(const string& from, const string& to, double val)
{
	int x = -1;
	int y = -1;

	for(unsigned int i = 0; i < currencyPosition.size(); i++) {
		if(from == currencyPosition[i]) {
			x = i;
		}
		if(to == currencyPosition[i]) {
			y = i;
		}
	}

	if(x == -1 || y == -1) {
		throw new CustomException();
	} else {
		setData(x, y, val);
	}
}

double CConsultantDataStructure::getData(int x, int y) const
{
	if(x >= size || y >= size) {
		throw new CustomException();
	} else {
		return currencyArray[x * size + y];
	}
}

void CConsultantDataStructure::setPath(int x, int y, double val)
{
	if(x >= size || y >= size) {
		throw new CustomException();
	} else {
		pathArray[x * size + y] = val;
	}
}

double CConsultantDataStructure::getPath(int x, int y) const
{
	if(x >= size || y >= size) {
		throw new CustomException();
	} else {
		return pathArray[x * size + y];
	}
}

string CConsultantDataStructure::getCurrency(int pos) const
{
	return currencyPosition[pos];
}

pair<int, int> CConsultantDataStructure::findArbitrage()
{
	for(int k = 0; k < size; k++) {
		for(int i = 0; i < size; i++) {
			for(int j = 0; j < size; j++) {
				if(getData(i, k) == -1 || getData(k, j) == -1) {
					continue;
				}

				if((getData(i, j) + 1e-12) < (getData(i, k) * getData(k, j))) {
					setData(i, j, (getData(i, k) * getData(k, j)));
					setPath(i, j, getPath(k, j));

					if(i == j) {
						if(getData(i, j) > 1) {
							// arbitrage found!
							return make_pair(i, j);
						}
					}
				}
			}
		}
	}

	return make_pair(-1, -1);
}

pair<int, int> CConsultantDataStructure::findExchange(const string& from, const string& to)
{
	for(int k = 0; k < size; k++) {
		for(int i = 0; i < size; i++) {
			for(int j = 0; j < size; j++) {
				if(getData(i, k) == -1 || getData(k, j) == -1) {
					continue;
				}

				if((getData(i, j) + 1e-12) < (getData(i, k) * getData(k, j))) {
					setData(i, j, (getData(i, k) * getData(k, j)));
					setPath(i, j, getPath(k, j));
				}
			}
		}
	}

	int x = -1;
	int y = -1;

	for(unsigned int i = 0; i < currencyPosition.size(); i++) {
		if(from == currencyPosition[i]) {
			x = i;
		}
		if(to == currencyPosition[i]) {
			y = i;
		}
	}

	if(x == -1 || y == -1) {
		throw new CustomException();
	}

	return make_pair(x, y);
}

void CConsultantDataStructure::reconstructPath(int x, int y, vector<string>& result) const
{
	if(x == y) {
		result.push_back(getCurrency(x));
	} else if(getPath(x, y) == -1) {
		throw new CustomException();
	} else {
		reconstructPath(x, getPath(x, y), result);
		result.push_back(getCurrency(y));
	}
}

// GLOBAL //////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Data {
	bool isExchange = false;
	CArbitrage* cAr_ptr = NULL;
	CExchange* cEx_ptr = NULL;
};

struct Functions {
	CArbitrage* (* cArb)(void);
	CExchange* (* cEx)(void);
	void (* cCompl)(int);
};

deque<Data> buffer;
bool producentEnded = false;
pthread_mutex_t CConsultantMutex;
pthread_mutex_t ProducentMutex;
sem_t SemFull, SemFree;

void* CExchangeProducer(Functions* atr);
void* CArbitrageProducer(Functions* atr);
void* Consume(Functions* atr);

// Consultant //////////////////////////////////////////////////////////////////////////////////////////////////////////

class CConsultant {
public:
	static void ExchangeSeq(CExchange* req);
	static void ArbitrageSeq(CArbitrage* req);
	CConsultant(CArbitrage* (* arbitrageFn)(void),
	            CExchange* (* exchangeFn)(void),
	            void        (* completed)(int));
	~CConsultant();
	void Execute(int workers);
private:
	Functions* functions;
};

void CConsultant::ExchangeSeq(CExchange* req)
{
	int bankId = -1;
	double bestRate = -1;

	for(auto it = req->m_Banks.begin(); it != req->m_Banks.end(); ++it) {
		try {
			CConsultantDataStructure data((*it));
			pair<int, int> coordinates = data.findExchange(req->m_From, req->m_To);

			double rate = data.getData(coordinates.first, coordinates.second);

			if((bankId == -1 || (bestRate + 1e-12) < rate) &&
			   data.getPath(coordinates.first, coordinates.second) != -1) {
				req->m_Currency.clear();
				data.reconstructPath(coordinates.first, coordinates.second, req->m_Currency);

				bankId = it - req->m_Banks.begin();
				bestRate = rate;

				req->m_Result = true;
				req->m_BankIdx = bankId;

				req->m_Rate = rate;
				req->m_Rate *= 10000;
				req->m_Rate = floor(req->m_Rate);
				req->m_Rate /= 10000;
			}
		} catch(...) {
			continue;
		}
	}
}

void CConsultant::ArbitrageSeq(CArbitrage* req)
{
	try {
		CConsultantDataStructure data(req->m_Rates);
		pair<int, int> coordinates = data.findArbitrage();
		if(coordinates.first == -1 || coordinates.second == -1) {
			return;
		} else {
			data.reconstructPath(coordinates.first, data.getPath(coordinates.first, coordinates.second),
			                     req->m_Currency);
			req->m_Currency.push_back(data.getCurrency(coordinates.first));

			req->m_Arbitrage = true;
			req->m_Rate = data.getData(coordinates.first, coordinates.second);
			req->m_Rate *= 10000;
			req->m_Rate = floor(req->m_Rate);
			req->m_Rate /= 10000;
		}
	} catch(...) {
		return;
	}
}

CConsultant::CConsultant(CArbitrage* (* arbitrageFn)(), CExchange* (* exchangeFn)(), void (* completed)(int))
{
	functions = new Functions;
	functions->cArb = arbitrageFn;
	functions->cEx = exchangeFn;
	functions->cCompl = completed;
}

CConsultant::~CConsultant()
{
	delete functions;
}

void CConsultant::Execute(int workers)
{
	// we have to delete everything from previous runs
	buffer.clear();
	producentEnded = false;
	pthread_mutex_init(&CConsultantMutex, NULL);
	sem_init(&SemFree, 0, (unsigned int) workers);
	sem_init(&SemFull, 0, 0);

	// execute
	pthread_t* thrID;
	pthread_attr_t attr;

	thrID = (pthread_t*) malloc(sizeof(*thrID) * (workers + 2));
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_create(&thrID[0], &attr, (void* (*)(void*)) CExchangeProducer, (void*) functions);
	pthread_create(&thrID[1], &attr, (void* (*)(void*)) CArbitrageProducer, (void*) functions);

	for(int i = 2; i < (workers + 2); i++) {
		pthread_create(&thrID[i], &attr, (void* (*)(void*)) Consume, (void*) functions);
	}

	pthread_attr_destroy(&attr);

	pthread_join(thrID[0], NULL);
	pthread_join(thrID[1], NULL);
	pthread_mutex_lock(&ProducentMutex);
	producentEnded = true;
	for(int i = 0; i < workers; i++) {
		sem_post(&SemFull);
	}
	pthread_mutex_unlock(&ProducentMutex);
	for(int i = 2; i < (2 + workers); i++) {
		pthread_join(thrID[i], NULL);
	}

	pthread_mutex_destroy(&CConsultantMutex);
	sem_destroy(&SemFree);
	sem_destroy(&SemFull);

	free(thrID);
}

// GLOBAL //////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* CExchangeProducer(Functions* atr)
{
	while(true) {
		CExchange* ptr = atr->cEx();
		if(ptr == NULL) {
			break;
		} else {
			Data data;
			data.isExchange = true;
			data.cEx_ptr = ptr;

			sem_wait(&SemFree);
			pthread_mutex_lock(&CConsultantMutex);
			buffer.push_back(data);
			pthread_mutex_unlock(&CConsultantMutex);
			sem_post(&SemFull);
		}
	}

	return NULL;
}

void* CArbitrageProducer(Functions* atr)
{
	while(true) {
		CArbitrage* ptr = atr->cArb();
		if(ptr == NULL) {
			break;
		} else {
			Data data;
			data.isExchange = false;
			data.cAr_ptr = ptr;

			sem_wait(&SemFree);
			pthread_mutex_lock(&CConsultantMutex);
			buffer.push_back(data);
			pthread_mutex_unlock(&CConsultantMutex);
			sem_post(&SemFull);
		}
	}

	return NULL;
}

void* Consume(Functions* atr)
{
	while(true) {
		Data data;

		sem_wait(&SemFull);
		pthread_mutex_lock(&CConsultantMutex);
		if(buffer.empty()) {
			pthread_mutex_lock(&ProducentMutex);
			if(producentEnded) {
				pthread_mutex_unlock(&ProducentMutex);
				pthread_mutex_unlock(&CConsultantMutex);

				return NULL;
			} else {
				pthread_mutex_unlock(&ProducentMutex);
				pthread_mutex_unlock(&CConsultantMutex);

				continue;
			}
		} else {
			data = buffer.back();
			buffer.pop_back();
		}
		pthread_mutex_unlock(&CConsultantMutex);
		sem_post(&SemFree);

		if(data.isExchange) {
			CConsultant::ExchangeSeq(data.cEx_ptr);
			atr->cCompl(data.cEx_ptr->m_ProblemID);
		} else {
			CConsultant::ArbitrageSeq(data.cAr_ptr);
			atr->cCompl(data.cAr_ptr->m_ProblemID);
		}
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// </my code>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PROGTEST__

int globalCounter = 0;

CArbitrage* arbitrageFunc(void)
{
	if(globalCounter > 100000) { return NULL; }
	//cout << "Arbitrage \"" << globalCounter << "\" in proccess" << endl;
	CArbitrage* arbitrage = new CArbitrage(globalCounter++,
	                                       "  CZK -> USD: 0.0398 , USD -> EUR : 0.9259 , EUR -> GBP :  0.7728 , GBP -> CZK : 35.0582 ");

	return arbitrage;

}

CExchange* exchangeFunc(void)
{
	if(globalCounter > 100000) { return NULL; }
	//cout << "Exchange \"" << globalCounter << "\" in proccess" << endl;
	CExchange* exchange = new CExchange(globalCounter++, "CZK", "GBP");
	exchange->AddBank("CZK -> USD: 0.0398, USD -> EUR: 0.9259, EUR -> GBP: 0.7728, CZK -> GBP: 0.0280");
	return exchange;

}

void completeFunc(int i)
{
//	cout << "Problem with id: " << i << " was completed" << endl;
	return;
}

int main(void)
{
	CConsultant* consultant = new CConsultant(&arbitrageFunc, &exchangeFunc, &completeFunc);
	consultant->Execute(1000);

	CArbitrage a0(0, "  CZK -> USD: 0.0398 , USD -> EUR : 0.9259 , EUR -> GBP :  0.7728 , GBP -> CZK : 35.0582 ");
	CConsultant::ArbitrageSeq(&a0);
	assert (a0.m_Arbitrage == false);

	CArbitrage a1(1, "CZK->USD:0.0398,USD->EUR:0.9259,EUR->GBP:0.7728,GBP->CZK:35.1263");
	CConsultant::ArbitrageSeq(&a1);
	assert (a1.m_Arbitrage == true);
	cout << a1.m_Rate << endl;
	// e.g.: m_Rate = 1.0003, m_Currency = { GBP, CZK, USD, EUR, GBP }

	CArbitrage a2(2,
	              "USD -> CZK: 25.0000, EUR -> CZK: 27.0000, CZK -> USD: 0.0400, CZK -> EUR: 0.0370, EUR -> RUB: 83.0000, RUB -> USD: 0.0130, CZK -> RUB: 2.9990");
	CConsultant::ArbitrageSeq(&a2);
	assert (a2.m_Arbitrage == false);

	CArbitrage a3(3,
	              "USD -> CZK: 25.0000, EUR -> CZK: 27.0000, CZK -> USD: 0.0400, CZK -> EUR: 0.0370, EUR -> RUB: 83.0000, RUB -> USD: 0.0130, CZK -> RUB: 2.9990, CZK -> PLN: 0.1736, PLN -> HUF: 71.4216, HUF -> RUB: 0.2565");
	CConsultant::ArbitrageSeq(&a3);
	assert (a3.m_Arbitrage == true);
	cout << a3.m_Rate << endl;
	// e.g.: m_Rate = 1.0335, m_Currency = { HUF, RUB, USD, CZK, PLN, HUF }

	CArbitrage a4(4,
	              "CZK -> USD: 0.0398, USD -> EUR: 0.9259, EUR -> CZK: 27.0472, PLN -> HUF: 71.4216, HUF -> RUB: 0.2565, RUB -> PLN: 0.0545");
	CConsultant::ArbitrageSeq(&a4);
	assert (a4.m_Arbitrage == false);

	CArbitrage a5(5,
	              "CZK -> USD: 0.0398, USD -> EUR: 0.9259, EUR -> CZK: 27.0472, PLN -> HUF: 71.4216, HUF -> RUB: 0.2565, RUB -> PLN: 0.0565");
	CConsultant::ArbitrageSeq(&a5);
	assert (a5.m_Arbitrage == true);
	cout << a5.m_Rate << endl;
	// e.g.: m_Rate = 1.0350, m_Currency = { RUB, PLN, HUF, RUB }

	CExchange e0(0, "CZK", "GBP");
	e0.AddBank("CZK -> USD: 0.0398, USD -> EUR: 0.9259, EUR -> GBP: 0.7728, CZK -> GBP: 0.0280");
	CConsultant::ExchangeSeq(&e0);
	assert (e0.m_Result == true);
	cout << e0.m_Rate << "(bank: " << e0.m_BankIdx << ")" << endl;
	// e.g.: m_BankIdx = 0, m_Rate = 0.0284, m_Currency = { CZK, USD, EUR, GBP }

	CExchange e1(1, "CZK", "CAD");
	e1.AddBank("CZK -> USD: 0.0398, USD -> EUR: 0.9259, EUR -> GBP: 0.7728, CAD -> GBP: 0.7494");
	CConsultant::ExchangeSeq(&e1);
	assert (e1.m_Result == false);

	CExchange e2(2, "CZK", "CAD");
	e2.AddBank("CZK -> USD: 0.0398, USD -> EUR: 0.9259, EUR -> GBP: 0.7728, USD -> CZK: 0.0400");
	CConsultant::ExchangeSeq(&e2);
	assert (e2.m_Result == false);

	CExchange e3(3, "CZK", "GBP");
	e3.AddBank("CZK -> USD: 0.0398, USD -> EUR: 0.9259, EUR -> GBP: 0.7728, CZK -> GBP: 0.0280");
	e3.AddBank("CZK -> USD: 0.0500, USD -> EUR: 0.8352, EUR -> GBP: 0.7728, EUR -> HUF: 308.6779, HUF -> GBP: 0.0027");
	CConsultant::ExchangeSeq(&e3);
	assert (e3.m_Result == true);
	cout << e3.m_Rate << "(bank: " << e3.m_BankIdx << ")" << endl;
	// e.g.: m_BankIdx = 1, m_Rate = 0.0348, m_Currency = { CZK, USD, EUR, HUF, GBP }

	return 0;
}

#endif /* __PROGTEST__ */

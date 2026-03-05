//============================================================================
// Name        	: MatrixGen.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Aug 30, 2023 7:46:11 PM
//============================================================================

#include <AlgoEngines/MatrixGen.hpp>

MatrixGen::MatrixGen() : _INFINITE_COL(24)
{
	// TODO Auto-generated constructor stub

}

MatrixGen::~MatrixGen() {
	// TODO Auto-generated destructor stub
}

void MatrixGen::LogMe(std::string szMsg, LogLevel::Enum level) {
	KT01_LOG_INFO(__CLASS__, szMsg)
;
}

void MatrixGen::GenOutrightMatrix(string Product, string StartMonth, int StartYear, int NumMonths)
{
	LogMe("Generating Outright Matrices. Ya.", LogLevel::INFO);

   std::string FUTS = "FGHJKMNQUVXZ";
   int iLenFuts = FUTS.length();

   int iStart = FUTS.find(StartMonth.c_str());

   int j = _INFINITE_COL;
   int iYr1 = StartYear;

   int iMth1 = iStart;

   int iStep = 1;

   //bool bIsTAS = false;
   //bool bIsSpread = false;

	for (int n = 0; n < NumMonths; n+=iStep)
	{
		std::string szSecDesc = Product;
		std::string szM1;
		std::string szFind = FUTS.substr(iMth1,1);

		ostringstream oss;
		oss << szFind << iYr1;
		szM1 = oss.str();

		std::string szBase = Product;

		char cDesc[100];
		sprintf(cDesc,"%s%s",szBase.c_str(),szM1.c_str());
		szSecDesc = cDesc;

		RC rc;
		//move the outright to have n = j
		j = n;
		rc.iRow = n;
		rc.iCol = j;

		//map it up
		m_GraphMap[std::make_pair(n,j)] = szSecDesc;
		m_RcMap[szSecDesc] = rc;

		cout << "GRAPH: " << n << "," << j << "=" << m_GraphMap[std::make_pair(n,j)] << endl;

		iMth1 +=iStep;
		if (iMth1 > (iLenFuts-1))
		{
		   iMth1 = iStep-1;
		   iYr1 ++;
		}
		if (iYr1 % 10 == 0)
			iYr1 = 0;
	}//n


 //  PrintGraph();


}

void MatrixGen::GenSpreadMatrix(string Product, string StartMonth, int StartYear, int NumMonths)
{
	LogMe("Generating Spread Matrices. Wowoweewah", LogLevel::INFO);

	std::string FUTS = "FGHJKMNQUVXZ";
	int iLenFuts = FUTS.length();

	int iStart = FUTS.find(StartMonth.c_str());

	int iYr1 = StartYear;
	int iYr2 = StartYear;
	int iMth1 = iStart;
	int iMth2 = iStart;

	int iStep = 1;


	// bool bIsTAS = false;
	// bool bIsSpread = true;

	for (int n = 0; n < NumMonths; n+=iStep)
	{
		std::string szSecDesc = Product;

		std::string szM1;
		std::string szFind = FUTS.substr(iMth1,1);

		ostringstream oss;
		oss << szFind << iYr1;
		szM1 = oss.str();


		iMth2 = iMth1 + iStep;
		iYr2 = iYr1;
		for (int j = n+iStep; j < NumMonths; j+=iStep)
		{
			if (iMth2 > iLenFuts-1)
			{
				iMth2 = iStep-1;
				iYr2 ++;
			}
			if (iYr2 % 10 == 0)
				iYr2 = 0;

		   std::string szM2;
		   std::string szFind2 = FUTS.substr(iMth2,1);


			ostringstream oss2;
			oss2 << szFind2 << iYr2;
			szM2 = oss2.str();


			ostringstream oss3;
			oss3 << Product  << szM1 << "-" << Product <<  szM2;

			std::string szSecDesc = oss3.str();

			RC rc;
			rc.iRow = n;
			rc.iCol = j;

			//map it up
			m_GraphMap[std::make_pair(n,j)] = szSecDesc;
			m_RcMap[szSecDesc] = rc;

			cout << "GRAPH: " << n << "," << j << "=" << m_GraphMap[std::make_pair(n,j)] << endl;

			iMth2+=iStep;
		} //j

		iMth1 +=iStep;
		if (iMth1 > iLenFuts-1)
		{
		   iMth1 = iStep-1;
		   iYr1 ++;
		}
		if (iYr1 == 10)
			iYr1 = 0;
	}//n


}

const map<pair<int, int>, std::string>& MatrixGen::GetGraphMap() const {
	return m_GraphMap;
}

const std::unordered_map<std::string, RC>& MatrixGen::GetRcMap() const {
	return m_RcMap;
}

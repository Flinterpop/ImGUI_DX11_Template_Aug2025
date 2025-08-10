#pragma once

#include <string>
#include <vector>

class LoadFile
{

public:
	std::vector<int> SDWs;

	char fname[260];
	int m_NumRows = 0;


		
	bool FineSync = false;
	bool CoarseSync = false;
	bool NTR = false;
	bool ETR = true;
	bool PR = true;
	int myJU = 172;



};
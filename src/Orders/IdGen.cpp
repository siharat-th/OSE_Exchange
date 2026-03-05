
#include "IdGen.h"

using namespace KTN;

IdGen::IdGen(){}
IdGen::~IdGen(){}


void IdGen::Seed()
{
	//char buff[100];
	srand (time(NULL));
	int v2 = rand() % 6000 + 1000;

	m_iSeq = v2;

	//sprintf(buff,"***ORDER ID SEED: %d", m_iSeq);
	//LWT_LOG_ME(buff,"IDGEN");
}
int IdGen::Next()
{
	inc();
	return m_iSeq;
}

int IdGen::Current()
{
	return m_iSeq;
}

void IdGen::inc()
{
	m_iSeq++;
}

#pragma once
#include "BankClient.h"
#include "stdafx.h"


enum PrimitiveSynchronize
{
	CriticalSection = 1
	, Mutex
	, Semaphore
	, Event
};

class CBankClient;

class CBank
{
public:
	CBank(PrimitiveSynchronize type);
	~CBank();
	CBankClient* CreateClient();
	void UpdateClientBalance(CBankClient& client, int value);
	void WaitThreads();
	void CreateThreads();
private:
	std::vector<CBankClient> m_clients;
	int m_totalBalance;
	std::vector<HANDLE> m_threads;
	PrimitiveSynchronize m_primitiveSynchronize;
	CRITICAL_SECTION m_criticalSection;
	HANDLE m_hMutex;
	HANDLE m_hSemaphore;
	HANDLE m_hEvent;
	int GetTotalBalance();
	void SetTotalBalance(int value);
	void SomeLongOperations();
	void selectPrimitiveSynchronize();
};
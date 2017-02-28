#include "Bank.h"
#include "stdafx.h"


CBank::CBank(PrimitiveSynchronize type)
	: m_primitiveSynchronize(type)
	, m_totalBalance(0)
	, m_clients(std::vector<CBankClient>())
{
	selectPrimitiveSynchronize();
}

CBank::~CBank()
{
	switch (m_primitiveSynchronize)
	{
	case PrimitiveSynchronize::CriticalSection:
		DeleteCriticalSection(&m_criticalSection);
		break;
	case PrimitiveSynchronize::Mutex:
		CloseHandle(&m_hMutex);
		break;
	case PrimitiveSynchronize::Semaphore:
		CloseHandle(m_hSemaphore);
		break;
	case PrimitiveSynchronize::Event:
		CloseHandle(m_hEvent);
		break;
	}
}

void CBank::selectPrimitiveSynchronize()
{
	switch (m_primitiveSynchronize)
	{
	case PrimitiveSynchronize::CriticalSection:
		InitializeCriticalSection(&m_criticalSection);
		break;
	case PrimitiveSynchronize::Mutex:
		m_hMutex = CreateMutex(NULL, false, NULL);
		break;
	case PrimitiveSynchronize::Semaphore:
		m_hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
		break;
	case PrimitiveSynchronize::Event:
		m_hEvent = CreateEvent(NULL, true, false, NULL);
		break;
	}
}

CBankClient* CBank::CreateClient()
{
	unsigned clientId = unsigned(m_clients.size());
	CBankClient* client = new CBankClient(this, clientId);
	m_clients.push_back(*client);
	return client;
}


void CBank::UpdateClientBalance(CBankClient &client, int value)
{
	int totalBalance = GetTotalBalance();
	std::cout << "Client " << client.GetId() << " initiates reading total balance. Total = " << totalBalance << "." << std::endl;
	
	SomeLongOperations();
	totalBalance += value;

	std::cout
		<< "Client " << client.GetId() << " updates his balance with " << value
		<< " and initiates setting total balance to " << totalBalance
		<< ". Must be: " << GetTotalBalance() + value << "." << std::endl;

	if (totalBalance < 0) 
	{
		std::cout << "! ERROR !" << std::endl;
	}

	SetTotalBalance(totalBalance);
}

void CBank::CreateThreads()
{
	for (size_t i = 0; i < m_clients.size(); ++i)
	{
		auto & client = m_clients[i];
		m_threads.push_back(CreateThread(NULL, 0, &client.ThreadFunction, &client, CREATE_SUSPENDED, NULL));
	}
}

void CBank::WaitThreads()
{
	for (auto & thread : m_threads)
	{
		ResumeThread(thread);
	}
	WaitForMultipleObjects(m_threads.size(), m_threads.data(), TRUE, INFINITE);
}

int CBank::GetTotalBalance()
{
	return m_totalBalance;
}


void CBank::SetTotalBalance(int value)
{
	switch (m_primitiveSynchronize)
	{
	case PrimitiveSynchronize::CriticalSection:
		EnterCriticalSection(&m_criticalSection);
		m_totalBalance = value;
		LeaveCriticalSection(&m_criticalSection);
		break;
	case PrimitiveSynchronize::Mutex:
		WaitForSingleObject(m_hMutex, INFINITE);
		m_totalBalance = value;
		ReleaseMutex(m_hMutex);
		break;
	case PrimitiveSynchronize::Semaphore:
		WaitForSingleObject(m_hSemaphore, INFINITE);
		m_totalBalance = value;
		ReleaseSemaphore(m_hSemaphore, 1, NULL);
	case PrimitiveSynchronize::Event:
		SetEvent(m_hEvent);
		m_totalBalance = value;
		ResetEvent(m_hEvent);
	default:
		m_totalBalance = value;
		break;
	}
}

void CBank::SomeLongOperations()
{
	Sleep(200);
}
#include "stdafx.h"
#include "Bank.h"
#include "BankClient.h"

bool correctPrimitiveSynchronize(char *argv[], PrimitiveSynchronize &type)
{
	int value = atoi(argv[2]);
	switch (value)
	{
	case 1:
		type = PrimitiveSynchronize::CriticalSection;
		break;
	case 2:
		type = PrimitiveSynchronize::Mutex;
		break;
	case 3:
		type = PrimitiveSynchronize::Semaphore;
		break;
	case 4:
		type = PrimitiveSynchronize::Event;
		break;
	default:
		return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	if (argc != 3 || argv[1] == "/?")
	{
		std::cout << "Enter arguments <number of clients> <type synchronization primitive>" << std::endl;
		std::cout << "Type:" << std::endl;
		std::cout << "1 - critical section" << std::endl;
		std::cout << "2 - mutex" << std::endl;
		std::cout << "3 - semaphore" << std::endl;
		std::cout << "4 - event" << std::endl;
		return 1;
	}
	PrimitiveSynchronize type;
	if (!correctPrimitiveSynchronize(argv, type))
	{
		std::cout << "Incorrect synchronization primitive";
		return 1;
	}
	size_t numberOfClients = size_t(atoi(argv[1]));
	CBank bank(type);
	for (size_t i = 0; i < numberOfClients; ++i)
	{
		bank.CreateClient();
	}
	bank.CreateThreads();
	bank.WaitThreads();
    return 0;
}
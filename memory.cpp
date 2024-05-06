#include "memory.h"

memory::memory(HANDLE process)
{
	hProcess = process;
}

memory::~memory()
{
	size_t allocCount = allocations.size();

	for (size_t i = 0; i < allocCount; i++)
	{
		VirtualFreeEx(hProcess, (LPVOID)allocations[0], 0, MEM_RELEASE);
	}
}

uintptr_t memory::Allocate(size_t size)
{
	uintptr_t lpBaseAddress;

	lpBaseAddress = reinterpret_cast<uintptr_t>(VirtualAllocEx(hProcess, NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE));

	if (lpBaseAddress == NULL)
	{
		throw "An error occured while allocating memory!";
	}

	//Add an allocation address to the list of all allocations
	allocations.push_back(lpBaseAddress);

	return lpBaseAddress;
}


uintptr_t memory::AllocateAndWrite(std::vector<byte> data)
{
	size_t dataSize = data.size();
	byte* buffer = data.data();

	uintptr_t lpBaseAddress;
	lpBaseAddress = Allocate(dataSize);

	bool status;
	status = WriteProcessMemory(hProcess, (LPVOID)lpBaseAddress, buffer, dataSize, NULL);

	if (!status)
	{
		throw "An error occured while writing byte buffer!";
	}

	return lpBaseAddress;
}

uintptr_t memory::AllocateAndWrite(std::string string)
{
	size_t size = string.size() + 1;

	std::vector<byte> data;
	data.resize(size);

	memcpy(data.data(), string.data(), size);

	uintptr_t address = AllocateAndWrite(data);

	return address;
}

std::string memory::ReadMemString(uintptr_t address)
{
	char buffer;
	int size = 0;

	//recognize size of the string
	do
	{
		buffer = ReadMem<byte>(address + size);
		size++;
	} while (buffer != 0x00);

	char* text = new char[size] {0};

	bool status;

	//read value
	status = ReadProcessMemory(hProcess, (LPVOID)address, text, size, NULL);
	if (!status)
	{
		throw "An error occured while reading string!";
	}

	return std::string(text);
}

void memory::WriteMemString(uintptr_t address, std::string buffer)
{
	size_t size = buffer.size() + 1;

	std::vector<byte> data;
	data.resize(size);

	bool status;
	status = WriteProcessMemory(hProcess, (LPVOID)address, buffer.data(), size, NULL);

	if (!status)
	{
		throw "An error occured while writing string!";
	}
}
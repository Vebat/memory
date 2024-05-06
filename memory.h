// memory.h 06.05.24
// contributor: Vebat
// version: 1

// This class provide a lay of abstraction for easy work with external process memory
// Provide a public interface for both a read and write memory in external memory
//
// Remark: all the functions in the class throws an exceptions to ensure the
// program that use this class don't get wrong results and as result have undefined behaviour

#pragma once

#ifndef MEMORY_CLASS
#define MEMORY_CLASS

#include <Windows.h>
#include <vector>
#include <string>

class memory
{
private:
	HANDLE hProcess;
	std::vector<uintptr_t> allocations;

public:
	memory(HANDLE process);
	~memory();

	/// <summary>
	/// Template specified abstraction for WriteProcessMemory
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="address">- an address in the external process where a data is written</param>
	/// <param name="buffer">- pointer to a buffer variable of specified type in the current process</param>
	template<typename T>
	void WriteMem(uintptr_t address, T* buffer)
	{
		bool status;

		status = WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(T), NULL);
		if (!status)
		{
			throw "An error occured while writing to memory!";
		}
	}

	/// <summary>
	///  Template specified abstraction for ReadProcessMemory
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="address">- an address in the external process wherefrom a data is readed</param>
	/// <returns>Returns a variable of specified type with readed value</returns>
	template<typename T>
	T ReadMem(uintptr_t address)
	{
		bool status;
		T buffer;

		status = ReadProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(buffer), NULL);
		if (!status)
		{
			throw "An error occured while reading memory!";
		}

		return buffer;
	}

	/// <summary>
	/// Allocate a memory on arbitrary address in the external process for the value writes value in
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="value">- a variable to write in the external process</param>
	/// <returns>Returns an address of a variable in the external process</returns>
	template<typename T>
	uintptr_t AllocateAndWrite(T value)
	{
		uintptr_t lpBaseAddress;
		lpBaseAddress = Allocate(sizeof(value));

		WriteMem<T>(lpBaseAddress, &value);

		return lpBaseAddress;
	}

	
	uintptr_t Allocate(size_t size);

	uintptr_t AllocateAndWrite(std::vector<byte> data);
	uintptr_t AllocateAndWrite(std::string string);

	void WriteMemString(uintptr_t address, std::string buffer);
	std::string ReadMemString(uintptr_t address);
};


#endif // MEMORY_CLASS

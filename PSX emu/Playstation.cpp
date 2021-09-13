#include "Playstation.h"
#include <fstream>
#include <iterator>
#include <vector>
#include <cstdint>

Playstation::Playstation() 
	: _cpu(this)
	, _bios(512 * 1024)
{
	std::basic_ifstream<std::uint8_t> input("SCPH1001.bin", std::ios::binary);

	std::vector<std::uint8_t> buffer(
		(std::istreambuf_iterator<std::uint8_t>(input)),
		(std::istreambuf_iterator<std::uint8_t>()));

	input.close();

	_bios.Write(0, buffer);
}

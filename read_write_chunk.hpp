#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>

//helper function that reads an array of structures preceded by a simple header:
//Expected format:
// |ma|gi|c.|..| <-- four byte "magic number"
// |sz|sz|sz|sz| <-- four byte (native endian) size
// |TT...TT| * (sz/sizeof(TT)) <-- enough T structures to make up sz bytes

template< typename T >
void read_chunk(std::istream &from, std::string const &magic, std::vector< T > *to_) {
	assert(to_);
	auto &to = *to_;
	char c;
	struct ChunkHeader {
		char magic[4] = {'\0', '\0', '\0', '\0'};
		uint32_t size = 0;
	};
	static_assert(sizeof(ChunkHeader) == 8, "header is packed");

	ChunkHeader header;
	if (!from.read(reinterpret_cast< char * >(header.magic), sizeof(header.magic))) {
		throw std::runtime_error("Failed to read chunk header");
	}

	from.get(c);
	header.size = 0;

	while (from.get(c)){
		if(c == ' ' || c =='\n'){
			break;
		}
		header.size = header.size * 10 + (c-'0');
	}

	// if (!from.read(reinterpret_cast< char * >(&header.size), sizeof(header.size))) {
	// 	throw std::runtime_error("Failed to read chunk header");
	// }
	to.resize(header.size, 0);
	size_t i = 0;
	while (from.get(c)){
		if(c == ' ' or c == '\n'){
			i += 1;
			continue;
		}

		if (i >= header.size)
		{
			break;
		}

		to[i] = to[i]*10+ (c-'0');
	}

	// for (i = 0; i < to.size(); i++)
	// {
	// 	std::cout << to[i] <<"\n";
	// }
	

	// if (std::string(header.magic,4) != magic) {
	// 	throw std::runtime_error("Unexpected magic number in chunk");
	// }

	// if (header.size % sizeof(T) != 0) {
	// 	throw std::runtime_error("Size of chunk not divisible by element size");
	// }

	// std::cout << header.size;
	// to.resize(header.size / sizeof(T));
	// for (size_t i = 0; i < header.size; i++)
	// {
	// 	if (!from.read(reinterpret_cast< char * >(&to[i]), sizeof(T))) {
	// 		throw std::runtime_error("Failed to read chunk data.");
	// 	}
	// 	std::cout<< &to[0] << "\n";
	// 	from.get(c);
	// }
	
	
}


//helper function to write a chunk of data in the same format as read_chunk:
template< typename T >
void write_chunk(std::string const &magic, std::vector< T > const &from, std::ostream *to_) {
	assert(magic.size() == 4);
	assert(to_);
	auto &to = *to_;

	struct ChunkHeader {
		char magic[4] = {'\0', '\0', '\0', '\0'};
		uint32_t size = 0;
	};
	static_assert(sizeof(ChunkHeader) == 8, "header is packed");
	ChunkHeader header;
	header.magic[0] = magic[0];
	header.magic[1] = magic[1];
	header.magic[2] = magic[2];
	header.magic[3] = magic[3];
	header.size = uint32_t(from.size() * sizeof(T));

	to.write(reinterpret_cast< const char * >(&header), sizeof(header));
	to.write(reinterpret_cast< const char * >(from.data()), from.size() * sizeof(T));
}

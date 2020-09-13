#include "PPU466.hpp"
#include "data_path.hpp"
#include "read_write_chunk.hpp"

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

void load_sprites(PPU466* ppu);
void load_background(PPU466* ppu);

void load_sprites(PPU466 *ppu){
	// load sprite palette
	std::ifstream spr0_palette(data_path("resources/sprite0_palette"));
	std::istream& spr0_p = spr0_palette;
	std::string sp0_p_magic="spr0";
	std::vector<int8_t> spr0_colors;
	read_chunk(spr0_p, sp0_p_magic, &spr0_colors);

	for (size_t i = 0; i < 4; i++)
	{
		ppu->palette_table[2][i]=(glm::u8vec4(spr0_colors[i*4],spr0_colors[i*4+1],spr0_colors[i*4+2],spr0_colors[i*4+3]));
	}

	std::ifstream spr1_palette(data_path("resources/sprite1_palette"));
	std::istream& spr1_p = spr1_palette;
	std::string sp1_p_magic="spr1";
	std::vector<int8_t> spr1_colors;
	read_chunk(spr1_p, sp1_p_magic, &spr1_colors);

	for (size_t i = 0; i < 4; i++)
	{
		ppu->palette_table[3][i]=(glm::u8vec4(spr1_colors[i*4],spr1_colors[i*4+1],spr1_colors[i*4+2],spr1_colors[i*4+3]));
	}

	// load sprite palette
	std::ifstream sprite_tile(data_path("resources/sprite0"));
	std::ifstream& sp = sprite_tile;
	std::string str_spr = "spr0";
	std::vector<int8_t> spr_tiles;
	read_chunk(sp, str_spr, &spr_tiles);

	// std::cout << spr_tiles.size();
	size_t spr_tiles_ind = 28;
	for (size_t i = 0; i < 4; i+=1)
	{
		for (size_t j = 0; j < 8; j++)
		{
			ppu->tile_table[spr_tiles_ind+i].bit0[8-j-1] = spr_tiles[i*8+j];
			ppu->tile_table[spr_tiles_ind+i].bit1[8-j-1] = spr_tiles[4*8+i*8+j];
			
		}
		
	}
	
	
}
void load_background(PPU466 *ppu){
	// load grass palette
	std::ifstream from(data_path("resources/grass_mid"));
	std::istream& is = from;
	std::string magic="gmid";
	std::vector<int8_t> colors;
	read_chunk(is, magic, &colors);

	assert(colors.size() == 16);
	for (size_t i = 0; i < 4; i++)
	{
		ppu->palette_table[5][i]=(glm::u8vec4(colors[i*4],colors[i*4+1],colors[i*4+2],colors[i*4+3]));
	}

	// load sand palette
	std::ifstream sand_palette(data_path("resources/sand_palette"));
	std::istream& is_sand = sand_palette;
	std::string sand_magic="sand";
	std::vector<int8_t> sand_colors;
	read_chunk(is_sand, sand_magic, &sand_colors);

	assert(sand_colors.size() == 16);
	for (size_t i = 0; i < 4; i++)
	{
		ppu->palette_table[4][i]=(glm::u8vec4(sand_colors[i*4],sand_colors[i*4+1],sand_colors[i*4+2],sand_colors[i*4+3]));
	}

	// load background tiles
	std::ifstream grass(data_path("resources/grass_tile"));
	std::istream& gs = grass;
	std::string gma = "gras";
	std::vector<int8_t> grass_tiles;
	read_chunk(gs, gma, &grass_tiles);

	// std::cout << grass_tiles.size();

	size_t bg_tile_start = 33;
	for (size_t j = 0; j < grass_tiles.size(); j++)
	{
		if(j%2 == 0){
			ppu->tile_table[bg_tile_start + j/16].bit0[(j/2)%8] = grass_tiles[j];
		}else{
			ppu->tile_table[bg_tile_start + j/16].bit1[(j/2)%8] = grass_tiles[j];
		}
	}


}

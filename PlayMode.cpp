#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"
#include "read_write_chunk.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

#include <random>

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	//Also, *don't* use these tiles in your game:

	{ //use tiles 0-16 as some weird dot pattern thing:
		std::array< uint8_t, 8*8 > distance;
		for (uint32_t y = 0; y < 8; ++y) {
			for (uint32_t x = 0; x < 8; ++x) {
				float d = glm::length(glm::vec2((x + 0.5f) - 4.0f, (y + 0.5f) - 4.0f));
				d /= glm::length(glm::vec2(4.0f, 4.0f));
				distance[x+8*y] = std::max(0,std::min(255,int32_t( 255.0f * d )));
			}
		}
		for (uint32_t index = 0; index < 16; ++index) {
			PPU466::Tile tile;
			uint8_t t = (255 * index) / 16;
			for (uint32_t y = 0; y < 8; ++y) {
				uint8_t bit0 = 0;
				uint8_t bit1 = 0;
				for (uint32_t x = 0; x < 8; ++x) {
					uint8_t d = distance[x+8*y];
					if (d > t) {
						bit0 |= (1 << x);
					} else {
						bit1 |= (1 << x);
					}
				}
				tile.bit0[y] = bit0;
				tile.bit1[y] = bit1;
			}
			ppu.tile_table[index] = tile;
		}
	}

	//use sprite 32 as a "player":
	ppu.tile_table[32].bit0 = {
		0b01111110,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b01111110,
	};
	ppu.tile_table[32].bit1 = {
		0b00000000,
		0b00000000,
		0b00011000,
		0b00100100,
		0b00000000,
		0b00100100,
		0b00000000,
		0b00000000,
	};

	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//makes the center of tiles 0-16 solid:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0xff, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for the misc other sprites:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x88, 0x88, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};

	std::ifstream from("resources/grass_mid");
	std::istream& is = from;
	std::string magic="gmid";
	std::vector<int8_t> colors;
	read_chunk(is, magic, &colors);

	assert(colors.size() == 16);
	for (size_t i = 0; i < 4; i++)
	{
		ppu.palette_table[5][i]=(glm::u8vec4(colors[i*4],colors[i*4+1],colors[i*4+2],colors[i*4+3]));

		printf("%x, %x, %x, %x\n",ppu.palette_table[5][i][0], ppu.palette_table[5][i][1], ppu.palette_table[5][i][2], ppu.palette_table[5][i][3]);
	}

	std::ifstream sand_palette("resources/sand_palette");
	std::istream& is_sand = sand_palette;
	std::string sand_magic="sand";
	std::vector<int8_t> sand_colors;
	read_chunk(is_sand, sand_magic, &sand_colors);

	assert(sand_colors.size() == 16);
	for (size_t i = 0; i < 4; i++)
	{
		ppu.palette_table[4][i]=(glm::u8vec4(sand_colors[i*4],sand_colors[i*4+1],sand_colors[i*4+2],sand_colors[i*4+3]));
	}

	std::ifstream spr0_palette("resources/sprite0_palette");
	std::istream& spr0_p = spr0_palette;
	std::string sp0_p_magic="spr0";
	std::vector<int8_t> spr0_colors;
	read_chunk(spr0_p, sp0_p_magic, &spr0_colors);

	assert(colors.size() == 16);
	for (size_t i = 0; i < 4; i++)
	{
		ppu.palette_table[2][i]=(glm::u8vec4(spr0_colors[i*4],spr0_colors[i*4+1],spr0_colors[i*4+2],spr0_colors[i*4+3]));
		printf("%x, %x, %x, %x\n",ppu.palette_table[2][i][0], ppu.palette_table[2][i][1], ppu.palette_table[2][i][2], ppu.palette_table[2][i][3]);
	
	}

	std::ifstream spr1_palette("resources/sprite1_palette");
	std::istream& spr1_p = spr1_palette;
	std::string sp1_p_magic="spr1";
	std::vector<int8_t> spr1_colors;
	read_chunk(spr1_p, sp1_p_magic, &spr1_colors);

	assert(colors.size() == 16);
	for (size_t i = 0; i < 4; i++)
	{
		ppu.palette_table[3][i]=(glm::u8vec4(spr1_colors[i*4],spr1_colors[i*4+1],spr1_colors[i*4+2],spr1_colors[i*4+3]));
	}
	

	std::ifstream grass("resources/grass_tile");
	std::istream& gs = grass;
	std::string gma = "gras";
	std::vector<int8_t> grass_tiles;
	read_chunk(gs, gma, &grass_tiles);

	std::cout << grass_tiles.size();

	size_t bg_tile_start = 33;
	for (size_t j = 0; j < grass_tiles.size(); j++)
	{
		if(j%2 == 0){
			ppu.tile_table[bg_tile_start + j/16].bit0[(j/2)%8] = grass_tiles[j];
		}else{
			ppu.tile_table[bg_tile_start + j/16].bit1[(j/2)%8] = grass_tiles[j];
			
		}
	}

	// std::ifstream sand_resource("resources/sand_tile");
	// std::istream& sand_tiles_ptr = sand_resource;
	// std::vector<int8_t> sand_tiles;
	// read_chunk(sand_tiles_ptr, sand_magic, &sand_tiles);

	// std::cout << sand_tiles.size();

	// bg_tile_start = 97;
	// for (size_t j = 0; j < grass_tiles.size(); j++)
	// {
	// 	if(j%2 == 0){
	// 		ppu.tile_table[bg_tile_start + j/16].bit0[(j/2)%8] = grass_tiles[j];
	// 	}else{
	// 		ppu.tile_table[bg_tile_start + j/16].bit1[(j/2)%8] = grass_tiles[j];
	// 	}
	// }

	
	std::ifstream sprite_tile("resources/sprite0");
	std::ifstream& sp = sprite_tile;
	std::string str_spr = "spr0";
	std::vector<int8_t> spr_tiles;
	read_chunk(sp, str_spr, &spr_tiles);

	std::cout << spr_tiles.size();
	size_t spr_tiles_ind = 28;
	for (size_t i = 0; i < 4; i+=1)
	{
		for (size_t j = 0; j < 8; j++)
		{
			ppu.tile_table[spr_tiles_ind+i].bit0[8-j-1] = spr_tiles[i*8+j];
			ppu.tile_table[spr_tiles_ind+i].bit1[8-j-1] = spr_tiles[4*8+i*8+j];
			
		}
		
	}
	
	
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);

	constexpr float PlayerSpeed = 30.0f;
	if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			ppu.background[x+PPU466::BackgroundWidth*y] = (33 + ((x%8)+(y%8)*8) % 64) | (4 << 8) ;
		}
	}

	ppu.sprites[0].x = int32_t(player_at.x);
	ppu.sprites[0].y = int32_t(player_at.y)+8;
	ppu.sprites[0].index = 29;
	ppu.sprites[0].attributes = 2;

	ppu.sprites[1].x = int32_t(player_at.x)+8;
	ppu.sprites[1].y = int32_t(player_at.y)+8;
	ppu.sprites[1].index = 28;
	ppu.sprites[1].attributes = 2;

	ppu.sprites[2].x = int32_t(player_at.x);
	ppu.sprites[2].y = int32_t(player_at.y);
	ppu.sprites[2].index = 31;
	ppu.sprites[2].attributes = 2;

	ppu.sprites[3].x = int32_t(player_at.x)+8;
	ppu.sprites[3].y = int32_t(player_at.y);
	ppu.sprites[3].index = 30;
	ppu.sprites[3].attributes = 2;

	ppu.sprites[4].x = 80;
	ppu.sprites[4].y = 80+8;
	ppu.sprites[4].index = 29;
	ppu.sprites[4].attributes = 3;

	ppu.sprites[5].x = 80+8;
	ppu.sprites[5].y = 80+8;
	ppu.sprites[5].index = 28;
	ppu.sprites[5].attributes = 3;

	ppu.sprites[6].x = 80;
	ppu.sprites[6].y = 80;
	ppu.sprites[6].index = 31;
	ppu.sprites[6].attributes = 3;

	ppu.sprites[7].x = 80+8;
	ppu.sprites[7].y = 80;
	ppu.sprites[7].index = 30;
	ppu.sprites[7].attributes = 3;


}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some hsv-like fade:
	// ppu.background_color = glm::u8vec4(
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
	// 	0xff
	// );

	//tilemap gets recomputed every frame as some weird plasma thing:
	//NOTE: don't do this in your game! actually make a map or something :-)
	// for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
	// 	for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
	// 		ppu.background[x+PPU466::BackgroundWidth*y] = (33 + ((x%8)+(y%8)*8) % 64) | (5 << 8) ;
	// 	}
	// }

	//background scroll:
	// ppu.background_position.x = int32_t(-0.5f * player_at.x);
	// ppu.background_position.y = int32_t(-0.5f * player_at.y);

	//player sprite:
	// ppu.sprites[0].x = int32_t(player_at.x);
	// ppu.sprites[0].y = int32_t(player_at.y);
	// ppu.sprites[0].index = 32;
	// ppu.sprites[0].attributes = 7;

	//some other misc sprites:
	// for (uint32_t i = 1; i < 63; ++i) {
	// 	float amt = (i + 2.0f * background_fade) / 62.0f;
	// 	ppu.sprites[i].x = int32_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
	// 	ppu.sprites[i].y = int32_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
	// 	ppu.sprites[i].index = 32;
	// 	ppu.sprites[i].attributes = 6;
	// 	if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	// }

	//--- actually draw ---
	ppu.draw(drawable_size);
}

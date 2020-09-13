#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"
#include "load_assets.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {

	// load assets
	load_background(&ppu);
	
	load_sprites(&ppu);

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

	// load timestamps for each grid
	for (size_t i = 0; i < grid_row*grid_col; i++)
	{
		grid_timestamps.push_back(time_limit);
	}

	srand (time(NULL));
	random_target();

}

PlayMode::~PlayMode() {
}

void PlayMode::random_target(){
	target_index = rand() % (grid_row * grid_col);

	target_at.y = (target_index / grid_col) * (PPU466::ScreenHeight / grid_row);
	target_at.x = (target_index % grid_col) * (PPU466::ScreenWidth / grid_col);

	std::cout <<  "target index: " << target_index << "\n";

	// initialize
	for (size_t i = 0; i < grid_timestamps.size(); i++)
	{
		grid_timestamps[i] = time_limit;
	}

	show_target = -1.f;
}

void PlayMode::clear_status(){
	random_target();

	player_at.x = 0;
	player_at.y = 0;

	ppu.sprites[4].y = 241;
	ppu.sprites[5].y = 241;
	ppu.sprites[6].y = 241;
	ppu.sprites[7].y = 241;

	target_y = 0.f;
	up_speed = 100.f;

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
		} else if (evt.key.keysym.sym == SDLK_SPACE){
			space.pressed = true;
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
		} else if (evt.key.keysym.sym == SDLK_SPACE){
			space.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	if (show_target >= time_limit)
	{
		clear_status();
		return;
	}
	
	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);

	if (show_target < 0)
	{
		constexpr float PlayerSpeed = 60.0f;
		if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
		if (right.pressed) player_at.x += PlayerSpeed * elapsed;
		if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
		if (up.pressed) player_at.y += PlayerSpeed * elapsed;
		if (player_at.x < 0)
		{
			player_at.x = 0;
		}
		if (player_at.x > PPU466::ScreenWidth-16)
		{
			player_at.x = PPU466::ScreenWidth-16;
		}
		if (player_at.y < 0)
		{
			player_at.y = 0;
		}
		if (player_at.y > PPU466::ScreenHeight-16)
		{
			player_at.y = PPU466::ScreenHeight-16;
		}

		if (space.pressed){
			// clear the grass
			int grid_index = (int32_t(player_at.x+8) / (PPU466::ScreenWidth / grid_col)) + (int32_t(player_at.y+8) / (PPU466::ScreenHeight / grid_row)) * grid_col;
			if (grid_timestamps[grid_index] >= time_limit)
			{
				grid_timestamps[grid_index] = 0.f;
			}
			if (grid_index == target_index)
			{
				show_target = 0.f;
			}
			// std::cout << player_at.x << " " << player_at.y << " " << grid_index << "\n" ;
		}

		//reset button press counters:
		left.downs = 0;
		right.downs = 0;
		up.downs = 0;
		down.downs = 0;

		for (uint32_t y = 0; y < PPU466::BackgroundHeight / 2; ++y) {
			for (uint32_t x = 0; x < PPU466::BackgroundWidth / 2; ++x) {
				int grid_index = int(x / (PPU466::BackgroundWidth / 2 / grid_col)) + (int(y / (PPU466::BackgroundHeight / 2 / grid_row))) * grid_col;
				if (grid_timestamps[grid_index] >= time_limit)
				{
					ppu.background[x+PPU466::BackgroundWidth*y] = (33 + ((x%8)+(y%8)*8) % 64) | (5 << 8) ;
				}else{
					// std::cout << x << " " << y << " " << grid_index << " " << x+PPU466::BackgroundWidth*y << "\n";
					ppu.background[x+PPU466::BackgroundWidth*y] = (33 + ((x%8)+(y%8)*8) % 64) | (4 << 8) ;
				}
				
			}
		}
	}


	if (show_target < 0)
	{
		for (size_t i = 0; i < grid_timestamps.size(); i++)
		{
			if (grid_timestamps[i] < time_limit){
				grid_timestamps[i] += elapsed;
			}
		}
	}
	
	
	if (show_target >= 0)
	{
		show_target += elapsed;
		if (up_speed >= 0 && target_index < grid_row * grid_col - grid_col)
		{
			target_y += up_speed * elapsed;
			up_speed -= elapsed * 200;
		}
	}
	
	
	// player
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

	// target 
	if (show_target >= 0){
		ppu.sprites[4].x = target_at.x + 8;
		ppu.sprites[4].y = target_at.y + 8 + target_y;
		ppu.sprites[4].index = 29;
		ppu.sprites[4].attributes = 3;

		ppu.sprites[5].x = target_at.x + 16;
		ppu.sprites[5].y = target_at.y+8+target_y;
		ppu.sprites[5].index = 28;
		ppu.sprites[5].attributes = 3;

		ppu.sprites[6].x = target_at.x + 8;
		ppu.sprites[6].y = target_at.y+target_y;
		ppu.sprites[6].index = 31;
		ppu.sprites[6].attributes = 3;

		ppu.sprites[7].x = target_at.x + 16;
		ppu.sprites[7].y = target_at.y+target_y;
		ppu.sprites[7].index = 30;
		ppu.sprites[7].attributes = 3;
	}

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- actually draw ---
	ppu.draw(drawable_size);
}

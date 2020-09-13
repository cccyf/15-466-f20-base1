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
			ppu.background[x+PPU466::BackgroundWidth*y] = (33 + ((x%8)+(y%8)*8) % 64) | (5 << 8) ;
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

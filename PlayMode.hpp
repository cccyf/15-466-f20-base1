#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	void random_target();
	void clear_status();

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space;

	//some weird background animation:
	float background_fade = 0.0f;

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);
	glm::vec2 target_at = glm::vec2(0.0f);
	float target_y = 0.f;
	float up_speed = 100.f;

	std::vector<float> grid_timestamps;
	int grid_row = 10;
	int grid_col = 8;
	float time_limit = 3.0f;
	int target_index = 0;
	float show_target = -1.f;


	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};

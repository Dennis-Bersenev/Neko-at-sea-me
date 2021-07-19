#pragma once

#include "common.hpp"
#include "render_components.hpp"
#include "render.hpp"
#include "text.hpp"

namespace Cutscene {

	extern vec2 window_dims;
	extern std::shared_ptr<Font> font;

	void get_cutscene_word(float elapsed_ms, vec2 window_dims);


};

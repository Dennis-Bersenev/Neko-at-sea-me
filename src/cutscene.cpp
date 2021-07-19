// Header
#include "cutscene.hpp"
#include "tiny_ecs.hpp"
#include "render.hpp"
#include "text.hpp"
#include "displaytext.hpp"

#include <cmath>
#include <iostream>

#include "render_components.hpp"

namespace Cutscene
{
    void get_cutscene_word(float elapsed_ms, vec2 window_size) {
		for (auto entity : ECS::registry<CutsceneComponent>.entities) {
			// Progress timer
			auto& counter = ECS::registry<CutsceneComponent>.get(entity);
			counter.counter_ms -= elapsed_ms;
			float padding = 50;
			// Restart the game once the transition timer expired
			float init_time = 16000;
			float interval = 900;

			if (counter.counter_ms < init_time && counter.counter_ms >  init_time - interval) {
				DisplayText::createDisplayText({ window_size.x / 4 - 2.f* padding, window_size.y / 4 }, "c_1", "cutscene_1.png", 1.0);
			}else if (counter.counter_ms < (init_time - interval) && counter.counter_ms >  (init_time - (interval * 2))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("Paw D. Roger, feline scourge of the seven seas,", font, { window_size.x / 2.f - 4.f*padding, window_size.y / 4.f - 2.f * padding }));
				ECS::registry<Text>.get(e1).scale = 0.5;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };
			}
			else if (counter.counter_ms < (init_time - (interval * 2)) && counter.counter_ms > (init_time - (interval * 3))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("had finally been apprehended", font, { 3*window_size.x / 4.f - 4.f * padding, window_size.y / 4.f - padding }));
				ECS::registry<Text>.get(e1).scale = 0.5;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };
			}
			else if (counter.counter_ms < (init_time - (interval * 3)) && counter.counter_ms > (init_time - (interval * 4))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("dragged to the execution stand.", font, { window_size.x / 2.f - 2.0f *padding, window_size.y / 4.f }));
				ECS::registry<Text>.get(e1).scale = 0.5;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };
			}
			else if (counter.counter_ms < (init_time - (interval * 4)) && counter.counter_ms > (init_time - (interval * 5))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("Just before the axe fell, someone in the crowd yelled:", font, { window_size.x / 2.f + 2.5f * padding, window_size.y / 4.f + padding }));
				ECS::registry<Text>.get(e1).scale = 0.5;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };
			
			}
			else if (counter.counter_ms < (init_time - (interval * 5)) && counter.counter_ms > (init_time - (interval * 6))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("Where is the Treasure!!! Where is One Puss ? !? ", font, { window_size.x / 2.f - 4.f * padding , window_size.y / 4.f + 2.f * padding }));
				ECS::registry<Text>.get(e1).scale = 0.7;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 0.0f, 0.0f };
				

			}
			else if (counter.counter_ms < (init_time - (interval * 6)) && counter.counter_ms > (init_time - (interval * 7))) {
				DisplayText::createDisplayText({ window_size.x / 2 - 3.5f * padding, window_size.y / 2 }, "c_2", "cutscene_3.png", 1.0);
			}
			else if (counter.counter_ms < (init_time - (interval * 7)) && counter.counter_ms > (init_time - (interval * 8))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("Chuckling, Roger lifted his head", font, { window_size.x / 2.f  , window_size.y / 4.f + 3.f * padding }));
				ECS::registry<Text>.get(e1).scale = 0.5;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };
			}
			else if (counter.counter_ms < (init_time - (interval * 8)) && counter.counter_ms > (init_time - (interval * 9))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("and laughed for a good ten seconds before intoning:", font, { window_size.x / 2.f + padding , window_size.y / 4.f + 4.f * padding }));
				ECS::registry<Text>.get(e1).scale = 0.5;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };
			}
			else if (counter.counter_ms < (init_time - (interval * 9)) && counter.counter_ms > (init_time - (interval * 10))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("My treasure is yours for the taking, ", font, { window_size.x / 2.f + 1.5f*padding , window_size.y / 4.f + 5.f * padding }));
				ECS::registry<Text>.get(e1).scale = 0.6;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 0.0f, 1.0f };
			}
			else if (counter.counter_ms < (init_time - (interval * 10)) && counter.counter_ms > (init_time - (interval * 11))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("But you will have to find it first. I left everything I own", font, { window_size.x / 2.f - 1.25f * padding , window_size.y / 4.f + 6.f * padding }));
				ECS::registry<Text>.get(e1).scale = 0.6;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 0.0f, 1.0f };
			}
			else if (counter.counter_ms < (init_time - (interval * 11)) && counter.counter_ms > (init_time - (interval * 12))) {
				DisplayText::createDisplayText({ window_size.x / 4 - 2.f * padding, 3 * window_size.y / 4 }, "c_3", "cutscene_2.png", 1.0);
			}
			else if (counter.counter_ms < (init_time - (interval * 12)) && counter.counter_ms > (init_time - (interval * 13))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("100 years have passed and it is 1689, the zenith of the golden age of purrates.", font, { window_size.x / 2.f -6.f * padding , window_size.y / 4.f + 9.f * padding }));
				ECS::registry<Text>.get(e1).scale = 0.5;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };
			}
			else if (counter.counter_ms < (init_time - (interval * 13)) && counter.counter_ms > (init_time - (interval * 14))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("Oceans rages with battles of purrates, all looking for the teasure", font, { window_size.x / 2.f - 5.f * padding , window_size.y / 4.f + 10.f * padding }));
				ECS::registry<Text>.get(e1).scale = 0.5;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };
			}
			else if (counter.counter_ms < (init_time - (interval * 14)) && counter.counter_ms > (init_time - (interval * 15))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("Will the treasure be found...", font, { window_size.x / 2.f - 4.f * padding , window_size.y / 4.f + 11.f * padding }));
				ECS::registry<Text>.get(e1).scale = 0.5;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };
			}
			else if (counter.counter_ms < (init_time - (interval * 15)) && counter.counter_ms > (init_time - (interval * 16))) {
				ECS::Entity e1;
				ECS::registry<Text>.insert(e1, Text("Only time will tell", font, { window_size.x / 2.f - 1.f * padding , window_size.y / 4.f + 12.f * padding }));
				ECS::registry<Text>.get(e1).scale = 0.5;
				ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };
			}
			else if (counter.counter_ms < 0) {
				if (ECS::registry<TransitionTimer>.components.size() <= 0) {
					ECS::Entity e1;
					if (!ECS::registry<TransitionTimer>.has(e1)) {
						ECS::registry<TransitionTimer>.emplace(e1);
					}
				}
			}
		}


    }
    vec2 window_dims = { 0,0 };
	std::shared_ptr<Font> font = NULL;//placeholder text until we figure out what we want
}


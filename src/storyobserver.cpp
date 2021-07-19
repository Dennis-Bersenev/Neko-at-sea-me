#include <functional>
#include <iostream>
#include <string>

#include "storyobserver.hpp"
#include "story_textbox.hpp"
#include "observer_pattern.hpp"
#include "displaytext.hpp"
#include "world.hpp"
#include "shark.hpp"
#include "boat.hpp"
#include "island.hpp"
#include "crate.hpp"
#include "button.hpp"
#include "cat.hpp"

#include "text.hpp"

int s_left_padding = 40.f;
int s_y_padding = 50.f;
int s_line_padding = 150.f;
int s_padding = 200.f;
ivec2 s_wind_size;

StorySystem::StorySystem(WorldSystem& world)
{
    s_wind_size = world.get_window_size();
    // initialize local variable;
    chapter_1_b = 0;
    chapter_1_e = 0;

    chapter_2_b = 0;
    chapter_2_e = 0;

    chapter_3_b = 0;
    chapter_3_e = 0;

    chapter_4_b = 0;
    chapter_4_e = 0;

    font = Font::load("data/fonts/Noto/NotoSans-Regular.ttf");//placeholder text until we figure out what we want

    // add observer
    world.subj_story_listener.add_observer_entity([&](ECS::Entity& entity) {on_next_text(entity); });
    world.subj_restart_story_listener.add_observer([&] {restart_scene(); });
}

void clear_story_list(std::vector<ECS::Entity> list) {
    for (int i = 0; i < list.size(); i++) {
        if (ECS::registry<Text>.has(list[i])) {
            ECS::registry<Text>.remove(list[i]);
        }
        if (ECS::registry<StoryTextbox>.has(list[i])) {
            ECS::registry<StoryTextbox>.remove(list[i]);
            ECS::ContainerInterface::remove_all_components_of(list[i]);
        }
        if (ECS::registry<CatComponent>.has(list[i])) {
            ECS::registry<CatComponent>.remove(list[i]);
            ECS::ContainerInterface::remove_all_components_of(list[i]);
        }
        if (ECS::registry<DisplayTextComponent>.has(list[i])) {
            ECS::registry<DisplayTextComponent>.remove(list[i]);
            ECS::ContainerInterface::remove_all_components_of(list[i]);
        }
    }
}

void remove_skip() {
    for (auto entity : ECS::registry<Button>.entities) {
        auto& text = ECS::registry<TextComponent>.get(entity);
        if (text.text == "skip_story") {
            if (ECS::registry<Text>.has(entity)) {
                ECS::registry<Text>.remove(entity);
            }
            ECS::ContainerInterface::remove_all_components_of(entity);
        }
    }
}
//void removehelptool(std::vector<ECS::Entity> list) {
//    if (ECS::registry<Helptool>.components.size() > 0) {
//        for (auto entity : ECS::registry<Helptool>.entities) {
//            ECS::ContainerInterface::remove_all_components_of(entity);
//        }
//    }
//    for (int i = 0; i < list.size(); i++) {
//        if (ECS::registry<Text>.has(list[i])) {
//            ECS::registry<Text>.remove(list[i]);
//        }
//    }
//
//}

void  StorySystem::on_next_text(ECS::Entity entity) {

    std::string level = ECS::registry<StoryLevel>.get(entity).level;
    bool after_battle = ECS::registry<StoryLevel>.get(entity).after_battle;

    float line_x = 75.f;
    float line_y = 50.f;
    float line = 25.f;

    if (after_battle == false) {

        if (level == "1s") {
            clear_story_list(curr_cutscene);
            if (chapter_1_b == 0) {
                //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
                //auto& ccomp = ECS::registry<CatComponent>.get(cat);
                //ccomp.scale = ccomp.scale * 3.0f;

                ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
                auto& tcom = ECS::registry<StoryComponent>.get(textbox);
                float x_scale = abs(tcom.scale.x) / 2.f;
                float y_scale = abs(tcom.scale.y) / 2.f;

                ECS::Entity textEntity0;
                ECS::Entity textEntity1;
                ECS::Entity textEntity2;
                ECS::Entity textEntity3;
                ECS::Entity textEntity4;
                ECS::Entity textEntity5;

                ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
                ECS::registry<Text>.get(textEntity0).scale = 0.5;
                ECS::registry<Text>.insert(textEntity1, Text("The golden age of purrates. After the death of Paw D. Roger, everyone began to set", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line *2.f}));
                ECS::registry<Text>.get(textEntity1).scale = 0.4;
                ECS::registry<Text>.insert(textEntity2, Text("out to sea to find his treasure, the One Puss. Overcoming their hatred of water in", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line *3.f}));
                ECS::registry<Text>.get(textEntity2).scale = 0.4;
                ECS::registry<Text>.insert(textEntity3, Text(" their lust for fish and shiny objects, feline buccaneers take to the waves in ships ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
                ECS::registry<Text>.get(textEntity3).scale = 0.4;
                ECS::registry<Text>.insert(textEntity4, Text("bristling with outlandish weaponry, ready to plunder far and wide across the", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
                ECS::registry<Text>.get(textEntity4).scale = 0.4;
                ECS::registry<Text>.insert(textEntity5, Text("Catlantic Ocean. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
                ECS::registry<Text>.get(textEntity5).scale = 0.4;

                curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
                chapter_1_b++;
            }
            else if (chapter_1_b == 1) {
                //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "sailor_cat_talking.png", "sailor_cat");
                //auto& ccomp = ECS::registry<CatComponent>.get(cat);
                //ccomp.scale = ccomp.scale * 3.0f;

                ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
                auto& tcom = ECS::registry<StoryComponent>.get(textbox);
                float x_scale = abs(tcom.scale.x) / 2.f;
                float y_scale = abs(tcom.scale.y) / 2.f;

                ECS::Entity textEntity0;
                ECS::Entity textEntity1;
                ECS::Entity textEntity2;
                ECS::Entity textEntity3;
                ECS::Entity textEntity4;
                ECS::Entity textEntity5;

                ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
                ECS::registry<Text>.get(textEntity0).scale = 0.5;
                ECS::registry<Text>.insert(textEntity1, Text("But the Catlantic Ocean was no ordinary ocean. This treacherous body of water was ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
                ECS::registry<Text>.get(textEntity1).scale = 0.4;
                ECS::registry<Text>.insert(textEntity2, Text("covered in a mysterious fog making it almost impossible to navigate. Only the most ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
                ECS::registry<Text>.get(textEntity2).scale = 0.4;
                ECS::registry<Text>.insert(textEntity3, Text("skills could survive and sail these waters. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
                ECS::registry<Text>.get(textEntity3).scale = 0.4;
                ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
                ECS::registry<Text>.get(textEntity4).scale = 0.4;
                ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
                ECS::registry<Text>.get(textEntity5).scale = 0.4;

                curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
                chapter_1_b++;
            }
            else if (chapter_1_b == 2) {
                //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
                //auto& ccomp = ECS::registry<CatComponent>.get(cat);
                //ccomp.scale = ccomp.scale * 3.0f;

                ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
                auto& tcom = ECS::registry<StoryComponent>.get(textbox);
                float x_scale = abs(tcom.scale.x) / 2.f;
                float y_scale = abs(tcom.scale.y) / 2.f;

                ECS::Entity textEntity0;
                ECS::Entity textEntity1;
                ECS::Entity textEntity2;
                ECS::Entity textEntity3;
                ECS::Entity textEntity4;
                ECS::Entity textEntity5;

                ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
                ECS::registry<Text>.get(textEntity0).scale = 0.5;
                ECS::registry<Text>.insert(textEntity1, Text("Many years have passed and there was still no luck in finding the treasure. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
                ECS::registry<Text>.get(textEntity1).scale = 0.4;
                ECS::registry<Text>.insert(textEntity2, Text("Where is it? Where did he hide the treasure?  Where is the One Puss? ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
                ECS::registry<Text>.get(textEntity2).scale = 0.4;
                ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
                ECS::registry<Text>.get(textEntity3).scale = 0.4;
                ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
                ECS::registry<Text>.get(textEntity4).scale = 0.4;
                ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
                ECS::registry<Text>.get(textEntity5).scale = 0.4;

                curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
                chapter_1_b++;
            }
            else if (chapter_1_b == 3) {
                ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_0");
                auto& ccomp = ECS::registry<CatComponent>.get(cat);
                ccomp.scale = ccomp.scale * 3.0f;
                ccomp.row = 0;

                ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
                auto& tcom = ECS::registry<StoryComponent>.get(textbox);
                float x_scale = abs(tcom.scale.x) / 2.f;
                float y_scale = abs(tcom.scale.y) / 2.f;

                ECS::Entity textEntity0;
                ECS::Entity textEntity1;
                ECS::Entity textEntity2;
                ECS::Entity textEntity3;
                ECS::Entity textEntity4;
                ECS::Entity textEntity5;

                ECS::registry<Text>.insert(textEntity0, Text("???", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
                ECS::registry<Text>.get(textEntity0).scale = 0.5;
                ECS::registry<Text>.insert(textEntity1, Text("Finally I am old enough to go out to find the One Puss. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
                ECS::registry<Text>.get(textEntity1).scale = 0.4;
                ECS::registry<Text>.insert(textEntity2, Text("I have been training all my life for this!!!! I am so excited!!!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
                ECS::registry<Text>.get(textEntity2).scale = 0.4;
                ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
                ECS::registry<Text>.get(textEntity3).scale = 0.4;
                ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
                ECS::registry<Text>.get(textEntity4).scale = 0.4;
                ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
                ECS::registry<Text>.get(textEntity5).scale = 0.4;

                curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
                chapter_1_b++;
            }
            else if (chapter_1_b == 4) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "sailor_cat_expression.png", "sailor_map_2");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 2;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("???", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Hold your tail, Rusty!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
            }
            else if (chapter_1_b == 5) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Huh...What is now, Purrito? You told me when I finally turned three years", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("old then I could go out into the Catlantic Ocean. Looked I even overcame", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("my fear of the water and can swim now.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
            }
            else if (chapter_1_b == 6) {
            //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            //auto& ccomp = ECS::registry<CatComponent>.get(cat);
            //ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Rusty suddenly jumped into a nearby lake to demonstrate his swimming", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("abilities and to show off his amazing paw stroke.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
            chapter_1_b++;
            }
            else if (chapter_1_b == 7) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "sailor_cat_expression.png", "sailor_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Purrito", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("I am not stopping you but I need to warn you that the outside of KittenLake is much", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("is much more dangerous than you think. There is a thick fog out there and without", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("without the proper navigating skills you will be sinking down into Davy Jones", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("Litter Box.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
            }
            else if (chapter_1_b == 8) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "sailor_cat_expression.png", "sailor_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Purrito", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Plus, I do not think you were expecting to go off in a raft or something?", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("You do not even have a boat. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
            }
            else if (chapter_1_b == 9) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Oh yeah... I forgot about that small detail. Hehehe...", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
            }
            else if (chapter_1_b == 10) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "sailor_cat_expression.png", "sailor_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Purrito", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Oh, you are as clueless as ever. You might be an adult but you still", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("have a mind of a kitten. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
            }
            else if (chapter_1_b == 11) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "sailor_cat_expression.png", "sailor_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Purrito", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Here, I will let you have one of my ships but you must pass my test", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("before I can allow you to go out into Catlantic.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
            }
            else if (chapter_1_b == 12) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("You are on! I will pass this test! No problem!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
            }
            else if (chapter_1_b == 13) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "sailor_cat_talking.png", "sailor_cat");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Purrito", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("First of all, let me explain a bit about sailing and avoiding dangers.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;

            //we need to put in the tutorial here 
            }
            else if (chapter_1_b == 14) {

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f + 50.f }, "story_text", true, 0);
            ECS::Entity page = DisplayText::createDisplayText({ s_wind_size.x / 2.f,s_wind_size.y / 2.f + 50.f }, "page_0", "page_0.png", 4);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            tcom.scale = { tcom.scale.x * 1.3f,tcom.scale.y * 3.f };
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;


            curr_cutscene = { textbox,page};
            chapter_1_b++;

            //we need to put in the tutorial here 
            }
            else if (chapter_1_b == 15) {

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f + 50.f }, "story_text", true, 0);
            ECS::Entity page = DisplayText::createDisplayText({ s_wind_size.x / 2.f,s_wind_size.y / 2.f + 50.f }, "page_1", "page_1.png", 4);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            tcom.scale = { tcom.scale.x * 1.3f,tcom.scale.y * 3.f };
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;


            curr_cutscene = { textbox,page };
            chapter_1_b++;

            //we need to put in the tutorial here 
            }
            else if (chapter_1_b == 16) {

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f + 50.f }, "story_text", true, 0);
            ECS::Entity page = DisplayText::createDisplayText({ s_wind_size.x / 2.f,s_wind_size.y / 2.f + 50.f }, "page_2", "page_2.png", 4);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            tcom.scale = { tcom.scale.x * 1.3f,tcom.scale.y * 3.f };
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;


            curr_cutscene = { textbox,page };
            chapter_1_b++;

            //we need to put in the tutorial here 
            }
            else if (chapter_1_b == 17) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "sailor_cat_expression.png", "sailor_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Purrito", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Alright... Let us begin!!!! All you need to do is to take out all of my ships.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;

            //we need to put in the tutorial here 
            }
            else {
                ECS::Entity next_button = Button::createButton({ s_wind_size.x - 100 * 2.f, s_wind_size.y - 100.f }, "Next");
                ECS::registry<Text>.insert(next_button, Text("Next", font, { s_wind_size.x - 250.f, s_wind_size.y - 100.f }));
                ECS::registry<Text>.get(next_button).scale = 0.6;
                remove_skip();
            }
        }
        else if (level == "2s") {
            clear_story_list(curr_cutscene);
            if (chapter_2_b == 0) {
                //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
                //auto& ccomp = ECS::registry<CatComponent>.get(cat);
                //ccomp.scale = ccomp.scale * 3.0f;

                ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
                auto& tcom = ECS::registry<StoryComponent>.get(textbox);
                float x_scale = abs(tcom.scale.x) / 2.f;
                float y_scale = abs(tcom.scale.y) / 2.f;

                ECS::Entity textEntity0;
                ECS::Entity textEntity1;
                ECS::Entity textEntity2;
                ECS::Entity textEntity3;
                ECS::Entity textEntity4;
                ECS::Entity textEntity5;

                ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
                ECS::registry<Text>.get(textEntity0).scale = 0.5;
                ECS::registry<Text>.insert(textEntity1, Text("Its only been a few weeks since Rusty has set out into the depths of Catlantic.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
                ECS::registry<Text>.get(textEntity1).scale = 0.4;
                ECS::registry<Text>.insert(textEntity2, Text("But being a very forgetful little kitten, he forgot something very important to bring ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
                ECS::registry<Text>.get(textEntity2).scale = 0.4;
                ECS::registry<Text>.insert(textEntity3, Text("on an adventure.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
                ECS::registry<Text>.get(textEntity3).scale = 0.4;
                ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
                ECS::registry<Text>.get(textEntity4).scale = 0.4;
                ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
                ECS::registry<Text>.get(textEntity5).scale = 0.4;

                curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
                chapter_2_b++;
            }
            else if (chapter_2_b == 1) {
                ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_3");
                auto& ccomp = ECS::registry<CatComponent>.get(cat);
                ccomp.scale = ccomp.scale * 3.0f;
                ccomp.row = 3;

                ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
                auto& tcom = ECS::registry<StoryComponent>.get(textbox);
                float x_scale = abs(tcom.scale.x) / 2.f;
                float y_scale = abs(tcom.scale.y) / 2.f;

                ECS::Entity textEntity0;
                ECS::Entity textEntity1;
                ECS::Entity textEntity2;
                ECS::Entity textEntity3;
                ECS::Entity textEntity4;
                ECS::Entity textEntity5;

                ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
                ECS::registry<Text>.get(textEntity0).scale = 0.5;
                ECS::registry<Text>.insert(textEntity1, Text("Uhhhh! I am so hungry!!!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
                ECS::registry<Text>.get(textEntity1).scale = 0.4;
                ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
                ECS::registry<Text>.get(textEntity2).scale = 0.4;
                ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
                ECS::registry<Text>.get(textEntity3).scale = 0.4;
                ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
                ECS::registry<Text>.get(textEntity4).scale = 0.4;
                ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
                ECS::registry<Text>.get(textEntity5).scale = 0.4;

                curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
                chapter_2_b++;
            }
            else if (chapter_2_b == 2) {
                ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_3");
                auto& ccomp = ECS::registry<CatComponent>.get(cat);
                ccomp.scale = ccomp.scale * 3.0f;
                ccomp.row = 3;

                ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
                auto& tcom = ECS::registry<StoryComponent>.get(textbox);
                float x_scale = abs(tcom.scale.x) / 2.f;
                float y_scale = abs(tcom.scale.y) / 2.f;

                ECS::Entity textEntity0;
                ECS::Entity textEntity1;
                ECS::Entity textEntity2;
                ECS::Entity textEntity3;
                ECS::Entity textEntity4;
                ECS::Entity textEntity5;

                ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
                ECS::registry<Text>.get(textEntity0).scale = 0.5;
                ECS::registry<Text>.insert(textEntity1, Text("Why did Purrito not tell me I needed to bring more than just a", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
                ECS::registry<Text>.get(textEntity1).scale = 0.4;
                ECS::registry<Text>.insert(textEntity2, Text("few treats when going on an adventure. I am going to starve to death before", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
                ECS::registry<Text>.get(textEntity2).scale = 0.4;
                ECS::registry<Text>.insert(textEntity3, Text("I can even start finding any clues on the One Puss. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
                ECS::registry<Text>.get(textEntity3).scale = 0.4;
                ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
                ECS::registry<Text>.get(textEntity4).scale = 0.4;
                ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
                ECS::registry<Text>.get(textEntity5).scale = 0.4;

                curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
                chapter_2_b++;
            }
            else if (chapter_2_b == 3) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "business_cat_expression.png", "business_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

                ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
                auto& tcom = ECS::registry<StoryComponent>.get(textbox);
                float x_scale = abs(tcom.scale.x) / 2.f;
                float y_scale = abs(tcom.scale.y) / 2.f;

                ECS::Entity textEntity0;
                ECS::Entity textEntity1;
                ECS::Entity textEntity2;
                ECS::Entity textEntity3;
                ECS::Entity textEntity4;
                ECS::Entity textEntity5;

                ECS::registry<Text>.insert(textEntity0, Text("???", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
                ECS::registry<Text>.get(textEntity0).scale = 0.5;
                ECS::registry<Text>.insert(textEntity1, Text("Hmmmm... you look a little peckish there, young kitten. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
                ECS::registry<Text>.get(textEntity1).scale = 0.4;
                ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
                ECS::registry<Text>.get(textEntity2).scale = 0.4;
                ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
                ECS::registry<Text>.get(textEntity3).scale = 0.4;
                ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
                ECS::registry<Text>.get(textEntity4).scale = 0.4;
                ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
                ECS::registry<Text>.get(textEntity5).scale = 0.4;

                curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
                chapter_2_b++;
            }
            else if (chapter_2_b == 4) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_3");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 3;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Uhhhh...more than peckish. I think I am dying of hunger.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
            }
            else if (chapter_2_b == 5) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Wait a minute! Who the paws are you?", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
            }
            else if (chapter_2_b == 6) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "business_cat_expression.png", "business_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("???", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Pardon me,sir. My name is Meowsir Von Whiskerston. I am quite a thriving", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("merchant around these parts. I am always selling goods at a reasonable", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("price. I think I might have something that might be of interest to you.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
            }
            else if (chapter_2_b == 7) {
            //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            //auto& ccomp = ECS::registry<CatComponent>.get(cat);
            //ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Suddenly a massive crate filled with cat treats and fishes", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("was displayed right in front of you.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
            chapter_2_b++;
            }
            else if (chapter_2_b == 8) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Ohhh wow... is that for me???", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
            }
            else if (chapter_2_b == 9) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "business_cat_expression.png", "business_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Meowsir", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("It can be yours for one hundred thousand kitty koins. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
            }
            else if (chapter_2_b == 10) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("But I do not have single koin on my ships.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
            }
            else if (chapter_2_b == 11) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "business_cat_expression.png", "business_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Meowsir", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Well if you do not have any koins then how about we have a battle for it.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("If you win, then you will get this crate of fish but if i win, then I get ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("your ships.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
            }
            else if (chapter_2_b == 12) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("DEAL!!!!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
            }
            else if (chapter_2_b == 13) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "business_cat_expression.png", "business_map_2");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 2;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Meowsir", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Alright! Let us begin! Hehehe! Your ships will be mine.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
            }
            else {
                ECS::Entity next_button = Button::createButton({ s_wind_size.x - 100 * 2.f, s_wind_size.y - 100.f }, "Next");
                ECS::registry<Text>.insert(next_button, Text("Next", font, { s_wind_size.x - 250.f, s_wind_size.y - 100.f }));
                ECS::registry<Text>.get(next_button).scale = 0.6;
                remove_skip();
            }

        }
        else if (level == "3s") {
        //this is chapter 3 
        clear_story_list(curr_cutscene);
        if (chapter_3_b == 0) {
            //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            //auto& ccomp = ECS::registry<CatComponent>.get(cat);
            //ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("While sailing around the East of Catlantic, you suddenly meet a Cat", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text(" wearing a crown on a giant elegant-looking ship. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
            chapter_3_b++;
        }
        else if (chapter_3_b == 1) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Are you some sort of royalty? Like a princess?", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_3_b++;
        }
        else if (chapter_3_b == 2) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "princess_cat_expression.png", "princess_map_2");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 2;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("???", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Yes. What of it? Do you not know who you are talking to", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("or should I spell it out for you so you might understand since you do not", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("seem like the bright type.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_3_b++;
        }
        else if (chapter_3_b == 3) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_1");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Oh no. I understand your grace or should I... I mean I understand", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("your majesty.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_3_b++;
        }
        else if (chapter_3_b == 4) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "princess_cat_expression.png", "princess_map_2");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 2;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("???", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("Huh. Your majesty has a name. My name is Elisapaw Meow du paw do ClawFord II", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("but you can call me Elisapaw. Now! Prepare yourself peasant as I, Elisapaw will defeat", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("you in battle.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_3_b++;
        }
        else if (chapter_3_b == 5) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_1");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 1;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("Wait, what? Your majesty? Hold on a second? Why are we battling.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_3_b++;
        }
        else if (chapter_3_b == 6) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "princess_cat_expression.png", "princess_map_2");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 2;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Elisapaw", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("It is Elisapaw... Fire the cannons!!!!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_3_b++;
        }
        else {
            ECS::Entity next_button = Button::createButton({ s_wind_size.x - 100 * 2.f, s_wind_size.y - 100.f }, "Next");
            ECS::registry<Text>.insert(next_button, Text("Next", font, { s_wind_size.x - 250.f, s_wind_size.y - 100.f }));
            ECS::registry<Text>.get(next_button).scale = 0.6;
            remove_skip();
        }
        }
        else if (level == "4s") {
        //this is chapter 4
        clear_story_list(curr_cutscene);
        if (chapter_4_b == 0) {
            //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            //auto& ccomp = ECS::registry<CatComponent>.get(cat);
            //ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("After the battle at the East Catlantic, Elisapaw has now joined your party.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("Due to her being in Catlantic longer than you have, she suggest to look for an", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("old famous purrate named Fishbone Joefur.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
            chapter_4_b++;
        }
        else if (chapter_4_b == 1) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "princess_cat_expression.png", "princess_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Elisapaw", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("He is one of the oldest purrates in the Catlantic. Cats and Kittens say that", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("he took down multiple ships at once with only one ship alone. He is dangerous", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("but is the only lead we got. Cats have been spreading a rumor that he found a", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("part of a map that could lead them to the One Puss.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_4_b++;
        }
        else if (chapter_4_b == 2) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Wow... Hes got a map to the location of the One Puss!!!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_4_b++;
        }
        else if (chapter_4_b == 3) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "princess_cat_talking.png", "princess_cat");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Elisapaw", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Well... Only part of a map. Simpleton. But it is our only lead for finding", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("the treasure. I don't think you wanna spend the rest of your life aimlessly ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("sailing around this fog until you happen to bump into the location of the", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("One Puss.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_4_b++;
        }
        else if (chapter_4_b == 4) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Hmmm... I see. Where is he then?", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_4_b++;
        }
        else if (chapter_4_b == 5) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "princess_cat_expression.png", "princess_map_0");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 0;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Elisapaw", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("Nobody knows. After a few years, he suddenly vanished. Cats has been looking ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("everywhere where for his tail but nothing. But I, the wonderful and brilliant Elisapaw", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("found a clue to his whereabout. I believe he is in thickest part of the fog in the", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("Catlantic. It is the only place that purrates have not search since anything that goes in", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("never comes out in one piece.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_4_b++;
        }
        else if (chapter_4_b == 6) {
        //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_1");
        //auto& ccomp = ECS::registry<CatComponent>.get(cat);
        //ccomp.scale = ccomp.scale * 3.0f;
        //ccomp.row = 1;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("With that, Rusty and Elisapaw head to the center of the Catlantic, the Rising", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("Depths. They call it that due to its fog being extremely thick like it just ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("magically rising from the ocean water and never seem to stop.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
        chapter_4_b++;
        }
        else if (chapter_4_b == 7) {
        //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
        //auto& ccomp = ECS::registry<CatComponent>.get(cat);
        //ccomp.scale = ccomp.scale * 3.0f;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("Suddenly a mysterious boat came toward them from the fog.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5 };
        chapter_4_b++;
        }
        else if (chapter_4_b == 8) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "cat_example_expression.png", "pirate_map_2");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 2;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("???", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("Another ship? Who the paw is dumb enought to sail into here.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("You kittens really wanna end up in Davy Jones Litter boxs or ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("something.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_4_b++;
        }
        else if (chapter_4_b == 9) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "princess_cat_expression.png", "princess_map_1");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 1;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Elisapaw", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("Are you Fishbone Joefur. You do not look that old???", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_4_b++;
        }
        else if (chapter_4_b == 10) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "cat_example_expression.png", "pirate_map_2");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 2;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Joefur", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("Old does not mean that i am old in age? What do you want?", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("I am guessing you wanna get your hands on the map piece, eh. Well no!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("Bunch of stupid kitten are going to have this map.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_4_b++;
        }
        else if (chapter_4_b == 11) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_2");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 2;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("We are not stupid kitten. I am three years old. I am adult. We came here to find", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("you and the map piece. What can we do to convince you to give it to us?", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_4_b++;
        }
        else if (chapter_4_b == 12) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "cat_example_expression.png", "pirate_map_2");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 2;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Joefur", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("NOTHING! Go home! Go back to where you came from? This is no place for", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("yall.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_4_b++;
        }
        else if (chapter_4_b == 13) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_2");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 2;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("We are not going to leave until we get the map piece!!!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_4_b++;
        }
        else if (chapter_4_b == 14) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "cat_example_expression.png", "pirate_map_2");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 2;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Joefur", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("Well then, let us battle it out. If you are tough enough to survive. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_4_b++;
        }
        else {
            ECS::Entity next_button = Button::createButton({ s_wind_size.x - 100 * 2.f, s_wind_size.y - 100.f }, "Next");
            ECS::registry<Text>.insert(next_button, Text("Next", font, { s_wind_size.x - 250.f, s_wind_size.y - 100.f }));
            ECS::registry<Text>.get(next_button).scale = 0.6;
            remove_skip();
        }
        }
    }
    //this is after battles
    else if (after_battle == true) {
    if (level == "1s") {
        clear_story_list(curr_cutscene);
        if (chapter_1_b == 0) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "sailor_cat_expression.png", "sailor_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Purrito", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Congrats, I knew you could do it. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
        }
        else if (chapter_1_b == 1) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_3");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 3;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Oh whiskers, that was tougher than i thought it was gonna be? ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("But I made it through.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
        }
        else if (chapter_1_b == 2) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "sailor_cat_expression.png", "sailor_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Purrito", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("There are gonna be tougher challenges waiting for you out there in the Catlantic.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("Do not worry, as you keep sailing through the ocean, you will meet more cats", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("and see some wonderful places.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_1_b++;
        }
        else if (chapter_1_b == 3) {
            //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            //auto& ccomp = ECS::registry<CatComponent>.get(cat);
            //ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("And from that day forward, Rusty set out into Catlantic. What", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("adventures will be waiting for our little feline friend.will be waiting", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("for our little feline friend.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
            chapter_1_b++;
        }
        else {
            ECS::Entity next_button = Button::createButton({ s_wind_size.x - 100 * 2.f, s_wind_size.y - 100.f }, "Next");
            ECS::registry<Text>.insert(next_button, Text("Next", font, { s_wind_size.x - 250.f, s_wind_size.y - 100.f }));
            ECS::registry<Text>.get(next_button).scale = 0.6;
            remove_skip();
        }
    }
    else if (level == "2s") {
        clear_story_list(curr_cutscene);
        if (chapter_2_b == 0) {
            //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            //auto& ccomp = ECS::registry<CatComponent>.get(cat);
            //ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Your attacks on Meowsirs fleet were too powerful and ended up blowing up", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("all of his ships full of expensive cargo.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5 };
            chapter_2_b++;
        }
        else if (chapter_2_b == 1) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "business_cat_expression.png", "business_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Meowsir", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("No No No, my cargo of goods were in those ships!!! ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("How could you destroy my goods!!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
        }
        else if (chapter_2_b == 2) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Oh! Looks like I won the battle. Can I have my crate of food now!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
        }
        else if (chapter_2_b == 3) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "business_cat_expression.png", "business_map_2");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 2;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Meowsir", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Ugh... fine! You won this time but mark my words, it will nott be that easy", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("the next time.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_2_b++;
        }
        else if (chapter_2_b == 4) {
        //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
        //auto& ccomp = ECS::registry<CatComponent>.get(cat);
        //ccomp.scale = ccomp.scale * 3.0f;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("That night, Rusty finally could sleep with a full stomach.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
        chapter_2_b++;
        }
        else {
            ECS::Entity next_button = Button::createButton({ s_wind_size.x - 100 * 2.f, s_wind_size.y - 100.f }, "Next");
            ECS::registry<Text>.insert(next_button, Text("Next", font, { s_wind_size.x - 250.f, s_wind_size.y - 100.f }));
            ECS::registry<Text>.get(next_button).scale = 0.6;
            remove_skip();
        }

    }
    else if (level == "3s") {
        //this is chapter 3 
        clear_story_list(curr_cutscene);
        if (chapter_3_b == 0) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "princess_cat_expression.png", "princess_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Elisapaw", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("No No No! How could I have lost to you, Simpleton. What am i supposed to do", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("now.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_3_b++;
        }
        else if (chapter_3_b == 1) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_1");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 1;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("First of all, Can you please explain to me why did you attack me?", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_3_b++;
        }
        else if (chapter_3_b == 2) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "princess_cat_expression.png", "princess_map_3");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 3;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Elisapaw", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("I need to prove myself or else I am not good enough to rule my kingdom.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("I need to find the One Puss.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_3_b++;
        }
        else if (chapter_3_b == 3) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Maybe do not randomly challenge cats to battles or else you might end up ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("in Davy Jones litter box at the bottom of this ocean. Beside...I am going to", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("be the one who will find the One Puss so you should find something else", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("to prove yourself to your kingdom.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_3_b++;
        }
        else if (chapter_3_b == 4) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "princess_cat_expression.png", "princess_map_0");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 0;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Elisapaw", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("Fine... I am joining your crew now to find One Puss.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_3_b++;
        }
        else if (chapter_3_b == 5) {
        ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "story_cat_expression.png", "story_map_2");
        auto& ccomp = ECS::registry<CatComponent>.get(cat);
        ccomp.scale = ccomp.scale * 3.0f;
        ccomp.row = 2;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Rusty", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("Wait, What??? No, you are not. I can not have another cat on my ship.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("Stay away from my catnip and fish!!!!!", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
        chapter_3_b++;
        }
        else if (chapter_3_b == 6) {
        //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
        //auto& ccomp = ECS::registry<CatComponent>.get(cat);
        //ccomp.scale = ccomp.scale * 3.0f;

        ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
        auto& tcom = ECS::registry<StoryComponent>.get(textbox);
        float x_scale = abs(tcom.scale.x) / 2.f;
        float y_scale = abs(tcom.scale.y) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;

        ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.5;
        ECS::registry<Text>.insert(textEntity1, Text("After a long argument, Elisapaw convinced you to allow her to join your crew.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;

        curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
        chapter_3_b++;
        }
        else {
            ECS::Entity next_button = Button::createButton({ s_wind_size.x - 100 * 2.f, s_wind_size.y - 100.f }, "Next");
            ECS::registry<Text>.insert(next_button, Text("Next", font, { s_wind_size.x - 250.f, s_wind_size.y - 100.f }));
            ECS::registry<Text>.get(next_button).scale = 0.6;
            remove_skip();
        }
    }
    else if (level == "4s") {
        //this is chapter 4
        clear_story_list(curr_cutscene);
        if (chapter_4_b == 0) {
            ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 4.0, "cat_example_expression.png", "pirate_map_0");
            auto& ccomp = ECS::registry<CatComponent>.get(cat);
            ccomp.scale = ccomp.scale * 3.0f;
            ccomp.row = 0;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Joefur", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Well Whiskers, I fought this hard since I start my journey into the fog.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("You prove yourself as a worthy purrate. ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,cat };
            chapter_4_b++;
        }
        else if (chapter_4_b == 1) {
            //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            //auto& ccomp = ECS::registry<CatComponent>.get(cat);
            //ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("Finally after a hard battle against Fishbone Joefur. You finally got a ", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("piece of the map.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
            chapter_4_b++;
        }
        else if (chapter_4_b == 2) {
            //ECS::Entity cat = Cat::createCat({ s_wind_size.x / 4, (1.5f * s_wind_size.y) / 4 }, 6.0, 1.0, "story_cat_talking.png", "story_cat");
            //auto& ccomp = ECS::registry<CatComponent>.get(cat);
            //ccomp.scale = ccomp.scale * 3.0f;

            ECS::Entity textbox = StoryTextbox::createTextbox({ s_wind_size.x / 2.f,s_wind_size.y / 2.f }, "story_text", true, 0);
            auto& tcom = ECS::registry<StoryComponent>.get(textbox);
            float x_scale = abs(tcom.scale.x) / 2.f;
            float y_scale = abs(tcom.scale.y) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity0, Text("Narrator", font, { s_wind_size.x / 2.f - x_scale + line_x ,s_wind_size.y / 2.f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.5;
            ECS::registry<Text>.insert(textEntity1, Text("You wonder where the map piece will take you to next.", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { s_wind_size.x / 2.f - x_scale + line_x,s_wind_size.y / 2.f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;

            curr_cutscene = { textbox, textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5};
            chapter_4_b++;
        }
        else {
            ECS::Entity next_button = Button::createButton({ s_wind_size.x - 100 * 2.f, s_wind_size.y - 100.f }, "Next");
            ECS::registry<Text>.insert(next_button, Text("Next", font, { s_wind_size.x - 250.f, s_wind_size.y - 100.f }));
            ECS::registry<Text>.get(next_button).scale = 0.6;
            remove_skip();
        }
    }

    }
    ECS::Entity e1;
    ECS::registry<Text>.insert(e1, Text("Click on text box to advances", font, { line_x, s_wind_size.y - 25.f }));
    ECS::registry<Text>.get(e1).scale = 0.5;
    ECS::registry<Text>.get(e1).colour = { 1.0f, 1.0f, 0.0f };

    ECS::Entity e2;
    if (level == "1s") {
        ECS::registry<Text>.insert(e2, Text("Chapter 1: Setting off to Catlantic", font, { line_x, line_y }));
        ECS::registry<Text>.get(e2).scale = 1;
        ECS::registry<Text>.get(e2).colour = { 1.0f, 1.0f, 0.0f };
    }
    else if (level == "2s") {
        ECS::registry<Text>.insert(e2, Text("Chapter 2: Hungry Business", font, { line_x, line_y }));
        ECS::registry<Text>.get(e2).scale = 1;
        ECS::registry<Text>.get(e2).colour = { 1.0f, 1.0f, 0.0f };
    }
    else if (level == "3s") {
        ECS::registry<Text>.insert(e2, Text("Chapter 3: Royal Meeting in the middle of the Sea", font, { line_x, line_y }));
        ECS::registry<Text>.get(e2).scale = 1;
        ECS::registry<Text>.get(e2).colour = { 1.0f, 1.0f, 0.0f };
    }
    else {
        ECS::registry<Text>.insert(e2, Text("Chapter 4: Pirate Showdown", font, { line_x, line_y }));
        ECS::registry<Text>.get(e2).scale = 1;
        ECS::registry<Text>.get(e2).colour = { 1.0f, 1.0f, 0.0f };

    }
    curr_cutscene.push_back(e1);
    curr_cutscene.push_back(e2);

}
void StorySystem::restart_scene() {
    //set all chapter scene to 0 so we can play out story again if needed
    chapter_1_b = 0;
    chapter_1_e = 0;

    chapter_2_b = 0;
    chapter_2_e = 0;

    chapter_3_b = 0;
    chapter_3_e = 0;

    chapter_4_b = 0;
    chapter_4_e = 0;

}




#include <functional>
#include <iostream>
#include <string>

#include "helpobserver.hpp"
#include "helptool.hpp"
#include "helpbox.hpp"
#include "observer_pattern.hpp"
#include "world.hpp"
#include "shark.hpp"
#include "boat.hpp"
#include "island.hpp"
#include "crate.hpp"
#include "button.hpp"
#include "story_textbox.hpp"

#include "text.hpp"

int left_padding = 40.f;
int y_padding = 50.f;
int line_padding = 150.f;
int padding = 200.f;
ivec2 wind_size;
HelpSystem::HelpSystem(WorldSystem& world)
{
    wind_size = world.get_window_size();
    // initialize local variable;
    menu_tip = 0;
    selection_tip = 0;
    game_tip = 0;
    level_tip = 0;

    font = Font::load("data/fonts/Noto/NotoSans-Regular.ttf");//placeholder text until we figure out what we want

    // add observer
    world.subj_help_listener.add_observer_entity([&](ECS::Entity& entity) {ongetHelp(entity); });
    world.subj_helptool_listener.add_observer_entity([&](ECS::Entity& entity) {hovergetHelp(entity); });
}

void clearlist(std::vector<ECS::Entity> list) {
    for (int i = 0; i < list.size(); i++) {
        if (ECS::registry<Text>.has(list[i])) {
            ECS::registry<Text>.remove(list[i]);
        }
        if (ECS::registry<Helpbox>.has(list[i])) {
            ECS::registry<Helpbox>.remove(list[i]);
            ECS::ContainerInterface::remove_all_components_of(list[i]);
        }
}

}
void removehelptool(std::vector<ECS::Entity> list) {
    if (ECS::registry<Helptool>.components.size() > 0) {
        for (auto entity : ECS::registry<Helptool>.entities) {
            ECS::ContainerInterface::remove_all_components_of(entity);
        }
    }
    for (int i = 0; i < list.size(); i++) {
        if (ECS::registry<Text>.has(list[i])) {
            ECS::registry<Text>.remove(list[i]);
        }
    }

}

void HelpSystem::ongetHelp(ECS::Entity entity)
{
    int scene = ECS::registry<Scene>.get(entity).scene;
    bool skip = ECS::registry<Scene>.get(entity).skip;
    int debug = ECS::registry<Scene>.get(entity).debug;
    if (skip == true) {
        if (scene == 0) {
            menu_tip = 10;
        }
        else if (scene == 1) {
            selection_tip = 10;
        }
        else if (scene == 2) {
            game_tip = 10;
        }
        else {
            level_tip = 10;
        }
    } 

    if (debug == 1) {
        //set all text skip
        menu_tip = 10;
        selection_tip = 10;
        game_tip = 10;
        level_tip = 10;
    }
    else if (debug == 2) {
        //set text to come back
        menu_tip = 0;
        selection_tip = 0;
        game_tip = 0;
        level_tip = 0;

    }

    onhelpScene(scene);
}

void HelpSystem::onhelpScene(int scene) {

    if (ECS::registry<Text>.has(advance_text)) {
        ECS::ContainerInterface::remove_all_components_of(advance_text);
    }

    if (menu_tip < 2 || selection_tip < 5 || game_tip < 6 || level_tip < 2) {
        if (!ECS::registry<Button>.has(skipbutton)) {
            if (scene != 2) {
                skipbutton = Button::createButton({ wind_size.x - padding, wind_size.y / 2 - 2.f * padding }, "skip");
            }
            else {
                skipbutton = Button::createButton({ wind_size.x - padding, wind_size.y / 2 - 1.5f * padding }, "skip");
            }
            if (!ECS::registry<Text>.has(skipbutton)) {
                if (scene != 2) {
                    ECS::registry<Text>.insert(skipbutton, Text("Skip", font, { wind_size.x - padding * 1.130, wind_size.y / 2 - 2.f * padding }));
                }
                else {
                    ECS::registry<Text>.insert(skipbutton, Text("Skip", font, { wind_size.x - padding * 1.130, wind_size.y / 2 - 1.5f * padding }));
                }
                ECS::registry<Text>.get(skipbutton).scale = 0.7;
            }
        }
        ECS::registry<Text>.insert(advance_text, Text("Click textbox to advance...", font, { 25.f, 25.f }));
        ECS::registry<Text>.get(advance_text).scale = 0.4;
        if (scene != 4 && scene != 0) {
            ECS::registry<Text>.get(advance_text).colour = { 1.0f, 1.0f, 0.0f };
        }
    }

    if (scene == 0) {
        if (menu_tip == 1) {
            std::cout << scene << std::endl;

            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ padding, wind_size.y / 2 - padding }, "help");
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;


            ECS::registry<Text>.insert(textEntity1, Text("Select Freeplay to play against", font, { left_padding, wind_size.y / 2 - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("robo-cat-captain!", font, { left_padding, wind_size.y / 2 - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("Select New Story to experience", font, { left_padding, wind_size.y / 2 - padding * 1.0f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("the story campaign", font, { left_padding, wind_size.y / 2 - padding * 0.875f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;

            curr_help_information = { help,textEntity1,textEntity2,textEntity3,textEntity4 };
            menu_tip++;
        }
        else if (menu_tip == 0) {

            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ padding, wind_size.y / 2 - padding }, "help");
            ECS::Entity textEntity0;
            ECS::Entity textEntity1;


            ECS::registry<Text>.insert(textEntity0, Text("Welcome to the", font, { left_padding, wind_size.y / 2 - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text(" World of Neko-At-Sea-Me!", font, { left_padding, wind_size.y / 2 - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;

            curr_help_information = { help,textEntity0,textEntity1 };
            menu_tip++;
        }
        else {

            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::ContainerInterface::remove_all_components_of(skipbutton);
            ECS::ContainerInterface::remove_all_components_of(advance_text);
        }
    }
    else if (scene == 1) {
        if (selection_tip == 0) {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ padding, wind_size.y / 2 - padding }, "help");
            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;

            ECS::registry<Text>.insert(textEntity0, Text("Welcome to boat selection!", font, { left_padding, wind_size.y / 2 - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("This is where you will assemble", font, { left_padding, wind_size.y / 2 - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("your fleet for battle!", font, { left_padding, wind_size.y / 2 - padding * 1.0f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            curr_help_information = { help,textEntity0,textEntity1,textEntity2 };
            selection_tip++;
        }
        else if (selection_tip == 1) {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ wind_size.x - padding, wind_size.y / 2 - padding }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            float x_scale = abs(hcom.scale.x) / 2.f;


            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;

            ECS::registry<Text>.insert(textEntity0, Text("Different ships will have advantages", font, { wind_size.x - padding - x_scale + 20.f, wind_size.y / 2 - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("and disadvantages so choose wisely!", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            curr_help_information = { help,textEntity0,textEntity1 };
            selection_tip++;
        }
        else if (selection_tip == 2) {

            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ wind_size.x - padding, wind_size.y / 2 - padding }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            float x_scale = abs(hcom.scale.x) / 2.f;


            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;

            ECS::registry<Text>.insert(textEntity0, Text("Each ship will have a cost,", font, { wind_size.x - padding - x_scale + 20.f, wind_size.y / 2 - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("which you can use token to select.", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("The total of token are indicated", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 1.0 }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("at the top.", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 0.875 }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;


            curr_help_information = { help,textEntity0,textEntity1,textEntity2,textEntity3 };
            selection_tip++;
        }
        else if (selection_tip == 3) {

            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ wind_size.x - padding, wind_size.y / 2 - padding }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            float x_scale = abs(hcom.scale.x) / 2.f;


            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;

            ECS::registry<Text>.insert(textEntity0, Text("Hover over the boat display", font, { wind_size.x - padding - x_scale + 20.f, wind_size.y / 2 - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("to get more information about", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("each boat", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 1.0 }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;


            curr_help_information = { help,textEntity0,textEntity1,textEntity2};
            selection_tip++;
        }
        else if (selection_tip == 4) {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ wind_size.x - padding, wind_size.y / 2 - padding }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            float x_scale = abs(hcom.scale.x) / 2.f;

            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;

            ECS::registry<Text>.insert(textEntity0, Text("You can also select a cat caption ", font, { wind_size.x - padding - x_scale + 20.f, wind_size.y / 2 - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("located on the left of the screen", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("Use the arrow keys below the ", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 1.0 }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("character portait to select a cat.", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 0.875 }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;


            curr_help_information = { help,textEntity0,textEntity1,textEntity2,textEntity3 };
            selection_tip++;

        }
        else if (selection_tip == 5) {
        clearlist(curr_help_information);
        curr_help_information.clear();
        ECS::Entity help = Helpbox::createHelpbox({ wind_size.x - padding, wind_size.y / 2 - padding }, "help");
        auto& hcom = ECS::registry<HelpComponent>.get(help);
        float x_scale = abs(hcom.scale.x) / 2.f;

        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;

        ECS::registry<Text>.insert(textEntity0, Text("Once you are finished choosing", font, { wind_size.x - padding - x_scale + 20.f, wind_size.y / 2 - padding * 1.25 }));
        ECS::registry<Text>.get(textEntity0).scale = 0.4;
        ECS::registry<Text>.insert(textEntity1, Text("your fleet, you can click the", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 1.125 }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("start button at the bottom", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 1.0 }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text(" to start the battle.", font, { wind_size.x - padding - x_scale + 20.f , wind_size.y / 2 - padding * 0.875 }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;


        curr_help_information = { help,textEntity0,textEntity1,textEntity2,textEntity3 };
        selection_tip++;

        }
        else {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::ContainerInterface::remove_all_components_of(skipbutton);
            ECS::ContainerInterface::remove_all_components_of(advance_text);

        }
    }
    else if (scene == 2) {
        if (game_tip == 0) {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ padding, wind_size.y / 2 - padding }, "help");
            ECS::Entity textEntity0;


            ECS::registry<Text>.insert(textEntity0, Text("It's time to battle!", font, { left_padding, wind_size.y / 2 - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity0).scale = 0.7;

            curr_help_information = { help,textEntity0 };
            game_tip++;

        }
        else if (game_tip == 1) {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ padding, wind_size.y / 2 - padding }, "help");
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity1, Text("In the beginning of every single round, you", font, { left_padding, wind_size.y / 2 - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.3;
            ECS::registry<Text>.insert(textEntity2, Text("will need to plan your fleets moves. This is", font, { left_padding, wind_size.y / 2 - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity2).scale = 0.3;
            ECS::registry<Text>.insert(textEntity3, Text("called the Planning Phase. As display at the ", font, { left_padding, wind_size.y / 2 - padding * 1.0 }));
            ECS::registry<Text>.get(textEntity3).scale = 0.3;
            ECS::registry<Text>.insert(textEntity4, Text("left top corner.", font, { left_padding, wind_size.y / 2 - padding * 0.875 }));
            ECS::registry<Text>.get(textEntity4).scale = 0.3;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { left_padding, wind_size.y / 2 - padding * 0.75 }));
            ECS::registry<Text>.get(textEntity5).scale = 0.3;

            curr_help_information = { help,textEntity1,textEntity2, textEntity3,textEntity4,textEntity5 };
            game_tip++;
        }
        else if (game_tip == 2) {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ padding, (wind_size.y * 0.75f)- padding }, "help");
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity1, Text("You can select a boat by left-clicking on it.", font, { left_padding, (wind_size.y * 0.75f)- padding * 1.25 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.3;
            ECS::registry<Text>.insert(textEntity2, Text("Once selected, you can draw a line to give it ", font, { left_padding, (wind_size.y * 0.75f) - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity2).scale = 0.3;
            ECS::registry<Text>.insert(textEntity3, Text("a path to move during Combat Phase. You can ", font, { left_padding, (wind_size.y * 0.75f) - padding * 1.0 }));
            ECS::registry<Text>.get(textEntity3).scale = 0.3;
            ECS::registry<Text>.insert(textEntity4, Text("also see the range of the boats view", font, { left_padding, (wind_size.y *0.75f) - padding * 0.875 }));
            ECS::registry<Text>.get(textEntity4).scale = 0.3;
            ECS::registry<Text>.insert(textEntity5, Text("(yellow dots) and attack range (red dots).", font, { left_padding, (wind_size.y * 0.75f) - padding * 0.75 }));
            ECS::registry<Text>.get(textEntity5).scale = 0.3;

            curr_help_information = { help,textEntity1,textEntity2, textEntity3,textEntity4,textEntity5 };
            game_tip++;
        }
        else if (game_tip == 3) {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ padding, (wind_size.y * 0.75f)- padding }, "help");
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity1, Text("To draw a path - select a boat, hold right-click", font, { left_padding, (wind_size.y * 0.75f) - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.3;
            ECS::registry<Text>.insert(textEntity2, Text("on the mouse and drag it from the boat to their ", font, { left_padding, (wind_size.y * 0.75f) - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity2).scale = 0.3;
            ECS::registry<Text>.insert(textEntity3, Text("destination. Also, right-clicking anywhere", font, { left_padding, (wind_size.y * 0.75f) - padding * 1.0 }));
            ECS::registry<Text>.get(textEntity3).scale = 0.3;
            ECS::registry<Text>.insert(textEntity4, Text("while boat is selected will create a straight", font, { left_padding, (wind_size.y * 0.75f) - padding * 0.875 }));
            ECS::registry<Text>.get(textEntity4).scale = 0.3;
            ECS::registry<Text>.insert(textEntity5, Text("path from the boat to it destination.", font, { left_padding, (wind_size.y * 0.75f) - padding * 0.75 }));
            ECS::registry<Text>.get(textEntity5).scale = 0.3;

            curr_help_information = { help,textEntity1,textEntity2, textEntity3,textEntity4,textEntity5 };
            game_tip++;
        }
        else if (game_tip == 4) {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ padding, (wind_size.y * 0.75f)-padding }, "help");
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;

            ECS::registry<Text>.insert(textEntity1, Text("While a boat is selected, you can see their", font, { left_padding, (wind_size.y * 0.75f) - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.3;
            ECS::registry<Text>.insert(textEntity3, Text("abilities. Each boat has a different abilities to", font, { left_padding, (wind_size.y * 0.75f) - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity3).scale = 0.3;
            ECS::registry<Text>.insert(textEntity4, Text("use. But keept in mind, these have limited usage", font, { left_padding, (wind_size.y * 0.75f) - padding * 1.0 }));
            ECS::registry<Text>.get(textEntity4).scale = 0.3;
            ECS::registry<Text>.insert(textEntity5, Text("and cooldowns. Click the skill button to activate", font, { left_padding, (wind_size.y * 0.75f) - padding * 0.875 }));
            ECS::registry<Text>.get(textEntity5).scale = 0.3;

            curr_help_information = { help,textEntity1,textEntity2, textEntity3,textEntity4,textEntity5 };
            game_tip++;
        }
        else if (game_tip == 5) {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ wind_size.x - padding, wind_size.y / 2 - padding /2.f }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            float x_scale = abs(hcom.scale.x) / 2.f;

            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;
            ECS::Entity textEntity6;


            ECS::registry<Text>.insert(textEntity2, Text("Once you have finished planning all your", font, { wind_size.x - padding - x_scale + 20.f, wind_size.y / 2.f + (padding / 2.f) - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity2).scale = 0.3;
            ECS::registry<Text>.insert(textEntity3, Text("moves for the next turn, click End turn to move", font, { wind_size.x - padding - x_scale + 20.f, wind_size.y / 2.f + (padding /2.f) - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity3).scale = 0.3;
            ECS::registry<Text>.insert(textEntity4, Text("to Combat Phase which your ships perform the", font, { wind_size.x - padding - x_scale + 20.f, wind_size.y / 2.f + (padding / 2.f)- padding * 1.0 }));
            ECS::registry<Text>.get(textEntity4).scale = 0.3;
            ECS::registry<Text>.insert(textEntity5, Text("a actions given. Combat Phase will only last", font, { wind_size.x - padding - x_scale + 20.f, wind_size.y / 2.f + (padding / 2.f) - padding * 0.875 }));
            ECS::registry<Text>.get(textEntity5).scale = 0.3;
            ECS::registry<Text>.insert(textEntity6, Text("until the timer reaches zero.", font, { wind_size.x - padding - x_scale + 20.f, wind_size.y / 2.f + (padding / 2.f)- padding * 0.75 }));
            ECS::registry<Text>.get(textEntity6).scale = 0.3;
            curr_help_information = { help,textEntity2,textEntity3,textEntity4,textEntity5,textEntity6 };
            game_tip++;
        }
        else {
            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::ContainerInterface::remove_all_components_of(skipbutton);
            ECS::ContainerInterface::remove_all_components_of(advance_text);
        }
    }
    //level selections
    else if (scene == 3) {
        if (level_tip == 0) {
            std::cout << scene << std::endl;

            clearlist(curr_help_information);
            curr_help_information.clear();
            ECS::Entity help = Helpbox::createHelpbox({ padding, wind_size.y / 2 - padding }, "help");
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;


            ECS::registry<Text>.insert(textEntity1, Text("Welcome to the map selection.", font, { left_padding, wind_size.y / 2 - padding * 1.25 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("This is where you choose", font, { left_padding, wind_size.y / 2 - padding * 1.125 }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("the map that you would like to", font, { left_padding, wind_size.y / 2 - padding * 1.0f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("play on!", font, { left_padding, wind_size.y / 2 - padding * 0.875f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;

            curr_help_information = { help,textEntity1,textEntity2,textEntity3,textEntity4 };
            level_tip++;
        }
    else if (level_tip == 1) {

        clearlist(curr_help_information);
        curr_help_information.clear();
        ECS::Entity help = Helpbox::createHelpbox({ padding, wind_size.y / 2 - padding }, "help");
        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;


        ECS::registry<Text>.insert(textEntity0, Text("Hover over the map button to get", font, { left_padding, wind_size.y / 2 - padding * 1.25 }));
        ECS::registry<Text>.get(textEntity0).scale = 0.4;
        ECS::registry<Text>.insert(textEntity1, Text("more info. Click on map button", font, { left_padding, wind_size.y / 2 - padding * 1.125 }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("to select", font, { left_padding, wind_size.y / 2 - padding * 1.0f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;

        curr_help_information = { help,textEntity0,textEntity1, textEntity2 };
        level_tip++;
    }
    else {
        clearlist(curr_help_information);
        curr_help_information.clear();
        ECS::ContainerInterface::remove_all_components_of(skipbutton);
        ECS::ContainerInterface::remove_all_components_of(advance_text);
    }

    }
    else {
    clearlist(curr_help_information);
    curr_help_information.clear();
    ECS::ContainerInterface::remove_all_components_of(skipbutton);
    ECS::ContainerInterface::remove_all_components_of(advance_text);

}

}

void HelpSystem::hovergetHelp(ECS::Entity entity) {
    int scene = ECS::registry<Scene>.get(entity).scene;
    if (ECS::registry<Button>.has(entity)) {
        std::string text = ECS::registry<TextComponent>.get(entity).text;
        if (scene == 0) {
            if (text == "load") {
                removehelptool(curr_help_tool);
                ECS::Entity help = Helptool::createHelptool({ wind_size.x/2.f,y_padding }, "help");
                auto& hcom = ECS::registry<HelpComponent>.get(help);
                float x_scale = abs(hcom.scale.x)/2.f;

                ECS::Entity textEntity0;
                TextComponent tag;
                tag.text = "help_tool";
                ECS::registry<TextComponent>.emplace(textEntity0, tag);

                ECS::registry<Text>.insert(textEntity0, Text("Select Continue to load up previous saved story chapters", font, { wind_size.x/2.f - x_scale +left_padding ,y_padding }));
                ECS::registry<Text>.get(textEntity0).scale = 0.4;
                curr_help_tool = { textEntity0 };

            }
            else if (text == "story") {
                removehelptool(curr_help_tool);
                ECS::Entity help = Helptool::createHelptool({ wind_size.x/2.f,y_padding }, "help");
                auto& hcom = ECS::registry<HelpComponent>.get(help);
                float x_scale = abs(hcom.scale.x) / 2.f;

                ECS::Entity textEntity0;
                ECS::Entity textEntity1;
                TextComponent tag;
                tag.text = "help_tool";
                ECS::registry<TextComponent>.emplace(textEntity0, tag);
                ECS::registry<TextComponent>.emplace(textEntity1, tag);

                ECS::registry<Text>.insert(textEntity0, Text("Select New Story to start a new story campaign", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding }));
                ECS::registry<Text>.get(textEntity0).scale = 0.4;
                ECS::registry<Text>.insert(textEntity1, Text("(Note: Selecting this will overwrite previous chapter save)", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding * 1.375 }));
                ECS::registry<Text>.get(textEntity1).scale = 0.4;
                curr_help_tool = { textEntity0, textEntity1 };

            }
            else if (text == "PVE") {
                removehelptool(curr_help_tool);
                ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding }, "help");
                auto& hcom = ECS::registry<HelpComponent>.get(help);
                float x_scale = abs(hcom.scale.x) / 2.f;

                ECS::Entity textEntity0;
                TextComponent tag;
                tag.text = "help_tool";
                ECS::registry<TextComponent>.emplace(textEntity0, tag);

                ECS::registry<Text>.insert(textEntity0, Text("Select Freeplay to play against to robo-cat captain!", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding }));
                ECS::registry<Text>.get(textEntity0).scale = 0.4;
                curr_help_tool = { textEntity0 };

            }
            else if (text == "Exit") {
                removehelptool(curr_help_tool);
                ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding }, "help");
                auto& hcom = ECS::registry<HelpComponent>.get(help);
                float x_scale = abs(hcom.scale.x) / 2.f;

                ECS::Entity textEntity0;
                TextComponent tag;
                tag.text = "help_tool";
                ECS::registry<TextComponent>.emplace(textEntity0, tag);

                ECS::registry<Text>.insert(textEntity0, Text("Exit the game", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding }));
                ECS::registry<Text>.get(textEntity0).scale = 0.4;
                curr_help_tool = { textEntity0 };

            }
            else if (text == "Options") {
                removehelptool(curr_help_tool);
                ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding }, "help");
                auto& hcom = ECS::registry<HelpComponent>.get(help);
                float x_scale = abs(hcom.scale.x) / 2.f;

                ECS::Entity textEntity0;
                TextComponent tag;
                tag.text = "help_tool";
                ECS::registry<TextComponent>.emplace(textEntity0, tag);

                ECS::registry<Text>.insert(textEntity0, Text("Adjust settings", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding }));
                ECS::registry<Text>.get(textEntity0).scale = 0.4;
                curr_help_tool = { textEntity0 };

            }

        }
        else if (scene == 1) {
            if (text == "Start") {
                removehelptool(curr_help_tool);
                ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding }, "help");
                auto& hcom = ECS::registry<HelpComponent>.get(help);
                float x_scale = abs(hcom.scale.x) / 2.f;


                ECS::Entity textEntity0;
                ECS::Entity textEntity1;
                TextComponent tag;
                tag.text = "help_tool";
                ECS::registry<TextComponent>.emplace(textEntity0, tag);
                ECS::registry<TextComponent>.emplace(textEntity1, tag);

                ECS::registry<Text>.insert(textEntity0, Text("After allocating ships to your fleet and selection your cat,", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding }));
                ECS::registry<Text>.get(textEntity0).scale = 0.4;
                ECS::registry<Text>.insert(textEntity1, Text("select Start to start the battle!", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding*1.375 }));
                ECS::registry<Text>.get(textEntity1).scale = 0.4;
                curr_help_tool = { textEntity0, textEntity1};
            }
            else if (text == "Back") {
                removehelptool(curr_help_tool);
                ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding }, "help");
                auto& hcom = ECS::registry<HelpComponent>.get(help);
                float x_scale = abs(hcom.scale.x) / 2.f;


                ECS::Entity textEntity0;
                TextComponent tag;
                tag.text = "help_tool";
                ECS::registry<TextComponent>.emplace(textEntity0, tag);

                ECS::registry<Text>.insert(textEntity0, Text("Select Back to go to the map selection!", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding }));
                ECS::registry<Text>.get(textEntity0).scale = 0.4;
                curr_help_tool = { textEntity0 };
            }

        }
        else if (scene == 2) {
            if (text == "End Turn") {
                removehelptool(curr_help_tool);
                ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding * 2.0 }, "help");
                auto& hcom = ECS::registry<HelpComponent>.get(help);
                float x_scale = abs(hcom.scale.x) / 2.f;


                ECS::Entity textEntity0;
                TextComponent tag;
                tag.text = "help_tool";
                ECS::registry<TextComponent>.emplace(textEntity0, tag);

                ECS::registry<Text>.insert(textEntity0, Text(" Done planning your fleet's movement, click End turn", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding * 2.0 }));
                ECS::registry<Text>.get(textEntity0).scale = 0.4;
                curr_help_tool = { textEntity0 };
            }
            else if (text == "game_pause") {
                removehelptool(curr_help_tool);
                ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding * 2.0 }, "help");
                auto& hcom = ECS::registry<HelpComponent>.get(help);
                float x_scale = abs(hcom.scale.x) / 2.f;


                ECS::Entity textEntity0;
                TextComponent tag;
                tag.text = "help_tool";
                ECS::registry<TextComponent>.emplace(textEntity0, tag);

                ECS::registry<Text>.insert(textEntity0, Text("Click to pause the game or press b", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding * 2.0 }));
                ECS::registry<Text>.get(textEntity0).scale = 0.4;
                curr_help_tool = { textEntity0 };
            }
        }
        else if (scene == 3) {
        if (text == "map_1") {
            removehelptool(curr_help_tool);
            ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            float x_scale = abs(hcom.scale.x) / 2.f;


            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            TextComponent tag;
            tag.text = "help_tool";
            ECS::registry<TextComponent>.emplace(textEntity0, tag);
            ECS::registry<TextComponent>.emplace(textEntity1, tag);
            ECS::registry<Text>.insert(textEntity0, Text("These waters are quite quiet as the name suggest...", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding}));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("Crates: High | Sharks: Low | Obstacles: Low ", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding * 1.375 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            curr_help_tool = { textEntity0,textEntity1 };
        }
        else if (text == "map_2") {
            removehelptool(curr_help_tool);
            ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            float x_scale = abs(hcom.scale.x) / 2.f;


            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            TextComponent tag;
            tag.text = "help_tool";
            ECS::registry<TextComponent>.emplace(textEntity0, tag);
            ECS::registry<TextComponent>.emplace(textEntity1, tag);
            ECS::registry<Text>.insert(textEntity0, Text("Little bit rougher but nothing to worry about...", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("Crates: Normal | Sharks: Normal | Obstacles: Normal ", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding * 1.375 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            curr_help_tool = { textEntity0,textEntity1 };
        }
        else if (text == "map_3") {
            removehelptool(curr_help_tool);
            ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            float x_scale = abs(hcom.scale.x) / 2.f;


            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            TextComponent tag;
            tag.text = "help_tool";
            ECS::registry<TextComponent>.emplace(textEntity0, tag);
            ECS::registry<TextComponent>.emplace(textEntity1, tag);
            ECS::registry<Text>.insert(textEntity0, Text("Eyes open! These parts are quite treacherous...", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("Crates: Low | Sharks: High | Obstacles: High ", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding * 1.375 }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            curr_help_tool = { textEntity0,textEntity1 };
        }

        float line_x = 50.f;
        float line_y = 50.f;
        float line = 25.f;

        if (text == "story_1s") {
            removehelptool(curr_help_tool);
            ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f + left_padding / 2.f, wind_size.y*0.75f}, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            hcom.scale = vec2{ hcom.scale.x *1.5f , hcom.scale.y * 2.5f };
            float x_scale = abs(hcom.scale.x) / 2.f;
            float y_scale = abs(hcom.scale.y) / 2.f;

            //chapter 1
            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;
            ECS::Entity textEntity6;
            ECS::Entity textEntity7;

            TextComponent tag;
            tag.text = "help_tool";
            ECS::registry<TextComponent>.emplace(textEntity0, tag);
            ECS::registry<TextComponent>.emplace(textEntity1, tag);
            ECS::registry<TextComponent>.emplace(textEntity2, tag);
            ECS::registry<TextComponent>.emplace(textEntity3, tag);
            ECS::registry<TextComponent>.emplace(textEntity4, tag);
            ECS::registry<TextComponent>.emplace(textEntity5, tag);
            ECS::registry<TextComponent>.emplace(textEntity6, tag);
            ECS::registry<TextComponent>.emplace(textEntity7, tag);
            ECS::registry<Text>.insert(textEntity0, Text("Chapter 1: Setting off to Catlantic", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x - line , wind_size.y * 0.75f - y_scale + line_y}));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("It has been years since the death of Paw D. Roger and nobody has found", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line}));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("any clues on the whereabouts of the One Puss. Rusty is now of age to", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line*2.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("set out to Catlantic to start his search for it but is he prepare for", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("the dangers that await him.", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;
            ECS::registry<Text>.insert(textEntity6, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity6).scale = 0.4;
            ECS::registry<Text>.insert(textEntity7, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 7.f }));
            ECS::registry<Text>.get(textEntity7).scale = 0.4;
            curr_help_tool = { textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,textEntity6,textEntity7 };
        }
        else if (text == "story_2s") {
            removehelptool(curr_help_tool);
            ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f + left_padding / 2.f, wind_size.y * 0.75f }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            hcom.scale = vec2{ hcom.scale.x * 1.5f , hcom.scale.y * 2.5f };
            float x_scale = abs(hcom.scale.x) / 2.f;
            float y_scale = abs(hcom.scale.y) / 2.f;

            //chapter 1
            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;
            ECS::Entity textEntity6;
            ECS::Entity textEntity7;

            TextComponent tag;
            tag.text = "help_tool";
            ECS::registry<TextComponent>.emplace(textEntity0, tag);
            ECS::registry<TextComponent>.emplace(textEntity1, tag);
            ECS::registry<TextComponent>.emplace(textEntity2, tag);
            ECS::registry<TextComponent>.emplace(textEntity3, tag);
            ECS::registry<TextComponent>.emplace(textEntity4, tag);
            ECS::registry<TextComponent>.emplace(textEntity5, tag);
            ECS::registry<TextComponent>.emplace(textEntity6, tag);
            ECS::registry<TextComponent>.emplace(textEntity7, tag);
            ECS::registry<Text>.insert(textEntity0, Text("Chapter 2: Hungry Business", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x - line , wind_size.y * 0.75f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("Rusty is finally starting his adventures into the Catlantic.", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("Unfortunatly he forgot the most important thing to being on his", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("journey.", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;
            ECS::registry<Text>.insert(textEntity6, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity6).scale = 0.4;
            ECS::registry<Text>.insert(textEntity7, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 7.f }));
            ECS::registry<Text>.get(textEntity7).scale = 0.4;
            curr_help_tool = { textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,textEntity6,textEntity7 };

        }
        else if (text == "story_3s") {
            removehelptool(curr_help_tool);
            ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f + left_padding / 2.f, wind_size.y * 0.75f }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            hcom.scale = vec2{ hcom.scale.x * 1.5f , hcom.scale.y * 2.5f };
            float x_scale = abs(hcom.scale.x) / 2.f;
            float y_scale = abs(hcom.scale.y) / 2.f;

            //chapter 1
            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;
            ECS::Entity textEntity6;
            ECS::Entity textEntity7;

            TextComponent tag;
            tag.text = "help_tool";
            ECS::registry<TextComponent>.emplace(textEntity0, tag);
            ECS::registry<TextComponent>.emplace(textEntity1, tag);
            ECS::registry<TextComponent>.emplace(textEntity2, tag);
            ECS::registry<TextComponent>.emplace(textEntity3, tag);
            ECS::registry<TextComponent>.emplace(textEntity4, tag);
            ECS::registry<TextComponent>.emplace(textEntity5, tag);
            ECS::registry<TextComponent>.emplace(textEntity6, tag);
            ECS::registry<TextComponent>.emplace(textEntity7, tag);
            ECS::registry<Text>.insert(textEntity0, Text("Chapter 3: Royal Meeting in the middle of the Sea", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x - line , wind_size.y * 0.75f - y_scale + line_y }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("While searching through East of the Catlantic, Rusty unexpectly bumped", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("into a unexpected royalty. Elisapaw ClawFord II is the next in line for the", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 2.f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("feline throne of Purrlandia but why is she in the middle of the Catlantic?", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 3.f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("Why has she suddenly decide to have a battle with you?", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 4.f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 5.f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.4;
            ECS::registry<Text>.insert(textEntity6, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 6.f }));
            ECS::registry<Text>.get(textEntity6).scale = 0.4;
            ECS::registry<Text>.insert(textEntity7, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 7.f }));
            ECS::registry<Text>.get(textEntity7).scale = 0.4;
            curr_help_tool = { textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,textEntity6,textEntity7 };

        }
        else if (text == "story_4s") {
        removehelptool(curr_help_tool);
        ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f + left_padding / 2.f, wind_size.y * 0.75f }, "help");
        auto& hcom = ECS::registry<HelpComponent>.get(help);
        hcom.scale = vec2{ hcom.scale.x * 1.5f , hcom.scale.y * 2.5f };
        float x_scale = abs(hcom.scale.x) / 2.f;
        float y_scale = abs(hcom.scale.y) / 2.f;

        //chapter 1
        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;
        ECS::Entity textEntity6;
        ECS::Entity textEntity7;

        TextComponent tag;
        tag.text = "help_tool";
        ECS::registry<TextComponent>.emplace(textEntity0, tag);
        ECS::registry<TextComponent>.emplace(textEntity1, tag);
        ECS::registry<TextComponent>.emplace(textEntity2, tag);
        ECS::registry<TextComponent>.emplace(textEntity3, tag);
        ECS::registry<TextComponent>.emplace(textEntity4, tag);
        ECS::registry<TextComponent>.emplace(textEntity5, tag);
        ECS::registry<TextComponent>.emplace(textEntity6, tag);
        ECS::registry<TextComponent>.emplace(textEntity7, tag);
        ECS::registry<Text>.insert(textEntity0, Text("Chapter 4: Pirate Showdown", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x - line , wind_size.y * 0.75f - y_scale + line_y }));
        ECS::registry<Text>.get(textEntity0).scale = 0.4;
        ECS::registry<Text>.insert(textEntity1, Text("Rusty has finally gotten a clue about the whereabouts of the ", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("One Puss. Unfortunatly it is in the depths of the most foggest place ", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 2.f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("in the Catlantic, the Rising Depths. Some purrates state that once you go in,", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 3.f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("you never will come back out in one piece.", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 4.f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 5.f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.4;
        ECS::registry<Text>.insert(textEntity6, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 6.f }));
        ECS::registry<Text>.get(textEntity6).scale = 0.4;
        ECS::registry<Text>.insert(textEntity7, Text("", font, { wind_size.x / 2.f + left_padding / 2.f - x_scale + line_x , wind_size.y * 0.75f - y_scale + line_y + line * 7.f }));
        ECS::registry<Text>.get(textEntity7).scale = 0.4;
        curr_help_tool = { textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5,textEntity6,textEntity7 };

        }
        else if (text == "Back") {
        removehelptool(curr_help_tool);
        ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding }, "help");
        auto& hcom = ECS::registry<HelpComponent>.get(help);
        float x_scale = abs(hcom.scale.x) / 2.f;


        ECS::Entity textEntity0;
        TextComponent tag;
        tag.text = "help_tool";
        ECS::registry<TextComponent>.emplace(textEntity0, tag);

        ECS::registry<Text>.insert(textEntity0, Text("Select Back to go to the main menu!", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding }));
        ECS::registry<Text>.get(textEntity0).scale = 0.4;
        curr_help_tool = { textEntity0 };
            }
}
    }
else if(ECS::registry<DisplayTextComponent>.has(entity)) {
    std::string text = ECS::registry<TextComponent>.get(entity).text;
    if (scene == 1) {
        if (text == "regular_boat_icon") {
            removehelptool(curr_help_tool);
            ECS::Entity help = Helptool::createHelptool({ wind_size.x / 3.f - 0.5 * padding,(3.f * wind_size.y) / 4.f - 0.3f * padding }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            hcom.scale = vec2{ 0.7 * hcom.scale.x,2.2f * hcom.scale.y };
            float x_scale = abs(hcom.scale.x) / 2.f;


            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;
            TextComponent tag;
            tag.text = "help_tool";
            ECS::registry<TextComponent>.emplace(textEntity0, tag);
            ECS::registry<TextComponent>.emplace(textEntity1, tag);
            ECS::registry<TextComponent>.emplace(textEntity2, tag);
            ECS::registry<TextComponent>.emplace(textEntity3, tag);
            ECS::registry<TextComponent>.emplace(textEntity4, tag);
            ECS::registry<TextComponent>.emplace(textEntity5, tag);
            ECS::registry<Text>.insert(textEntity0, Text("A standard boat that gets the job done!", font, { wind_size.x / 3.f - 0.5 * padding - x_scale + left_padding, wind_size.y / 2.f + 100.f * 1.125f }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("Name: Regular Boat | Token Cost: 4", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding, wind_size.y / 2.f + 100.f * 1.375f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("Boat Health: 100", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 1.625f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("Attack Damage: Low", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 1.875f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("Attack Range: Low", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 2.125f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("Skills: Emergency Repair, Risky Neutron Pulse ", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 2.375f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.3;
            curr_help_tool = { textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5 };


        }else if(text == "recon_boat_icon") {
            removehelptool(curr_help_tool);
            ECS::Entity help = Helptool::createHelptool({ wind_size.x / 3.f - 0.5 * padding,(3.f * wind_size.y) / 4.f - 0.3f * padding }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            hcom.scale = vec2{ 0.7 * hcom.scale.x,2.2f * hcom.scale.y };
            float x_scale = abs(hcom.scale.x) / 2.f;


            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;
            TextComponent tag;
            tag.text = "help_tool";
            ECS::registry<TextComponent>.emplace(textEntity0, tag);
            ECS::registry<TextComponent>.emplace(textEntity1, tag);
            ECS::registry<TextComponent>.emplace(textEntity2, tag);
            ECS::registry<TextComponent>.emplace(textEntity3, tag);
            ECS::registry<TextComponent>.emplace(textEntity4, tag);
            ECS::registry<TextComponent>.emplace(textEntity5, tag);
            ECS::registry<Text>.insert(textEntity0, Text("A great boat for scouting out the area!", font, { wind_size.x / 3.f - 0.5 * padding - x_scale + left_padding, wind_size.y / 2.f +  100.f *1.125f }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("Name: Recon Boat | Token Cost: 10", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding, wind_size.y / 2.f + 100.f * 1.375f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("Boat Health: 100", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 1.625f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("Attack Damage: Low", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 1.875f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("Attack Range: None", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 2.125f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("Skills: Engine Boost, Flares ", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 2.375f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.3;
            curr_help_tool = { textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5 };
        }
        else if (text == "big_boat_icon") {
            removehelptool(curr_help_tool);
            ECS::Entity help = Helptool::createHelptool({ wind_size.x / 3.f - 0.5 * padding,(3.f * wind_size.y) / 4.f - 0.3f * padding }, "help");
            auto& hcom = ECS::registry<HelpComponent>.get(help);
            hcom.scale = vec2{ 0.7 * hcom.scale.x,2.2f * hcom.scale.y };
            float x_scale = abs(hcom.scale.x) / 2.f;


            ECS::Entity textEntity0;
            ECS::Entity textEntity1;
            ECS::Entity textEntity2;
            ECS::Entity textEntity3;
            ECS::Entity textEntity4;
            ECS::Entity textEntity5;
            TextComponent tag;
            tag.text = "help_tool";
            ECS::registry<TextComponent>.emplace(textEntity0, tag);
            ECS::registry<TextComponent>.emplace(textEntity1, tag);
            ECS::registry<TextComponent>.emplace(textEntity2, tag);
            ECS::registry<TextComponent>.emplace(textEntity3, tag);
            ECS::registry<TextComponent>.emplace(textEntity4, tag);
            ECS::registry<TextComponent>.emplace(textEntity5, tag);
            ECS::registry<Text>.insert(textEntity0, Text("Bigger the boat, the tanker it is... ", font, { wind_size.x / 3.f - 0.5 * padding - x_scale + left_padding, wind_size.y / 2.f + 100.f * 1.125f }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.insert(textEntity1, Text("Name: Big Boat | Token Cost: 2", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding, wind_size.y / 2.f + 100.f * 1.375f }));
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
            ECS::registry<Text>.insert(textEntity2, Text("Boat Health: 200", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 1.625f }));
            ECS::registry<Text>.get(textEntity2).scale = 0.4;
            ECS::registry<Text>.insert(textEntity3, Text("Attack Damage: Normal", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 1.875f }));
            ECS::registry<Text>.get(textEntity3).scale = 0.4;
            ECS::registry<Text>.insert(textEntity4, Text("Attack Range: Normal", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 2.125f }));
            ECS::registry<Text>.get(textEntity4).scale = 0.4;
            ECS::registry<Text>.insert(textEntity5, Text("Skills: Missle Defense MK2, Missile Targeting Override", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 2.375f }));
            ECS::registry<Text>.get(textEntity5).scale = 0.3;
            curr_help_tool = { textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5 };
        }
        else if (text == "attack_boat_icon") {
        removehelptool(curr_help_tool);
        ECS::Entity help = Helptool::createHelptool({ wind_size.x / 3.f - 0.5 * padding,(3.f * wind_size.y) / 4.f - 0.3f * padding }, "help");
        auto& hcom = ECS::registry<HelpComponent>.get(help);
        hcom.scale = vec2{ 0.7 * hcom.scale.x,2.2f * hcom.scale.y };
        float x_scale = abs(hcom.scale.x) / 2.f;


        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        ECS::Entity textEntity2;
        ECS::Entity textEntity3;
        ECS::Entity textEntity4;
        ECS::Entity textEntity5;
        TextComponent tag;
        tag.text = "help_tool";
        ECS::registry<TextComponent>.emplace(textEntity0, tag);
        ECS::registry<TextComponent>.emplace(textEntity1, tag);
        ECS::registry<TextComponent>.emplace(textEntity2, tag);
        ECS::registry<TextComponent>.emplace(textEntity3, tag);
        ECS::registry<TextComponent>.emplace(textEntity4, tag);
        ECS::registry<TextComponent>.emplace(textEntity5, tag);
        ECS::registry<Text>.insert(textEntity0, Text("A boat that packs some punches", font, { wind_size.x / 3.f - 0.5 * padding - x_scale + left_padding, wind_size.y / 2.f + 100.f * 1.125f }));
        ECS::registry<Text>.get(textEntity0).scale = 0.4;
        ECS::registry<Text>.insert(textEntity1, Text("Name: Attack Boat | Token Cost: 6", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding, wind_size.y / 2.f + 100.f * 1.375f }));
        ECS::registry<Text>.get(textEntity1).scale = 0.4;
        ECS::registry<Text>.insert(textEntity2, Text("Boat Health: 80", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 1.625f }));
        ECS::registry<Text>.get(textEntity2).scale = 0.4;
        ECS::registry<Text>.insert(textEntity3, Text("Attack Damage: High", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 1.875f }));
        ECS::registry<Text>.get(textEntity3).scale = 0.4;
        ECS::registry<Text>.insert(textEntity4, Text("Attack Range: High", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 2.125f }));
        ECS::registry<Text>.get(textEntity4).scale = 0.4;
        ECS::registry<Text>.insert(textEntity5, Text("Skills: Open Fire, Fire Power Boost ", font, { wind_size.x / 3.f - 0.5f * padding - x_scale + left_padding,wind_size.y / 2.f + 100.f * 2.375f }));
        ECS::registry<Text>.get(textEntity5).scale = 0.3;
        curr_help_tool = { textEntity0,textEntity1,textEntity2,textEntity3,textEntity4,textEntity5 };
        }

    }


    }
    else {
        if (scene == 2) {
            handle_other_help(entity);
        }
        
    }

}

void HelpSystem::handle_other_help(ECS::Entity entity) {

    if (ECS::registry<Boat>.has(entity)) {
        removehelptool(curr_help_tool);

        Status& boat_status = ECS::registry<Status>.get(entity);
        std::string name = boat_status.name;
        std::string health = std::to_string((int)boat_status.health);
        std::string attack_range = std::to_string((int)boat_status.attack_range);
        std::string weapon_damage = std::to_string((int)boat_status.weapon_damage);
        std::string speed = std::to_string((int)boat_status.speed);


        ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,2*y_padding }, "help");
        auto& hcom = ECS::registry<HelpComponent>.get(help);
        float x_scale = abs(hcom.scale.x) / 2.f;



        ECS::Entity textEntity0;
        ECS::Entity textEntity1;
        TextComponent tag;
        tag.text = "help_tool";
        ECS::registry<TextComponent>.emplace(textEntity0, tag);
        ECS::registry<TextComponent>.emplace(textEntity1, tag);
        if (!ECS::registry<AIComponent>.has(entity)) {
            ECS::registry<Text>.insert(textEntity0, Text("Name: " + name + " | Health: " + health + " | Damage: " + weapon_damage + " | Speed: " + speed, font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding * 2.5 }));
            ECS::registry<Text>.insert(textEntity1, Text("Select a ship and click location to move set route!", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding * 2.0 }));

            ECS::registry<Text>.get(textEntity0).scale = 0.4;
            ECS::registry<Text>.get(textEntity1).scale = 0.4;
        }
        else {
            ECS::registry<Text>.insert(textEntity0, Text("Name: " + name + " | Health: " + health + " | Damage: " + weapon_damage + " | Speed: " + speed, font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding * 2.0 }));
            ECS::registry<Text>.get(textEntity0).scale = 0.4;
        }
        curr_help_tool = { textEntity0,textEntity1 };

    }
    else if (ECS::registry<Shark>.has(entity)) {
        removehelptool(curr_help_tool);
        ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding*2.0 }, "help");
        auto& hcom = ECS::registry<HelpComponent>.get(help);
        float x_scale = abs(hcom.scale.x) / 2.f;


        ECS::Entity textEntity0;
        TextComponent tag;
        tag.text = "help_tool";
        ECS::registry<TextComponent>.emplace(textEntity0, tag);

        ECS::registry<Text>.insert(textEntity0, Text("Sharks are dangerous! Try to avoid them!", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding*2.0 }));
        ECS::registry<Text>.get(textEntity0).scale = 0.4;
        curr_help_tool = { textEntity0 };

    }
    else if (ECS::registry<Crate>.has(entity)) {
        removehelptool(curr_help_tool);
        ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding*2.0 }, "help");
        auto& hcom = ECS::registry<HelpComponent>.get(help);
        float x_scale = abs(hcom.scale.x) / 2.f;


        ECS::Entity textEntity0;
        TextComponent tag;
        tag.text = "help_tool";
        ECS::registry<TextComponent>.emplace(textEntity0, tag);

        ECS::registry<Text>.insert(textEntity0, Text("There could be something useful in theses", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding*2.0 }));
        ECS::registry<Text>.get(textEntity0).scale = 0.4;
        curr_help_tool = { textEntity0 };

    }
    else if (ECS::registry<Island>.has(entity)) {
        removehelptool(curr_help_tool);
        ECS::Entity help = Helptool::createHelptool({ wind_size.x / 2.f,y_padding*2.0 }, "help");
        auto& hcom = ECS::registry<HelpComponent>.get(help);
        float x_scale = abs(hcom.scale.x) / 2.f;


        ECS::Entity textEntity0;
        TextComponent tag;
        tag.text = "help_tool";
        ECS::registry<TextComponent>.emplace(textEntity0, tag);

        ECS::registry<Text>.insert(textEntity0, Text("There are spiky rocks around here", font, { wind_size.x / 2.f - x_scale + left_padding ,y_padding*2.0 }));
        ECS::registry<Text>.get(textEntity0).scale = 0.4;
        curr_help_tool = { textEntity0 };
    }

}




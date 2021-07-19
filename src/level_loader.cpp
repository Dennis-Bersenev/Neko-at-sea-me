using namespace std;
#include <iostream>
#include <fstream>
#include <list>
#include "level_loader.hpp"
#include "common.hpp"
#include "tiny_ecs.hpp"
#include "nlohmann/json.hpp"
#include "shark.hpp"
#include "crate.hpp"
#include "island.hpp"
#include "player.hpp"

using namespace nlohmann;

string OBSTACLES = "obstacles";
string SPAWNS = "spawns";
string SHARK = "shark";
string ISLAND = "island";
string CRATE = "crate";
string XPOS = "xpos";
string YPOS = "ypos";
string TYPE = "type";
string ISLAND_TYPE = "i_type";

LevelSystem::LevelSystem() {
    auto entity = ECS::Entity();

    ECS::registry<Obstacles>.emplace(entity);

}

LevelSystem::~LevelSystem() {

}

struct obstacle {
    int id;
    int xpos;
    int ypos;
    string type;
};

void LevelSystem::load_level(string id) {
    string name = "data/levels/level" + id + ".json";
    ifstream i(name);
    json j;
    i >> j;

    auto& obs = j[OBSTACLES];
    for (unsigned int i = 0; i < obs.size(); i++) {
        auto type = j[OBSTACLES][i][TYPE];
        if (type == SHARK) {
            Shark::createShark(vec2(j[OBSTACLES][i][XPOS], j[OBSTACLES][i][YPOS]), 100.f);

        }else if (type == CRATE) {
            Crate::createCrate(vec2(j[OBSTACLES][i][XPOS], j[OBSTACLES][i][YPOS]));
        }
        else if (type == ISLAND) {
            Island::createIsland(vec2(j[OBSTACLES][i][XPOS], j[OBSTACLES][i][YPOS]), j[OBSTACLES][i][ISLAND_TYPE]);
        }
    }

    i.close();
    //load_boats(j);
}

void load_boats(json j) {
    auto& spawns = j[SPAWNS];
    int padding_x = 100.f;
    int x = 1;

    for(unsigned int i = 0; i < spawns.size(); i++) {
        vec2 spawn = vec2(j[SPAWNS][i][YPOS], j[SPAWNS][i][XPOS]);
        std::cout << ECS::registry<Player>.entities.size() << std::endl;
        for(auto& player: ECS::registry<Player>.entities) {
            std::cout << ECS::registry<Fleet>.get(player).boats.size() << std::endl;
            for(auto& boat: ECS::registry<Fleet>.get(player).boats) {
                auto& motion = ECS::registry<Motion>.get(boat);
                motion.position = spawn;
                motion.position.x += padding_x * x;
                /*
                auto& health = boat.get<Boat>().health;
                auto& health_motion = ECS::registry<Motion>.get(health);
                health_motion.position = spawn - vec2(0.f, 30.f);
                health_motion.position.x += padding_x * x;
                */
                x += 1;
            }

        }

    }

}

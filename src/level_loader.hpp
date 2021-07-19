// internal
using namespace std;

#include <nlohmann/json.hpp>
#include "common.hpp"
#include "tiny_ecs.hpp"
#include "string"
using namespace nlohmann;

class LevelSystem {
public:
    LevelSystem();

    // Releases all associated resources
    ~LevelSystem();

    static void load_level(string id);


};

void load_boats(json j);
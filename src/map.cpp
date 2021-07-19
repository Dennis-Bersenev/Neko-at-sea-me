#include <iostream>

#include "map.hpp"
#include "island.hpp"

Map::Map(vec2 win_size, WorldSystem& world)
{
	window_size = win_size;
	world.subj_level_done.add_observer([&] {remove_island();});
	init();
}

// utilities
void Map::add_edge_1d(int indx1, int indx2)
{
	// add_edge is only 1 direction
	// good for iterating through all verts, maybe using set?
	adj[indx1].push_back(indx2);
}

void Map::remove_edge_2d(int indx1, int indx2)
{
	// remove_edge is 2d
	// since the PREQ is the edge exists
	for (int i = 0; i < adj[indx1].size(); i++) {
		if (adj[indx1][i] == indx2) {
			adj[indx1].erase(adj[indx1].begin() + i);
			break;
		}
	}

	for (int i = 0; i < adj[indx2].size(); i++) {
		if (adj[indx2][i] == indx1) {
			adj[indx2].erase(adj[indx2].begin() + i);
			break;
		}
	}
}

VPos Map::get_VPos(int vert)
{
	if (vert == 0) 
	{
		return VPos::TL;
	}
	else if (vert == 9)
	{
		return VPos::TR;
	}
	else if (vert == 90)
	{
		return VPos::BL;
	}
	else if (vert == 99)
	{
		return VPos::BR;
	}
	else if (vert > 0 && vert < 9)
	{
		return VPos::Top;
	}
	else if (vert > 90 && vert < 99)
	{
		return VPos::Bottom;
	}
	else if (vert % 10 == 0)
	{
		return VPos::Left;
	}
	else if (vert % 10 == 9)
	{
		return VPos::Right;
	}
	else if (vert > -1 && vert < 100)
	{
		return VPos::Inner;
	}
	else
	{
		return VPos::Invalid;
	}
}

vec2 Map::get_pos(int vert)
{
	int x_dir = vert % 10;
	int y_dir = (vert / 10) % 10;
	float x_unit = window_size.x / 10;
	float y_unit = window_size.y / 10;
	vec2 ret = vec2{ x_dir * x_unit + x_unit / 2, y_dir * y_unit + y_unit / 2 };
	// std::cout << ret.x << " " << ret.y << std::endl;
	return (ret);
}

int Map::get_vert(vec2 pos)
{
	int x_dir = (pos.x / window_size.x) * 10; // 0-9
	int y_dir = (pos.y / window_size.y) * 10; // 0-9
	int ret = 10 * y_dir + x_dir;
	// std::cout << x_dir << " " << y_dir << " " << ret << std::endl;
	return ret;
}

void Map::print_map()
{
	for (int v = 0; v < 100; ++v)
	{
		std::cout << "\n Adjacency list of vertex " << v << "\n head ";
		for (auto x : adj[v])
			std::cout << "-> " << x;
		printf("\n");
	}
}

float Map::cost(int vert, int nb_vert)
{
	int diff = nb_vert - vert;
	if (diff == -10 || diff == 10 || diff == -1 || diff == 1) // up || down || left || right
		return 1.f;
	if (diff == -11 || diff == 11 || diff == -9 || diff == 9) // tl || br || tr || bl
		return 1.41f;
	return INFINITY; // not neighbor
}

float Map::heuristic(int vert1, int vert2)
{
	return distance(get_pos(vert1), get_pos(vert2)); // use the euclidean dist 
}

std::vector<int> Map::get_adj(int vert)
{
	return adj[vert];
}

// methods
void Map::init()
{
	for (int i = 0; i < 100; i++)
	{
		switch (get_VPos(i))
		{
		case VPos::TL:
			add_edge_1d(i, i + 1); // right
			add_edge_1d(i, i + 10); // down
			add_edge_1d(i, i + 11); // rd
			break;
		case VPos::TR:
			add_edge_1d(i, i - 1); // left
			add_edge_1d(i, i + 10); // down
			add_edge_1d(i, i + 9); // ld
			break;
		case VPos::BL:
			add_edge_1d(i, i + 1); // right
			add_edge_1d(i, i - 10); // up
			add_edge_1d(i, i - 9); // ru
			break;
		case VPos::BR:
			add_edge_1d(i, i - 1); // left
			add_edge_1d(i, i - 10); // up
			add_edge_1d(i, i - 11); // lu
			break;
		case VPos::Top:
			add_edge_1d(i, i - 1); // left
			add_edge_1d(i, i + 1); // right
			add_edge_1d(i, i + 10); // down
			add_edge_1d(i, i + 9); // ld
			add_edge_1d(i, i + 11); // rd
			break;
		case VPos::Bottom:
			add_edge_1d(i, i - 1); // left
			add_edge_1d(i, i + 1); // right
			add_edge_1d(i, i - 10); // up
			add_edge_1d(i, i - 11); // lu
			add_edge_1d(i, i - 9); // ru
			break;
		case VPos::Left:
			add_edge_1d(i, i + 1); // right
			add_edge_1d(i, i - 10); // up
			add_edge_1d(i, i + 10); // down
			add_edge_1d(i, i - 9); // ru
			add_edge_1d(i, i + 11); // rd
			break;
		case VPos::Right:
			add_edge_1d(i, i - 1); // left
			add_edge_1d(i, i - 10); // up
			add_edge_1d(i, i + 10); // down
			add_edge_1d(i, i - 11); // lu
			add_edge_1d(i, i + 9); // ld
			break;
		case VPos::Inner:
			add_edge_1d(i, i - 1); // left
			add_edge_1d(i, i + 1); // right
			add_edge_1d(i, i - 10); // up
			add_edge_1d(i, i + 10); // down
			add_edge_1d(i, i - 11); // lu
			add_edge_1d(i, i - 9); // ru
			add_edge_1d(i, i + 9); // ld
			add_edge_1d(i, i + 11); // rd
			break;
		defalut:
			std::cout << "Map init failed, potential out of bound error." << std::endl;
		}
	}
}

void Map::remove_given_vert(std::vector<int> vert_vec)
{
	for (int i : vert_vec)
	{
		switch (get_VPos(i))
		{
		case VPos::TL:
			remove_edge_2d(i, i + 1); // right
			remove_edge_2d(i, i + 10); // down
			remove_edge_2d(i, i + 11); // rd
			break;
		case VPos::TR:
			remove_edge_2d(i, i - 1); // left
			remove_edge_2d(i, i + 10); // down
			remove_edge_2d(i, i + 9); // ld
			break;
		case VPos::BL:
			remove_edge_2d(i, i + 1); // right
			remove_edge_2d(i, i - 10); // up
			remove_edge_2d(i, i - 9); // ru
			break;
		case VPos::BR:
			remove_edge_2d(i, i - 1); // left
			remove_edge_2d(i, i - 10); // up
			remove_edge_2d(i, i - 11); // lu
			break;
		case VPos::Top:
			remove_edge_2d(i, i - 1); // left
			remove_edge_2d(i, i + 1); // right
			remove_edge_2d(i, i + 10); // down
			remove_edge_2d(i, i + 9); // ld
			remove_edge_2d(i, i + 11); // rd
			break;
		case VPos::Bottom:
			remove_edge_2d(i, i - 1); // left
			remove_edge_2d(i, i + 1); // right
			remove_edge_2d(i, i - 10); // up
			remove_edge_2d(i, i - 11); // lu
			remove_edge_2d(i, i - 9); // ru
			break;
		case VPos::Left:
			remove_edge_2d(i, i + 1); // right
			remove_edge_2d(i, i - 10); // up
			remove_edge_2d(i, i + 10); // down
			remove_edge_2d(i, i - 9); // ru
			remove_edge_2d(i, i + 11); // rd
			break;
		case VPos::Right:
			remove_edge_2d(i, i - 1); // left
			remove_edge_2d(i, i - 10); // up
			remove_edge_2d(i, i + 10); // down
			remove_edge_2d(i, i - 11); // lu
			remove_edge_2d(i, i + 9); // ld
			break;
		case VPos::Inner:
			remove_edge_2d(i, i - 1); // left
			remove_edge_2d(i, i + 1); // right
			remove_edge_2d(i, i - 10); // up
			remove_edge_2d(i, i + 10); // down
			remove_edge_2d(i, i - 11); // lu
			remove_edge_2d(i, i - 9); // ru
			remove_edge_2d(i, i + 9); // ld
			remove_edge_2d(i, i + 11); // rd
			break;
		defalut:
			std::cout << "Remove vert failed, potential out of bound error." << std::endl;
		}
	}
}

void Map::remove_island()
{
	if (ECS::registry<Island>.entities.size() > 0) // ensure the there's island in the loaded level
	{
		auto& motion = ECS::registry<Motion>;
		for (auto& island : ECS::registry<Island>.entities)
		{
			auto island_pos = motion.get(island).position;
			auto island_scale = abs(motion.get(island).scale);
			// std::cout << "island pos " << island_pos.x << ' ' << island_pos.y << " island scale " << island_scale.x << ' ' << island_scale.y << std::endl;
			// find the 4 corner vertices
			int tl = get_vert(island_pos + vec2{ -0.5f, -0.5f } * island_scale);
			int tr = get_vert(island_pos + vec2{ 0.5f, -0.5f } * island_scale);
			int bl = get_vert(island_pos + vec2{ -0.5f, 0.5f } * island_scale);
			int br = get_vert(island_pos + vec2{ 0.5f, 0.5f } * island_scale);
			// std::cout << "island corner vertices " << tl << ' ' << tr << ' ' << bl << ' ' << br << std::endl;
			
			int del_x = tr - tl;
			int del_y = bl - tl;
			for (int y = 0; y <= del_y; y = y + 10)
			{
				for (int x = 0; x <= del_x; x++)
				{
					int r_vert = tl + y + x;
					// std::cout << "removed vertex is " << r_vert << std::endl;
					removed_vert.push_back(r_vert);
				}
			}
		}
		remove_given_vert(removed_vert);
		// print_map();
	}
	
}

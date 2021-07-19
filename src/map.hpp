//
// Created by yichen on 2021-03-10.
//

#pragma once
#include "tiny_ecs.hpp"
#include "common.hpp"

#include <vector>
#include <map>
#include <world.hpp>

/*
The vertex is an int, the ordering is from left to right, from top to bottom, along with the x and y axis, formed a 10 x 10, 8 directions grid system.
0 1 2 ... 9
10 ..... 19 
. ....... .
. ....... .
90 ..... 99
*/

enum class VPos {
	TL, // top left corner, 3 edges max
	TR, // top right corner, 3 edges max
	BL, // bottom left corner, 3 edges max
	BR, // bottom right corner, 3 edges max
	Top, // top edge, 5 edges max
	Bottom, // bottom edge, 5 edges max
	Left, // left edge, 5 edges max
	Right, // right edge, 5 edges max
	Inner, // inner vertices, 8 edges max
	Invalid
};

struct Map
{
	Map(vec2 win_size, WorldSystem& world);

public:
	void remove_given_vert(std::vector<int> vert_vec); // given multiple vertices, removes these vertices and all the edges from the graph
	vec2 get_pos(int vert); // given a vertex, return the position in the game world
	int get_vert(vec2 pos); // given a game world position, return the vertex
	std::vector<int> get_adj(int vert); // given vertex returns neighbors vector
	void print_map(); // print the whole adj list
	float cost(int vert, int nb_vert); // return the cost of travel from cur vert to neighbor vert, ret 1 if next to, ret 1.41 if diag, ret Infinity if not neighbor
	float heuristic(int vert1, int vert2);

public:
	std::vector<int> adj[100]; // can also use unordered map: std::unordered_map< int, std::vector<int>> edges;
	std::vector<int> removed_vert; // inaccessible verts

private:
	void init();
	void add_edge_1d(int indx1, int indx2);
	void remove_edge_2d(int indx1, int indx2);
	VPos get_VPos(int vert);
	void remove_island();

private:
	vec2 window_size;
};

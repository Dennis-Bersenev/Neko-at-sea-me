// Header
#include "convexhull.hpp"
#include "tiny_ecs.hpp"
#include "render.hpp"

#include <cmath>
#include <iostream>

#include "render_components.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 2
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
namespace ConvexHull 
{
    vec2 getvertexpoints(ColoredVertex vertex, Transform transform) {

        mat4 mat = transform.mat;
        vec4 position = vec4{ vertex.position.x,vertex.position.y, 0 ,1};
        vec4 transformed_pos = 
       vec4(mat[0][0] * position.x + mat[1][0] * position.y + mat[2][0]* position.z + mat[3][0] * 1.f,
            mat[0][1] * position.x + mat[1][1] * position.y + mat[2][1] * position.z + mat[3][1] * 1.f,
            mat[0][2] * position.x + mat[1][2] * position.y + mat[2][2] * position.z + mat[3][2] * 1.f,
            mat[0][3] * position.x + mat[1][3] * position.y + mat[2][3] * position.z + mat[3][3] * 1.f);

        return vec2(transformed_pos.x, transformed_pos.y);

    }
    //https://www.geeksforgeeks.org/convex-hull-set-1-jarviss-algorithm-or-wrapping/
    int orientation(vec2 p, vec2 q, vec2 r)
    {
        int val = (q.y - p.y) * (r.x - q.x) -
            (q.x - p.x) * (r.y - q.y);

        if (val == 0) return 0;  // colinear 
        return (val > 0) ? 1 : 2; // clock or counterclock wise 
    }
    //https://www.geeksforgeeks.org/convex-hull-set-1-jarviss-algorithm-or-wrapping/
	std::vector<vec2> getConvexhull(std::vector<ColoredVertex> vertices, int n, Motion& motion) {

     Transform transform;
	 transform.translate(vec2(motion.position.x - window_dims.x/2.f, window_dims.y/2.f - motion.position.y));
	 transform.rotate(motion.angle);
	 transform.scale(motion.scale);
     std::vector<vec2> result_hull;

        if (n < 3) {
            for (int x = 0; x < n; x++) {
                vec2 point = vertices[x].position;
                result_hull.push_back(point);
            }
        }

        // Find the leftmost point 
        int index = 0;
        for (int i = 1; i < n; i++) {
            vec2 first_vertex = getvertexpoints(vertices[index], transform);
            vec2 next_vertex = getvertexpoints(vertices[i], transform);
            if (next_vertex.x < first_vertex.x)
                index = i;
        }

        int p = index;
        int q;
        do
        {
            result_hull.push_back(vertices[p].position);
            q = (p + 1) % n;
            for (int i = 0; i < n; i++)
            {
                // If i is more counterclockwise than current q, then 
                // update q 
                vec2 p_vertex = getvertexpoints(vertices[p], transform);
                vec2 i_vertex = getvertexpoints(vertices[i], transform);
                vec2 q_vertex = getvertexpoints(vertices[q], transform);
                if (orientation(p_vertex, i_vertex, q_vertex) == 2)
                    q = i;
            }

            // Now q is the most counterclockwise with respect to p 
            // Set p as q for next iteration, so that q is added to 
            // result 'hull' 
            p = q;

        } while (p != index);  // While we don't come to first point 

        return result_hull;
	}

    void set_window(vec2 window) {
        window_dims = window;
    }

    vec2 window_dims = { 0,0 };
}


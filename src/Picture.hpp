// #include "vec.hpp"
#include <cmath>
#include "physics/physics.hpp"
#include "engine/common/color_utils.hpp"

inline float heart(float a, float b)
{
    float x=a/300 -0.5f, y=b/300 -0.5f;
    float temp = x * x + y * y - 1;

    // return temp * temp * temp - x * x * y * y * y < 0 ? 1 : 0;
    return x*x+y*y;
}

inline void drawheart(PhysicSolver &solver)
{
    for (auto id = 0; id < solver.objects.size(); id++) {
        float temp=heart(solver.objects[id].position.x, solver.objects[id].position.y);
        solver.objects[id].color = temp>1?ColorUtils::createColor(150, 5, 5): ColorUtils::createColor(50, 50, 50);
    }
}
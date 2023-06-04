#include <cstddef>
#include <iostream>
#include <stdint.h>
#include <unordered_map>

#include "SFML/Graphics/Color.hpp"
#include "engine/common/color_utils.hpp"
#include "engine/common/number_generator.hpp"
#include "engine/window_context_handler.hpp"

#include "physics/physics.hpp"
#include "renderer/renderer.hpp"
#include "thread_pool/thread_pool.hpp"

#include "stb_image.h"
// #include "Picture.hpp"

float heart(float a, float b)
{
    float x = a / 150 - 0.5f, y = b / 150 - 0.5f;
    float temp = x * x + y * y - 1;

    // return temp * temp * temp - x * x * y * y * y < 0 ? 1 : 0;
    // std::cout<<x*x+y*y<<std::endl;
    return x * x + y * y;
}

int width, height, channels;
unsigned char* image_data = stbi_load("res/head.png", &width, &height, &channels, 0);

sf::Color color(int x, int y)
{
    int offset = (y * width + x) * channels;
    unsigned char r = image_data[offset];
    unsigned char g = image_data[offset + 1];
    unsigned char b = image_data[offset + 2];
    return sf::Color(r, g, b);
}

int main()
{
    const uint32_t window_width = 1920;
    const uint32_t window_height = 1080;
    WindowContextHandler app("Verlet-MultiThread", sf::Vector2u(window_width, window_height), sf::Style::Default);
    RenderContext& render_context = app.getRenderContext();
    // Initialize solver and renderer

    /* The number of threads should be adjusted so that it divides the world size or some of the grid's cells
       won't be processed */
    tp::ThreadPool thread_pool(1);

    // 用来控制窗口内部实际使用到的区域
    const IVec2 world_size { 256, 140 };
    PhysicSolver solver { world_size, thread_pool };
    Renderer renderer(solver, thread_pool);

    const float margin = 120.0f;
    const auto zoom = static_cast<float>(window_height - margin) / static_cast<float>(world_size.y);
    render_context.setZoom(zoom);
    // 将键盘和鼠标输入的焦点设置到该窗口上
    render_context.setFocus({ world_size.x * 0.5f, world_size.y * 0.5f });

    bool emit = true;
    // addKeyPressedCallback接受的第二个参数 EventCallback callback 要求这个函数必须有sf::Event 这个类型的输入参数，所以需要sfev::CstEv
    app.getEventManager().addKeyPressedCallback(sf::Keyboard::Space, [&](sfev::CstEv) {
        emit = !emit;
    });

    int mode = 0;
    // 控制场景是否暂停
    //  bool stop = false;
    app.getEventManager().addKeyPressedCallback(sf::Keyboard::Tab, [&](sfev::CstEv) {
        // stop = !stop;
        mode = 0;
    });

    //  bool restart=false;
    app.getEventManager().addKeyPressedCallback(sf::Keyboard::R, [&](sfev::CstEv) {
        // restart = !restart;
        mode = 1;
    });
    app.getEventManager().addKeyPressedCallback(sf::Keyboard::T, [&](sfev::CstEv) {
        // restart = !restart;
        mode = 2;
    });

    constexpr uint32_t fps_cap = 60;
    int32_t target_fps = fps_cap;
    app.getEventManager().addKeyPressedCallback(sf::Keyboard::S, [&](sfev::CstEv) {
        target_fps = target_fps ? 0 : fps_cap;
        app.setFramerateLimit(target_fps);
    });

    // 使用hashmap保存 id:color
    std::unordered_map<uint64_t, sf::Color> id_color;

    // Main loop
    const float dt = 1.0f / static_cast<float>(fps_cap);
    while (app.run()) {
        if (mode == 0) {
            if (solver.objects.size() < 38000 && emit) {
                for (uint32_t i { 80 }; i--;) {
                    const auto id = solver.createObject({ 10.0f + 1.1f * i, 20.0f + 1.1f * i });
                    // 控制发射方向
                    solver.objects[id].last_position.x -= 0.2f;
                    solver.objects[id].last_position.y += 0.2f;

                    solver.objects[id].color = ColorUtils::getRainbow(id * 0.0001f);
                }
            }
            solver.update(dt);
            render_context.clear();
            renderer.render(render_context);
            render_context.display();
        }
        if (mode == 1) {
            for (auto id = 0; id < solver.objects.size(); id++) {
                // if(solver.objects[id].position.x>150.0f){
                //     solver.objects[id].color=ColorUtils::createColor(50,50,50);
                // }
                // float temp = heart(solver.objects[id].position.x, solver.objects[id].position.y);
                // solver.objects[id].color = temp>1?ColorUtils::createColor(150, 5, 5): ColorUtils::createColor(50, 50, 50);
                // sf::Color color = temp > 1 ? ColorUtils::createColor(193, 44, 31) : ColorUtils::createColor(119, 150, 73);
                
                id_color.insert({ id, color(solver.objects[id].position.x, solver.objects[id].position.y) });
            }
            solver.objects.clear();
            // solver.update(dt);
            // drawheart(solver);
            render_context.clear();
            renderer.render(render_context);
            render_context.display();
        };
        if (mode == 2) {
            if (solver.objects.size() < 38000 && emit) {
                for (uint32_t i { 80 }; i--;) {
                    const auto id = solver.createObject({ 10.0f + 1.1f * i, 20.0f + 1.1f * i });
                    // 控制发射方向
                    solver.objects[id].last_position.x -= 0.2f;
                    solver.objects[id].last_position.y += 0.2f;

                    // solver.objects[id].color = ColorUtils::getRainbow(id * 0.0001f);
                    solver.objects[id].color = id_color[id];
                }
            }
            solver.update(dt);
            render_context.clear();
            renderer.render(render_context);
            render_context.display();
        }
    }
    return 0;
}

// 思路
// 由于场景最终有8000个粒子
// 所以可以等到某一个相对稳态的时刻
// 对场景内所有粒子的id进行遍历
// 获得单个粒子的位置(x,y)，随后查询对应的图片中(x,y)位置的颜色是什么然后保存到一个表里
// 在所有粒子遍历完成之后清除所有粒子
// 重新发射所有粒子，这一遍就按照id查表给粒子附上颜色
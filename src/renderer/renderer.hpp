#pragma once
#include "engine/window_context_handler.hpp"
#include "physics/physics.hpp"
#include <SFML/Graphics.hpp>

struct Renderer {
    PhysicSolver& solver;
    // 用来控制窗口内部的那个实际使用到的区域
    sf::VertexArray world_va;
    sf::VertexArray objects_va;
    sf::Texture object_texture;

    tp::ThreadPool& thread_pool;

    explicit Renderer(PhysicSolver& solver_, tp::ThreadPool& tp);

    void render(RenderContext& context);
    
    // 对 world_va 进行设置，包括实际使用到的区域大小，此区域的背景颜色
    void initializeWorldVA();

    void updateParticlesVA();

    // 这个函数似乎是用来运行时显示信息的，后来被注释掉没用了
    void renderHUD(RenderContext& context);
};

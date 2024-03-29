#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>

// TODO: setup imgui
// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

#include "include/types.h"
#include "include/arena.h"
#include "include/audio.h"
#include "include/constants.h"
#include "include/renderer.h"
#include "include/opengl_renderer.h"
#include "include/game.h"
#include "include/game_init.h"
#include "include/game_nodes.h"
#include "include/input.h"
#include "include/node.h"
#include "include/time.h"

struct Window {
    int width;
    int height;
    GLFWwindow* handle;
};

const float game_width = GAME_WIDTH;
const float game_height = GAME_HEIGHT;

Window global_window;


void resize_cb(GLFWwindow* window, int width, int height) {
    global_window.width = width;
    global_window.height = height;
}

void init_window() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

#ifdef FULLSCREEN
    global_window.width = 1920;
    global_window.height = 1080;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
#else
    global_window.width = GAME_WIDTH;
    global_window.height = GAME_HEIGHT;
    GLFWmonitor* monitor = NULL;
#endif
    global_window.handle = glfwCreateWindow(global_window.width,
                                            global_window.height,
                                            "Game",
                                            monitor,
                                            NULL);
    if (!global_window.handle) {
        printf("Failed to create window\n");
        exit(1);
    }

    glfwSetFramebufferSizeCallback(global_window.handle,
                                   resize_cb);
    glfwMakeContextCurrent(global_window.handle);
}

void init_ui(Level* level, Node* scene_root, Font* font) {
    {
        auto* ui_wrapper = new CanvasNode(level);
        auto* score = new ScoreNode(level, font);
        score->position = v2(0, 40-GAME_HEIGHT);
        ui_wrapper->AddChild(score);
        auto* timer = new CountDownNode(level, font);
        timer->position = v2(0, -10);
        ui_wrapper->AddChild(timer);
        scene_root->AddChild(ui_wrapper);
    }
}

i32 main() {
#ifdef DEBUG
    printf("Running Debug build\n");
#endif
    init_window();
    init_pool(&pool);
    opengl_Init(game_width, game_height);

    Arena arena;
    init_arena(&arena, &pool);

    Arena game_arena;
    init_arena(&game_arena, &pool);

    u32 vertex_count = 20000;
    Vertex* vertex_buffer = (Vertex*) push_size(&arena, sizeof(Vertex) * vertex_count);
    u32 index_count = 20000;
    u32* index_buffer = (u32*) push_size(&arena, sizeof(u32) * index_count);
    u32 mask_vertex_count = 2000;
    MaskVertex* mask_vertices = (MaskVertex*) push_size(&arena, sizeof(MaskVertex) * mask_vertex_count);
    u32 cmd_len = 40000;
    u8* cmd_memory = (u8*) push_size(&arena, cmd_len);

    audio_Setup();
    AudioHandle song = audio_Load("audio/CantinaBand60.wav");
    // audio_Play(&song);
    // audio_UnloadAll();

    TextureHandle white;
    TextureLoadOp load_op = renderer_TextureLoadOp(&white, "assets/white.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    TextureHandle box_texture;
    load_op = renderer_TextureLoadOp(&box_texture, "assets/props/crate.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    TextureHandle image;
    load_op = renderer_TextureLoadOp(&image, "assets/image.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    TextureHandle animated_image;
    load_op = renderer_TextureLoadOp(&animated_image, "assets/player/test-animation.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    TextureHandle wall_texture;
    load_op = renderer_TextureLoadOp(&wall_texture, "assets/tilesets/wall_base.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    TextureHandle floor_texture;
    load_op = renderer_TextureLoadOp(&floor_texture, "assets/tilesets/ground.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    
    TextureHandle glass_wall_texture;
    load_op = renderer_TextureLoadOp(&glass_wall_texture, "assets/tilesets/glass_base.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    Font font;
    opengl_SetupFont(&font, "assets/Dosis.ttf", 38);

    u32 stage = 0;
    Level level;
    auto* scene_root = new Node(&level);

    StageAttributes attributes;
    game_Init(&level, stage, &game_arena, scene_root, &attributes);

    Mat4 projection = glm::ortho(
        -game_width / 2,
        game_width / 2,
        -game_height / 2,
        game_height / 2,
        .1f,
        1000.0f
    );

    Mat4 view = glm::lookAt(
        glm::vec3(0, 0, 100),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

    Mat4 proj = projection * view;

    float lastFrame = 0.0f;

    // Load UI
    init_ui(&level, scene_root, &font);

    // Begin render loop
    while (!glfwWindowShouldClose(global_window.handle)) {

        // Check for window close request
        if (glfwGetKey(global_window.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(global_window.handle, true);
        }

        if (level.resetStage) {
            if (level.ffGoNext) {
                if (stage != LEVEL_COUNT - 1) stage++;
                else stage = 0;
            }
            level.static_collider = 0;
            level.collider_count = 0;
            delete scene_root;
            scene_root = new Node(&level);
            dispose(&game_arena);
            init_arena(&game_arena,&pool);
            game_Init(&level, stage, &game_arena,scene_root, &attributes);
            level.resetStage = false;
            level.ffGoNext = false;

            // Load UI
            init_ui(&level, scene_root, &font);
        }


        // Calculate delta time
        float time = glfwGetTime();
        time_deltatime = time - lastFrame;
        lastFrame = time;

        // Update input keys
        input_UpdateAll(global_window.handle);

#ifdef DEBUG
        if (input_KeyN.down) {
            level.resetStage = true;
            level.ffGoNext = true;
        }
#endif
        // Construct command buffer for visual/rendering operations
        CommandBuffer cmd = renderer_Buffer(cmd_len, cmd_memory,
                                            vertex_count, vertex_buffer,
                                            index_count, index_buffer,
                                            mask_vertex_count, mask_vertices,
                                            proj, global_window.width, global_window.height,
                                            white);

        // Request to clear at the beginning
        renderer_PushClear(&cmd, v3(0.1647058824, 0.1176470588, 0.137254902));



        // render game grid
        game_RenderGrid(&cmd, &level, wall_texture, floor_texture, glass_wall_texture);
        game_RenderBoxes(&cmd, &level, box_texture);

        // Rune node tree lifecycle
        renderer_PushBase(&cmd, level.camera.center);
        scene_root->TryPreUpdate();
        scene_root->TryUpdate();

#ifdef DEBUG
        renderer_PushBase(&cmd, level.camera.center);
        for (u32 i = 0; i < level.collider_count; ++i) {
            AABB aabb = level.collider[i].aabb;
            renderer_PushOutline(&cmd, aabb.position,
                v2(aabb.position.x + aabb.size.x, aabb.position.y + aabb.size.y),
                40, 1, v3(0, 1, 0));
        }

        //renderer_PushBase(&cmd, v2(0));
        //V2 ray = v2(sin(0.3 * time), cos(0.3 * time));
        //float t = game_Raycast(&level, level.camera.center, ray);
        //renderer_PushLine(&cmd, v2(0),
        //                  v2(ray.x * t, ray.y * t), 30, 1, v3(0, 0, 1));

#endif

        renderer_PushBase(&cmd, level.camera.center);
        scene_root->TryRender(&cmd);

        // Queue post processing
        renderer_PushPostprocessPass(&cmd);

        // Execute final rendering operations
        opengl_RenderCommands(&cmd);

        glfwSwapBuffers(global_window.handle);
        glfwPollEvents();
    }

    delete scene_root; // cleanup scene root. This is probably unnecessary, but currently in charge for testing cleanups

    glfwTerminate();
    return 0;
}

#ifdef WINDOWS

i32 WinMain() {
    return main();
}

#endif

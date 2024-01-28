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

const float game_width = 960;
const float game_height = 540;

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

#ifdef FULLSCREEN
    global_window.width = 1920;
    global_window.height = 1080;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
#else
    global_window.width = 960;
    global_window.height = 540;
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
    init_arena(&arena, &pool);

    u32 vertex_count = 20000;
    Vertex* vertex_buffer = (Vertex*) push_size(&arena, sizeof(Vertex) * vertex_count);
    u32 index_count = 20000;
    u32* index_buffer = (u32*) push_size(&arena, sizeof(u32) * index_count);
    u32 mask_vertex_count = 200;
    MaskVertex* mask_vertices = (MaskVertex*) push_size(&arena, sizeof(MaskVertex) * mask_vertex_count);
    u32 cmd_len = 20000;
    u8* cmd_memory = (u8*) push_size(&arena, cmd_len);

    audio_Setup();
    AudioHandle song = audio_Load("audio/CantinaBand60.wav");
    // audio_Play(&song);
    // audio_UnloadAll();

    TextureHandle white;
    TextureLoadOp load_op = renderer_TextureLoadOp(&white, "assets/white.png");
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

    Font font;
    opengl_SetupFont(&font, "assets/Dosis.ttf", 48);

    // TODO: Setup imgui
    // Setup Dear ImGui context
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();

    // Setup Platform/Renderer backends
    // ImGui_ImplGlfw_InitForOpenGL(global_window.handle, true);
    // ImGui_ImplOpenGL3_Init();

    u32 stage = 0;
    Level level;
    auto* scene_root = new Node(&level);

    game_Init(&level, stage, &arena, scene_root);

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

    {
        auto* testNode = new RotatingNode(&level);
        auto* sub_tex = new TextureNode(&level, image, 32, 32);
        sub_tex->position = v2(20);
        testNode->AddChild(sub_tex);
        scene_root->AddChild(testNode);
    }
    // Begin render loop
    while (!glfwWindowShouldClose(global_window.handle)) {

        // Check for window close request
        if (glfwGetKey(global_window.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(global_window.handle, true);
        }
        

        // Calculate delta time
        float time = glfwGetTime();
        time_deltatime = time - lastFrame;
        lastFrame = time;

        // Update input keys
        input_UpdateAll(global_window.handle);

#ifdef DEBUG
        if (input_KeyN.down) {
            stage++;
            delete scene_root;
            scene_root = new Node(&level);
            dispose(&game_arena);
            init_arena(&game_arena,&pool);
            game_Init(&level, stage, &game_arena,scene_root);
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

        // renderer_PushSprite(&cmd,
        //                     v2(-30), v2(-30, 30), v2(30), v2(30, -30),
        //                     v2(0), v2(1),
        //                     v3(0, 1, 0), &white);
        // renderer_PushString(&cmd, &font, "Font rendering, gg!",
        //                     v2(sin(time) * 150, cos(time) * 150));

        // render game grid
        game_RenderWalls(&cmd, &level, white, wall_texture);
        game_RenderBoxes(&cmd, &level, white, wall_texture);


        // Rune node tree lifecycle
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

        renderer_PushBase(&cmd, v2(0));
        V2 ray = v2(sin(0.3 * time), cos(0.3 * time));
        float t = game_Raycast(&level, level.camera.center, ray);
        renderer_PushLine(&cmd, v2(0),
                          v2(ray.x * t, ray.y * t), 30, 1, v3(0, 0, 1));

        MaskVertex test_mask[3] = {
            v2(0, 0),
            v2(100, 100),
            v2(100, 0)
        };
        renderer_PushMaskOp(&cmd, 3, test_mask);
#endif

        renderer_PushBase(&cmd, level.camera.center);
        scene_root->TryRender(&cmd);

        // Queue post processing
        renderer_PushPostprocessPass(&cmd);

        // Execute final rendering operations
        opengl_RenderCommands(&cmd);
        // TODO: Setup imgui
        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();
        // ImGui::ShowDemoWindow();

        // TODO: Setup
        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

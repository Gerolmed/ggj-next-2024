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
#include "include/game_nodes.h"
#include "include/node.h"

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

    u32 vertex_count = 10000;
    Vertex* vertex_buffer = (Vertex *) push_size(&arena, sizeof(Vertex) * vertex_count);
    u32 index_count = 10000;
    u32* index_buffer = (u32 *) push_size(&arena, sizeof(u32) * index_count);
    u32 cmd_len = 10000;
    u8* cmd_memory = (u8 *) push_size(&arena, cmd_len);

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

    Level level;
    game_Init(&level, 0, &arena);

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

    float delta = 0.0f;
    float lastFrame = 0.0f;

    const auto scene_root = new Node(&level);
    scene_root->position = v2(1);
    scene_root->rotation = degreesToRadians(180);
    scene_root->Start();

    {
        const auto node2 = new TestNode(&level);
        node2->position = v2(2);
        scene_root->AddChild(node2);

        const auto node3 = new TextureNode(&level, &image, 100 , 100);
        node3->position = v2(1);
        scene_root->AddChild(node3);
    }

    while (!glfwWindowShouldClose(global_window.handle)) {
        if (glfwGetKey(global_window.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(global_window.handle, true);
        }

        float time = glfwGetTime();
        delta = time - lastFrame;
        lastFrame = time;

        CommandBuffer cmd = renderer_Buffer(cmd_len, cmd_memory,
                                            vertex_count, vertex_buffer,
                                            index_count, index_buffer,
                                            proj, global_window.width, global_window.height);

        renderer_PushClear(&cmd, v3(0.1, 0.1, 0.2));

        // renderer_PushSprite(&cmd,
        //                     v2(-30), v2(-30, 30), v2(30), v2(30, -30),
        //                     v2(0), v2(1),
        //                     v3(0, 1, 0), &white);
        // renderer_PushString(&cmd, &font, "Font rendering, gg!",
        //                     v2(sin(time) * 150, cos(time) * 150));


        scene_root->PreUpdate();
        scene_root->Update();
        scene_root->Render(&cmd);

        game_RenderGrid(&cmd, &level, &white);

        renderer_PushPostprocessPass(&cmd);

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

    delete scene_root;

    glfwTerminate();
    return 0;
}

#ifdef WINDOWS
i32 WinMain() {
    return main();
}
#endif

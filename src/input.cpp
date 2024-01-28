#include "include/input.h"

#include "GLFW/glfw3.h"


KeyInput input_KeyW = KeyInput(GLFW_KEY_W);
KeyInput input_KeyA = KeyInput(GLFW_KEY_A);
KeyInput input_KeyS = KeyInput(GLFW_KEY_S);
KeyInput input_KeyD = KeyInput(GLFW_KEY_D);
KeyInput input_KeyN = KeyInput(GLFW_KEY_N);
KeyInput input_KeySpace = KeyInput(GLFW_KEY_SPACE);

void input_UpdateAll(GLFWwindow* window) {
    input_KeyW.Update(window);
    input_KeyA.Update(window);
    input_KeyS.Update(window);
    input_KeyD.Update(window);
    input_KeyN.Update(window);
    input_KeySpace.Update(window);
}

KeyInput::KeyInput(const GLint key): input_key(key) {

}


void KeyInput::Update(GLFWwindow* window) {
    is_pressed = glfwGetKey(window, input_key) == GLFW_PRESS;

    down = is_pressed && !was_pressed;
    up = !is_pressed && was_pressed;
    was_pressed = is_pressed;
}

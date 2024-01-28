#ifndef INPUT_H
#define INPUT_H
#include <GLFW/glfw3.h>
class KeyInput;

extern KeyInput input_KeyW;
extern KeyInput input_KeyA;
extern KeyInput input_KeyS;
extern KeyInput input_KeyD;
extern KeyInput input_KeyN;
extern KeyInput input_KeySpace;



void input_UpdateAll(GLFWwindow* window);

class KeyInput {
public:
    bool down = false;
    bool up = false;
    bool is_pressed = false;
    explicit KeyInput(GLint key);

    void Update(GLFWwindow* window);
private:
    GLint input_key;
    bool was_pressed = false;

};
#endif //INPUT_H

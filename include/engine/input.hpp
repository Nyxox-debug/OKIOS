#include "GLFW/glfw3.h"

class Input {
public:
    static bool IsKeyPressed(GLFWwindow* window, int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }
};

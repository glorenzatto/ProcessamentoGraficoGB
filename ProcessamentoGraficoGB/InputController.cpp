#include "InputController.h"

InputController::InputController(GLFWwindow* window)
    : window(window), 
    rightPressed(false), 
    leftPressed(false), 
    upPressed(false), 
    downPressed(false), 
    shiftPressed(false), 
    ctrlPressed(false), 
    spacePressed(false),
    rightWasPressed(false), 
    leftWasPressed(false), 
    upWasPressed(false), 
    downWasPressed(false), 
    shiftWasPressed(false), 
    ctrlWasPressed(false), 
    spaceWasPressed(false) {}

void InputController::update() {
    rightWasPressed = rightPressed;
    leftWasPressed = leftPressed;
    upWasPressed = upPressed;
    downWasPressed = downPressed;
    shiftWasPressed = shiftPressed;
    ctrlWasPressed = ctrlPressed;
    spaceWasPressed = spacePressed;

    rightPressed = (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT));
    leftPressed = (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT));
    upPressed = (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP));
    downPressed = (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN));
    shiftPressed = (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_SHIFT));
    ctrlPressed = (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL));
    spacePressed = (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE));
}

bool InputController::wasRightPressed() const {
    return rightPressed && !rightWasPressed;
}

bool InputController::wasLeftPressed() const {
    return leftPressed && !leftWasPressed;
}

bool InputController::wasUpPressed() const {
    return upPressed && !upWasPressed;
}

bool InputController::wasDownPressed() const {
    return downPressed && !downWasPressed;
}

bool InputController::wasShiftPressed() const {
    return shiftPressed && !shiftWasPressed;
}

bool InputController::wasCtrlPressed() const {
    return ctrlPressed && !ctrlWasPressed;
}

bool InputController::wasSpacePressed() const {
    return spacePressed && !spaceWasPressed;
}

bool InputController::isShiftPressed() const {
    return shiftPressed;
}

bool InputController::isCtrlPressed() const {
    return ctrlPressed;
}

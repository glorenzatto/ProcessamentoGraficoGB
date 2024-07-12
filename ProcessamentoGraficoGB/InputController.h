#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <GLFW/glfw3.h>

class InputController {
public:
    InputController(GLFWwindow* window);

    void update();

    bool wasRightPressed() const;
    bool wasLeftPressed() const;
    bool wasUpPressed() const;
    bool wasDownPressed() const;
    bool wasShiftPressed() const;
    bool wasCtrlPressed() const;
    bool wasSpacePressed() const;

    bool isShiftPressed() const;
    bool isCtrlPressed() const;

private:
    GLFWwindow* window;
    bool rightPressed;
    bool leftPressed;
    bool upPressed;
    bool downPressed;
    bool shiftPressed;
    bool ctrlPressed;
    bool spacePressed;

    bool rightWasPressed;
    bool leftWasPressed;
    bool upWasPressed;
    bool downWasPressed;
    bool shiftWasPressed;
    bool ctrlWasPressed;
    bool spaceWasPressed;
};

#endif // INPUTCONTROLLER_H

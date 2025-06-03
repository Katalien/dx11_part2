#pragma once
#include "Dev.h"

class Input {
private:
    Engine* engine;
public:
    static const int KEYBOARD_SIZE = 256;

    BYTE
        keys[KEYBOARD_SIZE],           /* Current key states */
        oldKeys[KEYBOARD_SIZE],        /* Previous frame key states */
        keysClick[KEYBOARD_SIZE];      /* Click key flags */
    INT
        Mx, My,              /* Mouse cursor position */
        Mz,                  /* Mouse wheel state*/
        Mdx, Mdy, Mdz;       /* Delta values of mouse axes */

    Input(Engine * eng);

    void update();

    ~Input() = default;
};


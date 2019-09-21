#ifndef types_hpp
#define types_hpp

#include <iostream>
namespace mineworld2 {
    const int CELL_X = 16, CELL_Y = 16, CELL_Z = 16, CELL_V = CELL_X * CELL_Y * CELL_Z;
    enum PlayingState {
        PLAYING,
        PAUSE,
        TYPING
    };
    enum BlockFace {
        NONE,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        FRONT,
        BACK
    };
}

#endif /* types_hpp */

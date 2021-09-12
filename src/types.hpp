#ifndef types_hpp
#define types_hpp

#define MW_VERSION "MineWorld 3.0.3"
#define SCREEN_DISPLAY "MineWorld 3.0.3 holding: "

namespace mineworld {
    const int CELL_X = 16, CELL_Y = 16, CELL_Z = 16, CELL_V = CELL_X * CELL_Y * CELL_Z;
    enum PlayingState {
        PLAYING,
        PAUSE,
        TYPING
    };
    enum Face {
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

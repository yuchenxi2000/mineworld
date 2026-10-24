#ifndef terminal_hpp
#define terminal_hpp
#include <deque>
#include <memory>
#include <string>
#include "shape.hpp"
#include "types.hpp"

namespace mineworld {
    struct font_loc_t {
        int x,y;
        font_loc_t(int x, int y) : x(x), y(y) {}
    };
    font_loc_t getFontLoc(char c);
    rect2D createCharRect(const font_loc_t & location);
    class Font {
        rect2D fontset[96]{};
    public:
        GLuint textureID = 0, program = 0;
        Font() = default;
        Font(const Font &) = delete;
        Font & operator=(const Font &) = delete;
        ~Font() { release(); }
        void load();
        void release();
        rect2D getCharRect(char c) const {
            unsigned char value = static_cast<unsigned char>(c);
            return fontset[value >= 32 && value < 128 ? value-32 : 0];
        }
    };
    class Screen {
    protected:
        int width = 1, height = 1, capcity = 1;
        std::unique_ptr<glRect2DBuffer> glbuffer;
        std::string screencontent;
        Font font;
        float fontw = 16, fonth = 32;
    public:
        void init();
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        void resize();
        void putc(int position, char c);
        void print(int start, int size, const char * text);
        void clear();
        void flush();
        void show();
        void release() { glbuffer.reset(); font.release(); }
    };
    class Terminal : public Screen {
        std::deque<std::string> lines{"> "};
        bool edited = true;
    public:
        void init() { Screen::init(); edited = true; }
        void flush();
        void resize() { Screen::resize(); edited = true; }
        void inputc(unsigned int c) {
            if (c >= 32 && c < 127) { lines.back().push_back(static_cast<char>(c)); edited = true; }
        }
        void del() { if (lines.back().size() > 2) lines.back().pop_back(); edited = true; }
        void execute();
        bool executeLine(const std::string & command);
        void println(const std::string & text);
    };
    class Board : public Screen {
        std::string displayline;
        bool edited = true;
    public:
        void init() { Screen::init(); display(MW_VERSION); }
        void resize() { Screen::resize(); renderText(); }
        void renderText() {
            clear();
            print(0, static_cast<int>(displayline.size()), displayline.c_str());
            edited = true;
        }
        void display(const std::string & text) { if (displayline != text) { displayline = text; renderText(); } }
        void flush() { if (edited) { Screen::flush(); edited = false; } }
    };
    extern Terminal gterminal;
    extern Board gboard;
}
#endif

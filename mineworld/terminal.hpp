#ifndef terminal_hpp
#define terminal_hpp

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shape.hpp"
#include "util.hpp"
#include "cell.hpp"
#include "handler.hpp"
#include "block.hpp"
namespace mineworld2 {
    struct font_loc_t {
        int x, y;
        font_loc_t(int x, int y) : x(x), y(y) {}
    };
    font_loc_t getFontLoc(char c);
    
    rect2D createCharRect(const font_loc_t & fl);
    
    /*
     * 字体
     */
    class Font {
        rect2D fontset[16 * 6];
    public:
        GLuint textureID, program;
        void load();
        rect2D getCharRect(char c) {
            if (c <= 32) return fontset[0];
            else return fontset[c - 32];
        }
        ~Font() {
            glDeleteTextures(1, &textureID);
        }
    };
    
    /*
     * 将屏幕视为二维数组
     * 内容有更新时重新画一遍 (resize)
     */
    class Screen {
    protected:
        int width, height;
        int capcity;
        glRect2DBuffer * glbuffer;
        std::string screencontent;
        Font font;
        float fontw, fonth;
    public:
        void init();
        int getWidth() {
            return width;
        }
        int getHeight() {
            return height;
        }
        void resize();
        
        void putc(int pos, char c);
        void print(int start, int size, const char * str);
        void clear();
        
        void flush();
        
        void show();
    };
    
    /*
     * 按行打印信息
     * 执行指令
     */
    class Terminal : public Screen {
        std::deque<std::string> lines;
        int cursor, tail;
        
        bool edited = false;
    public:
        Terminal() {}
        void init() {
            Screen::init();
            
            lines.push_back(std::string("> "));
            edited = true;
        }
        void flush();
        void resize() {
            Screen::resize();
            edited = true;
        }
        
        void inputc(unsigned int c) { // 输入一个字符
            lines.rbegin()->push_back((char)c);
            edited = true;
        }
        
        void del() { // 删除一个字符
            if (lines.rbegin()->size() > 2)
                lines.rbegin()->pop_back();
            edited = true;
        }
        
        void execute(); // 执行指令
        
        void println(const std::string & str) { // 打印一行
            lines.push_back(str);
            std::cout << str << std::endl;
            edited = true;
        }

    };
    
    /*
     * 显示单行文字
     */
    class Board : public Screen {
        std::string displayline;
        bool edited = false;
    public:
        void init() {
            Screen::init();
            
            display(std::string("MineWorld 2.1.2"));
        }
        void resize() {
            Screen::resize();
            edited = true;
        }
        void display(const std::string & str) {
            displayline = str;
            
            clear();
            print(0, displayline.size(), displayline.c_str());
            
            edited = true;
        }
        void flush() {
            if (edited)
                Screen::flush();
        }
    };
    
    extern Terminal gterminal;
    extern Board gboard;
}

#endif /* terminal_hpp */

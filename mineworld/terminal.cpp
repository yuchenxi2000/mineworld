#include "terminal.hpp"
#include "texture.hpp"
#include <cmath>
namespace mineworld2 {
    
    font_loc_t getFontLoc(char c) { // 16 * 8
        int x = c & 0xF;
        int y = (c - 32) >> 4;
        return font_loc_t(x, y);
    }
    
    rect2D createCharRect(const font_loc_t & fl) {
        rect2D r;
        r.setuv((float)fl.x / 16.0f, (float)fl.y / 8.0f, 0.0625f, 0.125f);
        return r;
    }
    
    void Font::load() {
        textureID = create2DTexture(config.fontset.texture);
        std::string vshcode = readFromFile(config.fontset.fontvsh);
        std::string fshcode = readFromFile(config.fontset.fontfsh);
        program = compile(vshcode.c_str(), fshcode.c_str());
        for (int i = 32; i < 128; ++i) {
            fontset[i - 32] = createCharRect(getFontLoc((char)i));
        }
    }
    
    void Screen::init() {
        fontw = 16;
        fonth = 32;
        
        glbuffer = new glRect2DBuffer();
        font.load();
        width = setting.WINDOW_WIDTH / fontw;
        height = setting.WINDOW_HEIGHT / fonth;
        capcity = width * height;
        screencontent.assign(capcity, 0);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void Screen::resize() {
        width = setting.WINDOW_WIDTH / fontw;
        height = setting.WINDOW_HEIGHT / fonth;
        capcity = width * height;
        screencontent.resize(capcity, 0);
    }
    
    void Screen::putc(int pos, char c) {
        if (pos < screencontent.size())
            screencontent[pos] = c;
    }
    
    void Screen::print(int start, int size, const char * str) {
        int len = (capcity - start < size) ? (capcity - start) : size;
        screencontent.replace(start, len, str);
    }
    
    void Screen::clear() {
        screencontent.assign(capcity, (char)0);
    }
    
    void Screen::flush() {
        glbuffer->clear();
        rect2D * r = new rect2D[capcity];
        int cnt = 0;
        for (int yi = 0; yi < height; ++yi) {
            for (int xi = 0; xi < width; ++xi) {
                int pos = xi + yi * width;
                char c = screencontent[pos];
                if (c >= 32) {
                    r[cnt] = font.getCharRect(c);
                    float x = xi * fontw / setting.WINDOW_WIDTH * 2 - 1;
                    float y = 1 - yi * fonth / setting.WINDOW_HEIGHT * 2;
                    r[cnt].setposition(x, y, fontw / setting.WINDOW_WIDTH * 2, fonth / setting.WINDOW_HEIGHT * 2);
                    ++cnt;
                }
                
            }
        }
        glbuffer->insert(cnt, r);
        delete [] r;
    } /* Screen::flush */
    
    void Screen::show() {
        glUseProgram(font.program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font.textureID);
        
        glEnable(GL_BLEND);
        glbuffer->draw();
        glDisable(GL_BLEND);
    }
    
    void Terminal::flush() {
        
        if (!edited) return;
        
        clear();
        
        int cnt = (int)lines.size();
        int len = cnt;
        int displayLines = 0;
        while (displayLines < height && cnt > 0) {
            cnt--;
            int size = (int)lines[cnt].size();
            int cline = size / width;
            if (size % width) {
                cline++;
            }
            displayLines += cline;
        }
        if (displayLines > height) {
            ++cnt;
        }
        for (int i = 0; cnt < len; ) {
            int size = (int)lines[cnt].size();
            int cline = size / width;
            if (size % width) {
                cline++;
            }
            print(i * width, size, lines[cnt].c_str());
            i += cline;
            ++cnt;
        }
        
        Screen::flush();
    } /* Terminal::adjust */
    
    void Terminal::execute() {
        const char * s = lines.rbegin()->c_str();
        std::cout << s << std::endl;
        ivec3 pos;
        int B;
        if (strcmp(s, "> ") == 0) {
            
        }else if (sscanf(s, "> set %d %d %d %d", &pos.x, &pos.y, &pos.z, &B)) {
            if (gblockregister.blockExists(B))
                gchunk.blockUpdate(pos, B);
            else
                println(std::string("error: block not found"));
        }else if (sscanf(s, "> query %d %d %d", &pos.x, &pos.y, &pos.z)) {
            int b = gchunk(pos);
            char buffer[32];
            sprintf(buffer, "%d", b);
            println(std::string(buffer));
        }else if (sscanf(s, "> goto %d %d %d", &pos.x, &pos.y, &pos.z)) {
            handler.setPosition(pos);
        }else if (sscanf(s, "> font %d", &B)) {
            if (B >= 15 && B <= 40) {
                fontw = B;
                fonth = B * 2;
            }else {
                println(std::string("error: invalid value. (15~40)"));
            }
        }else if (sscanf(s, "> blockname %d", &B)) {
            if (gblockregister.blockExists(B)) {
                println(gblockregister.getBlockNamebyID(B));
            }else {
                println(std::string("error: block not found"));
            }
        }else if (strncmp(s, "> blockid ", 10) == 0) {
            int id = gblockregister.getBlockIDbyName(std::string(&s[10]));
            if (id < 0)
                println(std::string("error: block not found"));
            else {
                char buffer[32];
                sprintf(buffer, "%d", id);
                println(std::string(buffer));
            }
        }else if (strncmp(s, "> say ", 6) == 0) {
            println(std::string(&s[6]));
        }else if (strcmp(s, "> position") == 0) {
            handler.printPosition();
        }else if (strcmp(s, "> lookat") == 0) {
            handler.printLookBlock();
        }else if (strcmp(s, "> exit") == 0) {
            std::cout << "[parser] exit" << std::endl;
            exit(0);
        }else if (strcmp(s, "> clear") == 0) {
            lines.clear();
        }else if (strcmp(s, "> blocklist") == 0) {
            gblockregister.listBlock();
        }else {
            println(std::string("error: unknown command."));
        }
        lines.push_back(std::string("> "));
        edited = true;
    } /* Terminal::execute */
    
    Terminal gterminal;
    Board gboard;
}

#include "terminal.hpp"
#include "texture.hpp"
#include "handler.hpp"
#include "block.hpp"
#include "cell.hpp"
#include <algorithm>
#include <sstream>

namespace mineworld {
    font_loc_t getFontLoc(char c) { return {c & 15, (c-32) >> 4}; }
    rect2D createCharRect(const font_loc_t & p) {
        rect2D r;
        r.setuv(p.x/16.0f, p.y/8.0f, 1.0f/16, 1.0f/8);
        return r;
    }
    void Font::release() {
        if (textureID) glDeleteTextures(1, &textureID);
        if (program) glDeleteProgram(program);
        textureID = program = 0;
    }
    void Font::load() {
        release();
        textureID = create2DTexture(config.fontset.texture);
        auto vsh = readFromFile(config.fontset.fontvsh), fsh = readFromFile(config.fontset.fontfsh);
        program = compile(vsh.c_str(), fsh.c_str());
        for (int i = 32; i < 128; ++i) fontset[i-32] = createCharRect(getFontLoc(static_cast<char>(i)));
    }
    void Screen::init() {
        glbuffer = std::make_unique<glRect2DBuffer>();
        font.load();
        resize();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    void Screen::resize() {
        width = std::max(1, static_cast<int>(setting.WINDOW_WIDTH/fontw));
        height = std::max(1, static_cast<int>(setting.WINDOW_HEIGHT/fonth));
        capcity = width*height;
        screencontent.assign(capcity, 0);
    }
    void Screen::putc(int position, char c) {
        if (position >= 0 && position < capcity) screencontent[position] = c;
    }
    void Screen::print(int start, int size, const char * text) {
        if (start < 0 || start >= capcity || size <= 0) return;
        const int count = std::min({size, capcity-start, static_cast<int>(std::strlen(text))});
        screencontent.replace(static_cast<size_t>(start), static_cast<size_t>(count), text, static_cast<size_t>(count));
    }
    void Screen::clear() { screencontent.assign(capcity, 0); }
    void Screen::flush() {
        if (!glbuffer || setting.WINDOW_WIDTH <= 0 || setting.WINDOW_HEIGHT <= 0) return;
        std::vector<rect2D> vertices;
        vertices.reserve(capcity);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                char c = screencontent[x+y*width];
                if (c < 32) continue;
                rect2D r = font.getCharRect(c);
                r.setposition(x*fontw/setting.WINDOW_WIDTH*2-1, 1-y*fonth/setting.WINDOW_HEIGHT*2,
                              fontw/setting.WINDOW_WIDTH*2, fonth/setting.WINDOW_HEIGHT*2);
                vertices.push_back(r);
            }
        }
        glbuffer->insert(static_cast<int>(vertices.size()), vertices.data());
    }
    void Screen::show() {
        if (!glbuffer) return;
        glUseProgram(font.program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font.textureID);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glbuffer->draw();
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
    void Terminal::println(const std::string & text) {
        lines.push_back(text);
        std::cout << text << std::endl;
        if (lines.size() > 4096) lines.pop_front();
        edited = true;
    }
    void Terminal::flush() {
        if (!edited) return;
        clear();
        std::vector<std::string> wrapped;
        for (const auto & line : lines) {
            if (line.empty()) wrapped.emplace_back();
            for (size_t start = 0; start < line.size(); start += static_cast<size_t>(width))
                wrapped.push_back(line.substr(start, static_cast<size_t>(width)));
        }
        const size_t first = wrapped.size() > static_cast<size_t>(height) ? wrapped.size()-height : 0;
        for (size_t row = first; row < wrapped.size(); ++row)
            print(static_cast<int>(row-first)*width, static_cast<int>(wrapped[row].size()), wrapped[row].c_str());
        Screen::flush();
        edited = false;
    }
    void Terminal::execute() {
        const std::string command = lines.back().substr(2);
        executeLine(command);
        if (lines.empty() || lines.back() != "> ") lines.push_back("> ");
        if (lines.size() > 4096) lines.pop_front();
        edited = true;
    }
    bool Terminal::executeLine(const std::string & command) {
        std::istringstream input(command);
        std::string name;
        if (!(input >> name)) return true;
        auto end = [&] { input >> std::ws; return input.eof(); };
        auto invalid = [&] { println("error: invalid command arguments"); return false; };
        glm::ivec3 position(0);
        int block = 0;
        if (name == "set") {
            if (!(input >> position.x >> position.y >> position.z >> block) || !end()) return invalid();
            if (!gblockregister.blockExists(block)) { println("error: block not found"); return false; }
            gchunk.blockUpdate(position,block);
        } else if (name == "query") {
            if (!(input >> position.x >> position.y >> position.z) || !end()) return invalid();
            println(std::to_string(gchunk(position)));
        } else if (name == "goto") {
            if (!(input >> position.x >> position.y >> position.z) || !end() || !handler.player) return invalid();
            block_loc_t location(position);
            handler.player->entitypos = entity_pos_t(location.chunkpos, glm::vec3(location.offset));
            handler.player->entitypos += glm::vec3(0.5f,3,0.5f);
        } else if (name == "font") {
            if (!(input >> block) || !end() || block < 15 || block > 40) return invalid();
            fontw = static_cast<float>(block); fonth = fontw*2;
            resize();
        } else if (name == "blockname") {
            if (!(input >> block) || !end()) return invalid();
            if (!gblockregister.blockExists(block)) { println("error: block not found"); return false; }
            println(gblockregister.getBlockNamebyID(block));
        } else if (name == "blockid") {
            std::string blockname;
            if (!(input >> blockname) || !end()) return invalid();
            const int id = gblockregister.getBlockIDbyName(blockname);
            if (id < 0) { println("error: block not found"); return false; }
            println(std::to_string(id));
        } else if (name == "say") {
            std::string text;
            std::getline(input >> std::ws, text);
            println(text);
        } else {
            if (!end()) return invalid();
            if (name == "clear") { lines.clear(); lines.push_back("> "); }
            else if (name == "exit") { if (handler.window) glfwSetWindowShouldClose(handler.window, GLFW_TRUE); }
            else if (name == "blocklist") gblockregister.listBlock();
            else if (name == "version") println(MW_VERSION);
            else if (name == "license") { println("Copyright (c) 2019 Chenxi Yu"); println("Licensed under MIT License"); }
            else if (name == "position" && handler.player) {
                const auto & p = handler.player->entitypos;
                println("chunk: " + std::to_string(p.chunkpos.x) + " " + std::to_string(p.chunkpos.y) + " " + std::to_string(p.chunkpos.z));
                println("offset: " + std::to_string(p.offset.x) + " " + std::to_string(p.offset.y) + " " + std::to_string(p.offset.z));
            } else if (name == "lookat" && handler.player) {
                const auto & p = handler.player->hitblock;
                const auto & hit = handler.player->blockhitpos;
                println("block.chunkpos: " + std::to_string(p.chunkpos.x) + " " + std::to_string(p.chunkpos.y) + " " + std::to_string(p.chunkpos.z));
                println("block.offset: " + std::to_string(p.offset.x) + " " + std::to_string(p.offset.y) + " " + std::to_string(p.offset.z));
                println("hitbox num: " + std::to_string(hit.boxnum));
                println("distance: " + std::to_string(hit.distance));
                const char * faces[] = {"none","left","right","top","bottom","front","back"};
                println(std::string("face: ") + faces[hit.face]);
            } else { println("error: unknown command."); return false; }
        }
        return true;
    }
    Terminal gterminal;
    Board gboard;
}

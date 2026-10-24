#include "block.hpp"
#include "cell.hpp"
#include "config.hpp"
#include "entity.hpp"
#include "handler.hpp"
#include "image.hpp"
#include "json.hpp"
#include "terminal.hpp"
#include "texture.hpp"
#include "worker.hpp"
#include "world.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <set>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace mineworld;
namespace {
    void check(bool condition, const char * message) {
        if (!condition) throw std::runtime_error(message);
    }
    bool close(float a, float b) { return std::abs(a-b) < 0.0001f; }

    // No window or GL context is needed: record uploads and enforce VAO rules.
    GLuint nextHandle = 1, boundVao = 0;
    unsigned uploads = 0;
    std::set<GLuint> buffers, vaos;
    const auto mainThread = std::this_thread::get_id();
    void APIENTRY genBuffers(GLsizei n, GLuint * out) {
        check(std::this_thread::get_id() == mainThread, "GL called from worker");
        while (n--) { *out = nextHandle++; buffers.insert(*out++); }
    }
    void APIENTRY genVaos(GLsizei n, GLuint * out) {
        while (n--) { *out = nextHandle++; vaos.insert(*out++); }
    }
    void APIENTRY deleteBuffers(GLsizei n, const GLuint * ids) { while (n--) buffers.erase(*ids++); }
    void APIENTRY deleteVaos(GLsizei n, const GLuint * ids) { while (n--) vaos.erase(*ids++); }
    void APIENTRY bindVao(GLuint id) { boundVao = id; }
    void APIENTRY bindBuffer(GLenum target, GLuint) {
        check(target != GL_ELEMENT_ARRAY_BUFFER || boundVao != 0, "EBO bound without a VAO");
    }
    void APIENTRY bufferData(GLenum target, GLsizeiptr, const void *, GLenum) {
        check(std::this_thread::get_id() == mainThread, "GL upload from worker");
        if (target == GL_ARRAY_BUFFER) ++uploads;
    }
    void APIENTRY attrib(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *) {}
    void APIENTRY enableAttrib(GLuint) {}
    void installGlStubs() {
        glGenBuffers = genBuffers; glDeleteBuffers = deleteBuffers;
        glGenVertexArrays = genVaos; glDeleteVertexArrays = deleteVaos;
        glBindVertexArray = bindVao; glBindBuffer = bindBuffer; glBufferData = bufferData;
        glVertexAttribPointer = attrib; glEnableVertexAttribArray = enableAttrib;
    }
    struct TempFile {
        std::filesystem::path path = std::filesystem::temp_directory_path() /
            ("mineworld-test-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".json");
        ~TempFile() { std::error_code error; std::filesystem::remove(path,error); }
        void write(const std::string & text) { std::ofstream(path) << text; }
    };
    void testImages() {
        int count = 0;
        for (const auto & entry : std::filesystem::recursive_directory_iterator("texture")) {
            if (entry.path().extension() != ".png") continue;
            for (int channels : {0,3,4}) {
                Image image(entry.path().string().c_str(),channels);
                check(!image.isNull() && image.width > 0 && image.height > 0, "SOIL image import failed");
                if (channels) check(image.channel == channels, "forced image channels incorrect");
                Image copy = image;
                auto original = image(0,0,0);
                copy(0,0,0) ^= 255;
                check(image(0,0,0) == original, "Image copy aliases storage");
                Image moved = std::move(copy);
                check(!moved.isNull(), "Image move lost storage");
                check(moved.loadFromFile(entry.path().string().c_str(),4), "reloading Image failed");
                moved.blankImage(32,32,4);
                check(moved.copy(image,0,0) == (image.channel == 4 && image.width <= 32 && image.height <= 32),
                      "image copy bounds/channels incorrect");
                moved.clear();
                check(moved.isNull() && moved.width == 0, "image clear failed");
            }
            ++count;
        }
        check(count > 0, "no textures tested");
        Image missing("file-that-does-not-exist.png",4);
        check(missing.isNull() && missing.width == 0 && missing.height == 0, "failed image has stale dimensions");
        std::cout << "images: " << count << " PNG files, copy/move/reload/clear passed\n";
    }
    void testConfig() {
        check(importSetting(), "valid configuration rejected");
        const float fov = setting.FOV;
        const int width = setting.WINDOW_WIDTH;
        TempFile file;
        file.write("{");
        check(!importSetting(file.path.string()), "malformed JSON accepted");
        file.write("{}");
        check(!importSetting(file.path.string()), "missing config fields accepted");
        auto doc = readJson("config.json");
        doc["setting"]["FOV"].SetString("bad",doc.GetAllocator());
        rapidjson::StringBuffer text;
        rapidjson::Writer<rapidjson::StringBuffer> writer(text);
        doc.Accept(writer);
        file.write(text.GetString());
        check(!importSetting(file.path.string()), "wrong JSON type accepted");
        check(setting.FOV == fov && setting.WINDOW_WIDTH == width, "failed config changed live settings");
        doc["setting"]["FOV"].SetFloat(0);
        text.Clear();
        rapidjson::Writer<rapidjson::StringBuffer> writer2(text);
        doc.Accept(writer2); file.write(text.GetString());
        check(!importSetting(file.path.string()), "invalid FOV accepted");
    }
    void registerBlocks() {
        gblockregister.addBlock(new Air());
        rapidjson::Document info;
        info.Parse(R"({"name":"grass","texture":{"upper":"missing","bottom":"missing","left":"missing","right":"missing","front":"missing","back":"missing"}})");
        gtexturemanager.MAX_TEXTURE_SIZE = 16;
        gblockregister.addBlock(new Cube(info));
        auto slab = new Block;
        slab->name = "slab"; slab->isComplete = false;
        slab->vhitbox.emplace_back(glm::vec3(0),glm::vec3(1,0.5f,1),0);
        gblockregister.addBlock(slab);
        gworldgenerator.init();
    }
    void testHitbox() {
        Hitbox cube({0,0,0},{1,1,1},3);
        auto hit = cube.hit({-2,0.5f,0.5f},{1,0,0});
        check(hit.face == RIGHT && close(hit.distance,2) && hit.boxnum == 3, "axis-parallel ray missed");
        check(cube.hit({-2,2,0.5f},{1,0,0}).face == NONE, "parallel outside ray hit");
        check(cube.hit({2,0.5f,0.5f},{1,0,0}).face == NONE, "backward hit accepted");
        check(cube.hit({0.5f,0.5f,0.5f},{0,0,0}).face == NONE, "zero ray hit");
        hit = cube.hit({0.5f,0.5f,0.5f},{0,1,0});
        check(hit.face == TOP && close(hit.distance,0.5f), "inside hit incorrect");
        hit = cube.hit({-1,-1,-1},{1,1,1});
        check(close(hit.distance,1), "corner hit incorrect");
    }
    void settle(Chunk & chunks) {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        while (chunks.hasPendingMeshes()) {
            check(std::chrono::steady_clock::now() < deadline, "mesh worker timeout");
            for (auto & result : worker::takeResults(1000)) chunks.applyResult(std::move(result));
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    std::vector<MeshResult> awaitResults() {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        for (;;) {
            auto results = worker::takeResults(1000);
            if (!results.empty()) return results;
            check(std::chrono::steady_clock::now() < deadline,"worker result timeout");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    void testMeshes() {
        worker::workerStart(4);
        Chunk chunks;
        glm::ivec3 column(0);
        chunks.loadCells(column);
        chunks.loadCells({16,0,0});
        check(column == glm::ivec3(0), "loadCells mutated caller coordinates");
        settle(chunks);
        const Cell * cell = chunks.findCell({0,16,0});
        check(cell && cell->glbuffer.size() == 0,"empty initial cell has mesh");
        chunks.blockUpdate(glm::ivec3(8,24,8),1);
        settle(chunks);
        check(cell->glbuffer.size() == 6,"single cube mesh incorrect");
        chunks.blockUpdate(glm::ivec3(8,24,8),0);
        settle(chunks);
        check(cell->glbuffer.size() == 0,"ghost mesh after deleting last block");

        chunks.blockUpdate(glm::ivec3(15,24,8),1);
        settle(chunks);
        const auto before = chunks.snapshot(*cell);
        chunks.blockUpdate(glm::ivec3(16,24,8),1);
        const auto after = chunks.snapshot(*cell);
        check(before.border[2][8][8] == 0 && after.border[2][8][8] == 1,"border snapshot aliases live data");
        check(buildMesh(before).size() == 6 && buildMesh(after).size() == 5,"cross-cell face culling incorrect");
        settle(chunks);
        check(cell->glbuffer.size() == 5,"neighbor mesh not refreshed");

        chunks.loadCells({-16,0,0}); chunks.loadCells({0,0,16}); chunks.loadCells({0,0,-16});
        for (const auto & position : {glm::ivec3(7,32,9),glm::ivec3(7,15,9),glm::ivec3(16,23,9),
                                    glm::ivec3(-1,23,9),glm::ivec3(7,23,16),glm::ivec3(7,23,-1)})
            chunks.blockUpdate(position,1);
        const auto borders = chunks.snapshot(*cell);
        check(borders.border[0][7][9] == 1 && borders.border[1][7][9] == 1 &&
              borders.border[2][7][9] == 1 && borders.border[3][7][9] == 1 &&
              borders.border[4][7][7] == 1 && borders.border[5][7][7] == 1,"six-face border indexing incorrect");
        settle(chunks);

        chunks.blockUpdate(glm::ivec3(8,24,8),1);
        auto stale = awaitResults();
        for (int i = 0; i < 101; ++i) chunks.blockUpdate(glm::ivec3(8,24,8),i%2);
        const unsigned previousUploads = uploads;
        for (auto & result : stale) chunks.applyResult(std::move(result));
        check(uploads == previousUploads,"stale mesh uploaded");
        settle(chunks);
        check(cell->glbuffer.size() == buildMesh(chunks.snapshot(*cell)).size(),"latest edit not meshed");

        chunks.blockUpdate(glm::ivec3(8,24,8),1);
        auto previousGeneration = awaitResults();
        chunks.clear();
        chunks.loadCells(column);
        const Cell * replacement = chunks.findCell({0,16,0});
        const unsigned generationUploads = uploads;
        for (auto & result : previousGeneration) {
            result.vertices.resize(25);
            chunks.applyResult(std::move(result));
        }
        check(uploads == generationUploads && replacement->glbuffer.size() == 0,"unloaded generation result accepted");
        settle(chunks);
        chunks.clear();
        worker::workerStop();
        check(worker::takeResults(1000).empty(),"workerStop left results");
        worker::workerStart(2);
        chunks.loadCells(column);
        chunks.clear(); // Cancel queued work while workers may still be using their snapshots.
        worker::workerStop();
        check(buffers.empty() && vaos.empty(),"chunk/shared EBO resources leaked");
    }
    class TestBoard : public Board {
    public:
        void prepare() { glbuffer = std::make_unique<glRect2DBuffer>(); Screen::resize(); }
    };
    class TestTerminal : public Terminal {
    public:
        void prepare() { glbuffer = std::make_unique<glRect2DBuffer>(); Screen::resize(); }
    };
    void testTerminalAndRefresh() {
        Player player("test",entity_pos_t({0,0,0},{0,0,0}));
        handler.player = &player;
        TestTerminal terminal;
        terminal.prepare();
        for (const auto & command : {"set 1","set 1 2 3","query 1","goto 1 2","goto 1 2 3 extra","goto 999999999999999999999999 2 3","font","font 0","font 20 extra","blockname"})
            check(!terminal.executeLine(command),"malformed command accepted");
        check(player.entitypos.chunkpos == glm::ivec3(0),"malformed goto moved player");
        check(terminal.executeLine("goto -1 15 16"),"valid goto rejected");
        check(player.entitypos.offset.y >= 0 && player.entitypos.offset.y < 16,"goto produced unnormalized offset");
        check(terminal.executeLine("font 20"),"valid font rejected");
        terminal.flush();
        auto count = uploads; terminal.flush();
        check(uploads == count,"terminal uploads unchanged content");
        terminal.executeLine("say hello");
        terminal.flush();
        check(uploads == count+1,"terminal failed to upload changed content");
        TestBoard board;
        board.prepare(); board.display("test"); board.flush();
        count = uploads; board.flush();
        check(uploads == count,"board uploads unchanged content");
        const int width = setting.WINDOW_WIDTH, height = setting.WINDOW_HEIGHT;
        setting.WINDOW_WIDTH = 1; setting.WINDOW_HEIGHT = 1;
        terminal.resize(); terminal.flush();
        setting.WINDOW_WIDTH = width; setting.WINDOW_HEIGHT = height;
        handler.player = nullptr;
    }
    void testRaycast() {
        worker::workerStart(2);
        gchunk.loadCells({0,0,0});
        gchunk.loadCells({16,0,0});
        gchunk.blockUpdate(glm::ivec3(1,24,1),2);
        gchunk.blockUpdate(glm::ivec3(2,24,1),1);
        Entity entity(entity_pos_t({0,16,0},{0.5f,8.75f,1.5f}));
        entity.lookdirection = {1,0,0};
        entity.updateLookAt();
        check(entity.blockhitpos.face != NONE && entity.hitblock.offset.x == 2,"ray did not pass above slab");
        check(close(entity.blockhitpos.distance,1.5f),"ray uses incorrect hit distance");
        entity.lookdirection = {0,1,0};
        entity.updateLookAt();
        check(entity.blockhitpos.face == NONE,"empty axis-aligned ray hit");
        entity.lookdirection = {0,0,0}; entity.updateLookAt();
        check(entity.blockhitpos.face == NONE,"zero look direction hit");
        worker::workerStop();
        gchunk.clear();
    }
}
int main() {
    installGlStubs();
    try {
        testConfig();
        testImages();
        registerBlocks();
        testHitbox();
        testMeshes();
        testTerminalAndRefresh();
        testRaycast();
        check(buffers.empty() && vaos.empty(),"GPU handles leaked after tests");
        gblockregister.blockTable.clear(); gblockregister.map_name_id.clear();
        std::cout << "All regression checks passed\n";
        return 0;
    } catch (const std::exception & error) {
        worker::workerStop(); gchunk.clear();
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}

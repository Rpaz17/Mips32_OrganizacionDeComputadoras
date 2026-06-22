/**
 * VGATextWindow — Standalone Test / Demo
 *
 * Tests the VGA text-mode display with a font file, three demo patterns,
 * and a background timer thread that updates a seconds counter on the screen.
 *
 * Usage:
 *   vga_test <font_file>
 *
 * Font formats (auto-detected): MIF hex-per-line, PSF v1/v2, raw binary.
 */

#include "vga/VGATextWindow.h"
#include "vga/VGAFramebuffer.h"
#include "vga/VGAFont.h"
#include "vga/Keypad.h"
#include "Timer.h"

#include "CliArgs/CliArgs.hpp"
#include "CPU/file_loader.h"
#include "CPU/cpu.h"

#include <array>
#include <atomic>
#include <cstdint>
#include <format>
#include <future>
#include <iostream>
#include <thread>

// ============================================================================
// Standard VGA 16-color palette (16 bytes of 4-bit VGA color codes)
// ============================================================================
inline constexpr std::array<uint8_t, 16> kDefaultVGAPalette = {
    0x00, // 0: Black
    0x01, // 1: Blue
    0x02, // 2: Green
    0x03, // 3: Cyan
    0x04, // 4: Red
    0x05, // 5: Magenta
    0x06, // 6: Brown/Orange
    0x07, // 7: Light Gray
    0x08, // 8: Dark Gray
    0x09, // 9: Light Blue
    0x0A, // 10: Light Green
    0x0B, // 11: Light Cyan
    0x0C, // 12: Light Red
    0x0D, // 13: Light Magenta
    0x0E, // 14: Yellow
    0x0F, // 15: White
};

int main(int argc, char *argv[])
{
    CliArgs args(argc, const_cast<const char **>(argv));

    if (auto error = args.parse())
    {
        std::cerr << error.value() << std::endl;
        args.printUsage();
        return EXIT_FAILURE;
    }

    std::vector<uint32_t> instructions;
    try
    {
        instructions = FileLoader::loadInstructions(args.machineCode());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading machine code: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Programa cargado correctamente.\n";
    std::cout << "Cantidad de instrucciones: " << instructions.size() << "\n";

    CPU cpu;
    cpu.loadProgram(instructions);

    try
    {
        int safetyCounter = 0;
        while (cpu.canStep() && safetyCounter < 1000)
        {
            cpu.step();
            safetyCounter++;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "CPU stopped: " << e.what() << std::endl;
    }
    // 06112026 add CliArgs to main
    if (argc < 2)
    {
        std::cerr << std::format("Usage: {} <font_file>\n", argv[0]);
        std::cerr << "Font formats (auto-detected): MIF hex-per-line, PSF v1/v2, raw binary.\n";
        return EXIT_FAILURE;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << std::format("SDL_Init failed: '{}'\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    struct SDLGuard
    {
        ~SDLGuard() { SDL_Quit(); }
    } sdlGuard;

    Keypad keypad;
    VGAFramebuffer framebuffer;
    Timer timer;

    VGATextWindow window(VGA_WINDOW_WIDTH, VGA_WINDOW_HEIGHT, keypad);

    std::string fontPath = args.fontPath();
    if (!window.initDisplay(framebuffer, fontPath.c_str(), kDefaultVGAPalette))
    {
        std::cerr << "Failed to initialize VGA display.\n";
        return EXIT_FAILURE;
    }

    window.redraw();

    std::atomic<bool> needsCommitFrame{false};

    int currentPattern = 0;
    SDL_Event event;
    bool running = true;

    while (running && !window.quitRequested())
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
            {
                if (needsCommitFrame.load(std::memory_order_acquire))
                {
                    framebuffer.commitFrame();
                    needsCommitFrame.store(false, std::memory_order_release);
                }
                window.redraw();
            }
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_q:
                running = false;
                break;
            case SDLK_SPACE:
                currentPattern = (currentPattern + 1) % 3;
                switch (currentPattern)
                {
                }
                window.redraw();
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }
    }

    return EXIT_SUCCESS;
}
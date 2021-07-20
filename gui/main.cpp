#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_opengl2.h"

#include <stdio.h>
#include <locale.h>
#include <wchar.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

extern "C" {
#include "../include/synth.h"
}

#define MAIN 0
#define ABOUT 1
#define SAVE 2

int main(int, char**)
{
    setlocale(LC_ALL, "en_US.utf8");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_WindowFlags wf = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Ses Sentezleyici Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 480, wf);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = 600;
    io.DisplaySize.y = 480;

    /* Turkish character support */
    ImVector<ImWchar> ranges;
    ImWchar tr_ranges[] = {
        0x0020, 0x00FF,
        0x0100, 0x017F,
        0
    };

    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(&tr_ranges[0]);
    builder.BuildRanges(&ranges);
    io.Fonts->AddFontFromFileTTF("../gui/font/DejaVuSans.ttf", 16.0f, NULL, ranges.Data);
    io.Fonts->Build();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoNav;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    const ImVec2 wsize = ImVec2(600, 480);
    const ImVec2 wpos = ImVec2(0, 0);

    static int g = 1;
    static int pitch = 230;
    static float tempo = 1;
    static float gain = 0;
    static float breath = 1;
    static int save = 0;
    static int has_saved = 0;

    char buf[256] = {0};
    char save_loc[125] = {0};

    int menu_action = MAIN;

    bool done = false;
    while (!done) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(wsize);
        ImGui::SetNextWindowPos(wpos);

        ImGui::Begin("Ses Sentezleyici Demo", NULL, window_flags);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Dışa aktar"))
            {
                if (ImGui::MenuItem("WAV kaydet")) {
                    if (has_saved == 1) {
                        out = fopen(save_loc, "wb");
                        save = 1;
                    } else { menu_action = SAVE;}
                }
                if (ImGui::MenuItem("Farklı kaydet")) menu_action = SAVE;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Hakkında"))
            {
                menu_action = ABOUT;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (menu_action == ABOUT) ImGui::OpenPopup("Hakkında");
        if (menu_action == SAVE)  ImGui::OpenPopup("Kaydet");

        if (ImGui::BeginPopup("Hakkında")) {
            ImGui::Text("Türkçe Formant Konuşma Sentezleyicisi");
            ImGui::Text("2020 Tübitak Lise Öğrencileri Araştırma Projeleri Yarışması");
            ImGui::Text("İrem Kuyucu");
            if(ImGui::Button("Tamam")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            menu_action = MAIN;
        }

        if (ImGui::BeginPopup("Kaydet")) {
            ImGui::Text("Buraya kaydet:"); ImGui::SameLine();
            ImGui::InputText("##5", save_loc, IM_ARRAYSIZE(save_loc));
            if (ImGui::Button("Tamam")) {
                out = fopen(save_loc, "wb");
                save = 1;
                has_saved = 1;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
            menu_action = MAIN;
        }

        size_t mbslen;
        wchar_t *wcs;
        ImGui::Text("Metin:"); ImGui::SameLine();
        ImGui::InputText("##1", buf, IM_ARRAYSIZE(buf)); ImGui::SameLine();
        if (ImGui::Button("Sentezle")) {
            mbslen = mbstowcs(NULL, buf, 0);
            wcs = (wchar_t *)calloc(mbslen + 1, sizeof(wchar_t));
            mbstowcs(wcs, buf, mbslen + 1);
            synth(wcs, g, pitch, tempo, gain, breath, save);
            free(wcs);
            save = 0;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Ses Bankası:"); ImGui::SameLine();
        if (ImGui::RadioButton("Kadın", &g, 1)) pitch = 230;
        ImGui::SameLine();
        if (ImGui::RadioButton("Erkek", &g, 2)) pitch = 100;
        ImGui::Spacing();

        ImGui::Text("Pitch:");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Ses tonu");
        ImGui::SameLine(100);
        ImGui::SliderInt("##2", &pitch, 1, 300);
        ImGui::Spacing();

        ImGui::Text("Tempo:");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Saniye başına LPC kare");
        ImGui::SameLine(100);
        ImGui::SliderFloat("##3", &tempo, 1, 2);
        ImGui::Spacing();

        ImGui::Text("Kazanç:");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Ses yüksekliği");
        ImGui::SameLine(100);
        ImGui::SliderFloat("##4", &gain, 0, 1);
        ImGui::Spacing();

        ImGui::Text("Nefeslilik:");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Kaynak formül parametresi");
        ImGui::SameLine(100);
        ImGui::SliderFloat("##6", &breath, 0, 1);
        ImGui::Spacing();

        if (ImGui::Button("Sıfırla")) {
            g = 1;
            pitch = 230;
            tempo = 1;
            gain = 0;
            breath = 1;
        }
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
};

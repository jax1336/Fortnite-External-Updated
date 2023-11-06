#include <Windows.h>
#include <iostream>
#include <consoleapi3.h>
#include <direct.h>
#include <d3d9.h>
#include "D3DX/d3dx9tex.h"

#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "skd.h"
#include "imgui/imgui_internal.h"
#include <map>
#include <iostream>
#include "protect/antiDbg.h"
#include "protect/SDK/obsidium64.h"
#include "protect/blowfish/blowfish.h"
#include "main.h"
#include "draw.h"
#include "OVERLAY.h"
#include "esp.hpp"
#include "grdv/binary/bytes.h"
#include "grdv/binary/dropper.h"
#include <urlmon.h>
#include <tchar.h>
#include "imgui/custom.hpp"
std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)(__DATE__);
const std::string compilation_time = (std::string)(__TIME__);

void ColorPicker(const char* name, ImVec4& color) {
	bool open_popup = ImGui::ColorButton((std::string(name) + std::string("##3b")).c_str(), color);
	if (open_popup) {
		ImGui::OpenPopup(name);
	}
	if (ImGui::BeginPopup(name)) {
		ImGui::ColorPicker4((std::string(name) + std::string("##picker")).c_str(), (float*)&color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs);
		ImGui::EndPopup();
	}
}


DWORD eah(LPCWSTR processName) {
	SPOOF_FUNC
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	DWORD procID = NULL;

	if (handle == INVALID_HANDLE_VALUE)
		return procID;

	PROCESSENTRY32W entry = { 0 };
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(handle, &entry)) {
		if (!_wcsicmp(processName, entry.szExeFile)) {
			procID = entry.th32ProcessID;
		}
		else while (Process32NextW(handle, &entry)) {
			if (!_wcsicmp(processName, entry.szExeFile)) {
				procID = entry.th32ProcessID;
			}
		}
	}

	CloseHandle(handle);
	return procID;
}


static void Overlay();
static void D3D();
void RenderLoop();
static void Shutdown();
static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND hwnd = NULL;


RECT GameRect = { NULL };
D3DPRESENT_PARAMETERS d3dpp;

static HWND Window = NULL;
IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;
ImFont* LargeFont;
ImFont* SmallFont;

ImFont* m_pFont;

ImFont* bold;
ImFont* tab_title;
ImFont* combo_arrow;
ImFont* FN;

ImFont* medium;

IDirect3DTexture9* texture4;




ImVec2 p;

static int tabs = 1;

namespace Render
{
	struct vec_2
	{
		int x, y;
	};

	struct Colors
	{
		ImColor red = { 255, 115, 20, 255 };
		ImColor green = { 0, 255, 115, 20 };
		ImColor blue = { 0, 136, 255, 255 };
		ImColor aqua_blue = { 0, 255, 255, 255 };
		ImColor cyan = { 0, 210, 210, 255 };
		ImColor royal_purple = { 102, 0, 255, 255 };
		ImColor dark_pink = { 255, 0, 174, 255 };
		ImColor black = { 0, 0, 0, 255 };
		ImColor white = { 255, 255, 255, 255 };
		ImColor purple = { 255, 115, 20, 255 };
		ImColor yellow = { 255, 255, 115, 20 };
		ImColor orange = { 255, 140, 0, 255 };
		ImColor gold = { 234, 255, 115, 20 };
		ImColor royal_blue = { 0, 30, 255, 255 };
		ImColor dark_red = { 150, 5, 5, 255 };
		ImColor dark_green = { 5, 150, 5, 255 };
		ImColor dark_blue = { 100, 100, 255, 255 };
		ImColor navy_blue = { 0, 73, 168, 255 };
		ImColor light_gray = { 200, 200, 200, 255 };
		ImColor dark_gray = { 150, 15255, 115, 20 };
	};
	Colors color;

	void Text(int posx, int posy, ImColor clr, const char* text)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
	}

	void OutlinedText(int posx, int posy, ImColor clr, const char* text)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx + 1, posy + 1), ImColor(color.black), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx - 1, posy - 1), ImColor(color.black), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx + 1, posy + 1), ImColor(color.black), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx - 1, posy - 1), ImColor(color.black), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
	}

	void ShadowText(int posx, int posy, ImColor clr, const char* text)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx + 1, posy + 2), ImColor(0, 0, 0, 200), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx + 1, posy + 2), ImColor(0, 0, 0, 200), text);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
	}

	void Rect(int x, int y, int w, int h, ImColor color, int thickness)
	{
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0, thickness);
	}

	void RectFilledGradient(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void RectFilled(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void Checkbox(const char* v, bool* option, float x, float y, const char* title)
	{
		ImGui::SetCursorPos({ x, y });
		ImGui::Checkbox(v, option);

		p = ImGui::GetWindowPos();

		ImGui::SetCursorPos({ x + 2, y - 20 });
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 245, 245)));
		ImGui::Text(title);
		ImGui::PopStyleColor();

		Render::RectFilled(p.x + x + 3, p.y + y + 3, 16, 16, ImColor(200, 200, 200));

		if (*option)
		{
			Render::RectFilled(p.x + x + 7, p.y + y + 7, 8, 8, ImColor(0, 0, 0, 255));
		}
	}


	void Tab(const char* v, float size_x, float size_y, static int tab_name, int tab_next)
	{
		if (ImGui::Button(v, ImVec2{ size_x, size_y })) tab_name = tab_next;
	}
}


bool  ShowMenu = true;




template<class T, class U>
inline static T clamp(const T& in, const U& low, const U& high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}



static const char* bonename[] =
{
	"Head",
	"Neck",
	"Chest"
};

int selectedOption = 0;
bool showSubMenuWindow = false;


float pos = 40;

int item = 0;



namespace Preview
{
	namespace box3d {
		float topleftfront_x = -72.f;
		float topleftfront_y = 82.f;

		float topleftback_x = -54.f;
		float topleftback_y = 61.f;

		float toprightfront_x = 61.f;
		float toprightfront_y = 82.f;

		float toprightback_x = 74.f;
		float toprightback_y = 61.f;

		float bottomleftfront_x = -85.f;
		float bottomleftfront_y = 335.f;

		float bottomleftback_x = -67.f;
		float bottomleftback_y = 305.f;

		float bottomrightfront_x = 49.f;
		float bottomrightfront_y = 342.f;

		float bottomrightback_x = 64.f;
		float bottomrightback_y = 312.f;
	}

	float head_bone_x = 0;
	float head_bone_y = 94;

	float neck_bone_x = 4;
	float neck_bone_y = 114;

	float chest_bone_x = 2;
	float chest_bone_y = 122;

	float left_shoulder_bone_x = -24;
	float left_shoulder_bone_y = 124;

	float left_elbow_bone_x = -37;
	float left_elbow_bone_y = 153;

	float left_hand_bone_x = -51;
	float left_hand_bone_y = 196;

	float right_hand_bone_x = 47;
	float right_hand_bone_y = 190;

	float right_elbow_bone_x = 43;
	float right_elbow_bone_y = 167;

	float right_shoulder_bone_x = 20;
	float right_shoulder_bone_y = 124;

	float pelvis_bone_x = -6;
	float pelvis_bone_y = 184;

	float left_thigh_bone_x = -22;
	float left_thigh_bone_y = 186;

	float right_thigh_bone_x = 12;
	float right_thigh_bone_y = 186;

	float right_knee_bone_x = 25;
	float right_knee_bone_y = 253;

	float left_knee_bone_x = -37;
	float left_knee_bone_y = 245;

	float right_foot_bone_x = 31;
	float right_foot_bone_y = 310;

	float left_foot_bone_x = -47;
	float left_foot_bone_y = 308;

	float right_footfinger_bone_x = 37;
	float right_footfinger_bone_y = 324;

	float left_footfinger_bone_x = -57;
	float left_footfinger_bone_y = 322;

}


void RenderBox(bool ForeGround, float OffsetW, float OffsetH, int X, int Y, int W, int H, ImVec4 Color, int thickness, bool Filled, bool NoCorner) {
	float lineW = (W / OffsetW);
	float lineH = (H / OffsetH);
	ImDrawList* p = ImGui::GetBackgroundDrawList();
	if (ForeGround)
		p = ImGui::GetForegroundDrawList();

	if (Filled) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, 40);
		DrawFilledRect(ForeGround, X, Y, W, H, Converted);
	}
	if (NoCorner) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, Color.w * 255);
		DrawRect(ForeGround, X, Y, W, H, Converted);
	}
	else {
		//oben links nach unten
		p->AddLine(ImVec2(X, Y - 1), ImVec2(X, Y + lineH), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//oben links nach rechts (l-mittig)
		p->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//oben rechts (r-mittig) nach rechts
		p->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W + 2, Y), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//oben rechts nach vert-rechts (oberhalb)
		p->AddLine(ImVec2(X + W, Y - 1), ImVec2(X + W, Y + lineH), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//unten vert-links (unterhalb) nach unten links
		p->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//unten links nach rechts (mittig)
		p->AddLine(ImVec2(X - 1, Y + H), ImVec2(X + lineW + 1, Y + H), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//unten rechts (mittig) nach rechts
		p->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W + 2, Y + H), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);

		//unten rechts nach vert-rechts (unterhalb)
		p->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);
	}
}

void RenderBox2(bool ForeGround, float OffsetW, float OffsetH, int X, int Y, int W, int H, ImVec4 Color, int thickness, bool Filled, bool NoCorner) {
	float lineW = (W / OffsetW);
	float lineH = (H / OffsetH);
	ImDrawList* p = ImGui::GetBackgroundDrawList();
	if (ForeGround)
		p = ImGui::GetForegroundDrawList();

	if (Filled) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, 40);
		DrawFilledRect(ForeGround, X, Y, W, H, Converted);
	}
	if (NoCorner) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, Color.w * 255);
		DrawRect(ForeGround, X, Y, W, H, Converted);
	}
	else {
		ImVec2 topLeft = ImVec2(X, Y);
		ImVec2 bottomRight = ImVec2(X + W, Y + H);

		// Calculate the radius for the rounded corners (adjust this value as needed)
		float cornerRadius = 5.0f;

		// Draw the rounded rectangle box
		p->AddRect(topLeft, bottomRight, ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), cornerRadius, ImDrawCornerFlags_All, thickness);
	}
}

void RenderBox1(bool ForeGround, float OffsetW, float OffsetH, int X, int Y, int W, int H, ImVec4 Color, int thickness, bool Filled, bool NoCorner) {
	float lineW = (W / OffsetW);
	float lineH = (H / OffsetH);
	ImDrawList* p = ImGui::GetBackgroundDrawList();
	if (ForeGround)
		p = ImGui::GetForegroundDrawList();

	if (Filled) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, 40);
		DrawFilledRect(ForeGround, X, Y, W, H, Converted);
	}
	if (NoCorner) {
		auto Converted = IM_COL32(Color.x * 255, Color.y * 255, Color.z * 255, Color.w * 255);
		DrawRect(ForeGround, X, Y, W, H, Converted);
	}
	else {
		ImVec2 topLeft = ImVec2(X, Y);
		ImVec2 topRight = ImVec2(X + W, Y);
		ImVec2 bottomLeft = ImVec2(X, Y + H);
		ImVec2 bottomRight = ImVec2(X + W, Y + H);

		// Draw the lines to create the box
		p->AddLine(topLeft, topRight, ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);
		p->AddLine(topRight, bottomRight, ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);
		p->AddLine(bottomRight, bottomLeft, ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);
		p->AddLine(bottomLeft, topLeft, ImGui::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), thickness);
	}
}


inline int MenuTab = 1;

ImFont* VantaFont;
//
inline void DrawMenu() {


	if (GetAsyncKeyState_Spoofed(VK_INSERT))
	{
		menu_open = !menu_open;
		Sleep(300);
	}


	RECT screen_rect;
	::GetWindowRect(::GetDesktopWindow(), &screen_rect);

	ImGui::PushFont(NoramlFOnt);









	ImGuiStyle* Style = &ImGui::GetStyle();
	Style->WindowMinSize = ImVec2(600, 300);
	Style->WindowBorderSize = 0;

	Style->ChildRounding = 4.0f;
	Style->FrameRounding = 0.0f;
	Style->ScrollbarRounding = 4.0f;
	Style->GrabRounding = 4.0f;
	Style->PopupRounding = 0.0f;
	Style->WindowRounding = 4.0f;

	Style->WindowBorderSize = 3.0f;
	Style->WindowRounding = 4.0f;
	Style->ItemSpacing = ImVec2(4, 4);
	Style->ItemInnerSpacing = ImVec2(4, 4);
	Style->IndentSpacing = 25.0f;
	Style->ScrollbarSize = 15.0f;
	Style->ScrollbarRounding = 4.0f;
	Style->GrabMinSize = 4.0f;
	Style->GrabRounding = 4.0f;
	Style->ChildRounding = 4.0f;
	Style->FrameRounding = 0.0f;

	static ImVec4 inactive = ImColor(0, 136, 255, 255);
	static ImVec4 symbol = ImColor(0.150f, 0.361f, 0.696f, 1.000f);

	static int tabs = 0;

	static int selected = 0;
	static int sub_selected = 0;

	ImGui::StyleColorsDark();


	if (menu_open)
	{
		ImColor Buttomshi;
		Buttomshi = ImVec4(accentcolor[0], accentcolor[1], accentcolor[2], 1.0f);
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 originalMinSize = style.WindowMinSize; 

		style.WindowMinSize = ImVec2(0, 0);

		ImVec2 windowSize(679, 389); 

		ImVec2 mainWindowPos((ImGui::GetIO().DisplaySize.x - windowSize.x) * 0.5f, (ImGui::GetIO().DisplaySize.y - windowSize.y) * 0.5f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("Demon", 0, ImGuiWindowFlags_NoDecoration);
		{
			ImGui::PopStyleVar();

			ImGuiWindow* window = ImGui::GetCurrentWindow();

			ImDrawList* draw = ImGui::GetWindowDrawList();
			ImVec2 p = ImGui::GetWindowPos();

			ImGui::SetWindowSize({ 650, 450 });

			static bool navbar_collapsed = true;
			static float navbar_width = 0.f; navbar_width = ImLerp(navbar_width, navbar_collapsed ? 0.f : 1.f, 0.04f);
			content_anim = ImLerp(content_anim, 1.f, 0.04f);

			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !navbar_collapsed);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, content_anim * (navbar_width > 0.005f ? 1 / navbar_width / 2 : 1.f));
			ImGui::SetCursorPos({ 81, 25 });
			ImGui::BeginGroup();
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
			ImGui::Text("DemonWare");
			ImGui::PopFont();

			ImGui::BeginChild("main", { window->Size.x - 81, window->Size.y - window->DC.CursorPos.y + window->Pos.y }, 0, ImGuiWindowFlags_NoBackground);
			{
				switch (tab) {
				case 0: {
					ImGui::BeginChild("subtabs", { ImGui::GetWindowWidth() - 30, 40 }, 1);
					{
						ImGui::SetCursorPos({ 16, 0 });

						if (custom::subtab("Aimbot##subtab", subtab == 0)) subtab = 0; ImGui::SameLine(0, 20);
						if (custom::subtab("AimMode##subtab", subtab == 1)) subtab = 1; ImGui::SameLine(0, 20);

					}
					ImGui::EndChild();
					if (subtab == 0) {
						ImGui::BeginChild("main_child", { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 55 }, 0, ImGuiWindowFlags_NoBackground);
						{
							ImGui::BeginGroup();
							{
								custom::begin_child("Aimbot settings", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
								{
									ImGui::Checkbox("Aimbot##aimbot", &aimbot);
									ImGui::Checkbox("Memory Aim [HIGH RISK OF BAN]##aimbot", &memory);
									ImGui::Checkbox("Show FOV", &draw_fov);
									ImGui::Checkbox("Aim Line", &triggerbot);
									ImGui::SliderFloat("FOV", &fov, 15, 300, "%.2f");
									ImGui::SliderFloat("Smooth", &smoothing, 0.5f, 20.0f, "%.2f");
									//ImGui::ColorEdit4("Color Edit", col, ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_DisplayHex);
								}
								custom::end_child();
							}
							ImGui::EndGroup();
							ImGui::SameLine(0, 15);
							ImGui::BeginGroup();
							{
								custom::begin_child("Aimbot Settings", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
								{
									ImGui::Combo("Aimbone:", &AimboneVar, AimboneText, sizeof(AimboneText) / sizeof(*AimboneText));
									ImGui::Combo("Keybinds:", &AimKeyVar, AimKeysText, sizeof(AimKeysText) / sizeof(*AimKeysText));


								}
								custom::end_child();
								custom::begin_child("Aimbot Keybinds", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
								{


								}
								custom::end_child();
							}
							ImGui::EndGroup();
							ImGui::Spacing();
						}
						ImGui::EndChild();
					}
					if (subtab == 1) {
						ImGui::BeginChild("main_child1", { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 55 }, 0, ImGuiWindowFlags_NoBackground);
						{
							ImGui::BeginGroup();
							{
								custom::begin_child("Aim Configuration", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
								{
									//Triggerbot
									ImGui::Checkbox("Target Line", &targetline);
									ImGui::Checkbox("Target Circle", &targetcirlce);
									ImGui::Checkbox("Target Text", &targettex);



								}
								custom::end_child();
							}
							ImGui::EndGroup();
							ImGui::Spacing();
						}
						ImGui::EndChild();
					}

				} break;
				case 1: {
					ImGui::BeginChild("subtabs2", { ImGui::GetWindowWidth() - 30, 40 }, 1);
					{
						ImGui::SetCursorPos({ 16, 0 });

						if (custom::subtab("Visuals##subtab", subtab == 0)) subtab = 0; ImGui::SameLine(0, 20);
						if (custom::subtab("Player##subtab", subtab == 1)) subtab = 1; ImGui::SameLine(0, 20);

					}
					ImGui::EndChild();
					if (subtab == 0)
					{
						ImGui::BeginChild("main_child3", { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 55 }, 0, ImGuiWindowFlags_NoBackground);
						{
							ImGui::BeginGroup();
							{
								custom::begin_child("Visuals settings", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
								{
									ImGui::Checkbox("Enable ESP", &esp);
									if (esp == true)
									{
										ImGui::Combo("ESP Design:", &ESPStartType, ESPDesign, sizeof(ESPDesign) / sizeof(*ESPDesign));
										ImGui::Checkbox("Skeleton", &skeleton);
										ImGui::Checkbox("Line ESP", &line);
										ImGui::Checkbox("Head ESP", &headesp);
										ImGui::Checkbox("Outlined Esp", &Outline);

									}


									custom::end_child();
								}
								ImGui::EndGroup();
								ImGui::SameLine(0, 15);
								ImGui::BeginGroup();
								{
									custom::begin_child("Other", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
									{

										ImGui::SliderFloat(TEXT("Box Thickness"), &box_thickness, 0.f, 5);
										ImGui::SliderFloat(TEXT("Skeleton Thickness"), &skelthick, 0.f, 5);
										ImGui::SliderFloat(TEXT("Head Circle size"), &headcirclesize, 0.f, 50);
										ImGui::SliderFloat(TEXT("Render Distance"), &max_esp_distance, 1.0f, 500);
									}
									custom::end_child();
								}
								ImGui::EndGroup();
								ImGui::Spacing();
							}
							ImGui::EndChild();
						}

					}
					if (subtab == 1)
					{
						ImGui::BeginChild("main_child1", { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 55 }, 0, ImGuiWindowFlags_NoBackground);
						{
							ImGui::BeginGroup();
							{
								custom::begin_child("Player information", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
								{
									ImGui::Checkbox("Player Distance", &draw_distance);
									ImGui::Checkbox("Draw username", &draw_username);
									ImGui::Checkbox("Draw Platform", &draw_platform);

								}
								custom::end_child();
							}
							ImGui::EndGroup();
							ImGui::Spacing();
						}
						ImGui::EndChild();

					}
				} break;
				case 3: {
					ImGui::BeginChild("subtabs3", { ImGui::GetWindowWidth() - 30, 40 }, 1);
					{
						ImGui::SetCursorPos({ 16, 0 });

						if (custom::subtab("Colors##subtab", subtab == 0)) subtab = 0; ImGui::SameLine(0, 20);
					}
					ImGui::EndChild();
					ImGui::BeginChild("main_child4", { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 55 }, 0, ImGuiWindowFlags_NoBackground);
					{
						ImGui::BeginGroup();
						{
							custom::begin_child("Vis Colors", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
							{
								ImGui::Text(" boxes color:");
								ImGui::SameLine();
								ColorPicker("Box", PlayerColor::BoxVisible);

								ImGui::Text("Skeleton:");
								ImGui::SameLine();
								ColorPicker("vis Skelcol", PlayerColor::SkeletonVisible);

								ImGui::Text("Snapline:");
								ImGui::SameLine();
								ColorPicker("Snapline ", PlayerColor::SnaplineVisible);


								ImGui::Text("Top text:");
								ImGui::SameLine();
								ColorPicker("Top text ", PlayerColor::TopTextVisible);


								ImGui::Text("Bottom text:");
								ImGui::SameLine();
								ColorPicker("Bottom text ", PlayerColor::BottomTextVisible);


								custom::end_child();
							}
							ImGui::EndGroup();
							ImGui::SameLine(0, 15);
							ImGui::BeginGroup();
							{
								custom::begin_child("Not vis Colors", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
								{
									ImGui::Text("Box :");
									ImGui::SameLine();
									ColorPicker("Box ", PlayerColor::BoxNotVisible);

									ImGui::Text("Skeleton:");
									ImGui::SameLine();
									ColorPicker("Skeleton ", PlayerColor::SkeletonNotVisible);

									ImGui::Text("Snapline: ");
									ImGui::SameLine();
									ColorPicker("Snapline  ", PlayerColor::SnaplineNotVisible);

									ImGui::Text("Top Text :");
									ImGui::SameLine();
									ColorPicker("Top Text  ", PlayerColor::TopTextNotVisible);

									ImGui::Text("Button Text :");
									ImGui::SameLine();
									ColorPicker("Button Text", PlayerColor::BottomTextNotVisible);
								
									ImGui::Spacing();
								}
								custom::end_child();
								custom::begin_child("Unload cheat", { (ImGui::GetWindowWidth() - 30) / 2 - ImGui::GetStyle().ItemSpacing.x / 2, 0 });
								{
									if (ImGui::Button("Unhook")) exit(0);
								}
								custom::end_child();
							}
							ImGui::EndGroup();
							ImGui::Spacing();
						}
						ImGui::EndChild();
					}
				} break;
				}
			}
			ImGui::EndChild();

			ImGui::EndGroup();
			ImGui::PopStyleVar();
			ImGui::PopItemFlag();

			ImGui::SetCursorPos({ 0, 0 });

			ImGui::BeginChild("navbar", { 50 + 100 * navbar_width, window->Size.y }, 0, ImGuiWindowFlags_NoBackground);
			{

				ImGui::SetCursorPosY(87);

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 16 });
				if (custom::tab("A", "Aimbot", tab == 0)) tab = 0;
				if (custom::tab("B", "Visuals", tab == 1)) tab = 1;
				if (custom::tab("D", "Settings", tab == 3)) tab = 3;
				ImGui::PopStyleVar();
			}
			ImGui::EndChild();

			ImGui::SetCursorPos({ 41.5f + 100 * navbar_width, 47 });
			if (custom::collapse_button(navbar_collapsed)) navbar_collapsed = !navbar_collapsed;
		}
		ImGui::End();


	}
	else
	{
	}
}



bool aim;




MSG Message = { NULL };

__forceinline VOID directx_init()
{
	SPOOF_FUNC
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::p_object)))
		exit(3);

	ZeroMemory(&DirectX9Interface::p_params, sizeof(DirectX9Interface::p_params));
	DirectX9Interface::p_params.Windowed = TRUE;
	DirectX9Interface::p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	DirectX9Interface::p_params.hDeviceWindow = DirectX9Interface::hijacked_hwnd;
	DirectX9Interface::p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	DirectX9Interface::p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	DirectX9Interface::p_params.BackBufferWidth = screen_width;
	DirectX9Interface::p_params.BackBufferHeight = screen_height;
	DirectX9Interface::p_params.EnableAutoDepthStencil = TRUE;
	DirectX9Interface::p_params.AutoDepthStencilFormat = D3DFMT_D16;
	DirectX9Interface::p_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	if (FAILED(DirectX9Interface::p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, DirectX9Interface::hijacked_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &DirectX9Interface::p_params, 0, &DirectX9Interface::p_device)))
	{
		DirectX9Interface::p_object->Release();
		Sleep(1000);
		exit(1);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(DirectX9Interface::hijacked_hwnd);
	ImGui_ImplDX9_Init(DirectX9Interface::p_device);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	static const ImWchar icons_ranges[] = { 0xf000, 0xf5ff, 0 };
	ImFontConfig icons_config;

	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	ImFontConfig config;

	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 1;
	icons_config.OversampleV = 1;
	ImFontConfig CustomFont;
	CustomFont.FontDataOwnedByAtlas = false;

	 io.Fonts->AddFontFromFileTTF("C:\\font.otf", 20.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	ESPFont = io.Fonts->AddFontFromFileTTF("C:\\font.otf", 20.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	VantaFont = io.Fonts->AddFontFromFileTTF("C:\\vanta.ttf", 45.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	NoramlFOnt = io.Fonts->AddFontFromFileTTF("C:\\verdana.ttf", 15.0f, nullptr, io.Fonts->GetGlyphRangesDefault());


	DirectX9Interface::p_object->Release();
}





VOID render_loop()
{
	SPOOF_FUNC;
	static RECT old_rc;
	RtlSecureZeroMemory(&DirectX9Interface::message, sizeof(MSG));
	while (DirectX9Interface::message.message != WM_QUIT)
	{
		if (PeekMessageA(&DirectX9Interface::message, DirectX9Interface::hijacked_hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&DirectX9Interface::message);
			DispatchMessageA(&DirectX9Interface::message);
		}

		RECT rc;
		POINT xy;
		RtlSecureZeroMemory(&rc, sizeof(RECT));
		RtlSecureZeroMemory(&xy, sizeof(POINT));
		GetClientRect(DirectX9Interface::game_wnd, &rc);
		ClientToScreen(DirectX9Interface::game_wnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = DirectX9Interface::game_wnd;
		screen_width = io.DisplaySize.x;
		screen_height = io.DisplaySize.y;
		center_x = io.DisplaySize.x / 2;
		center_y = io.DisplaySize.y / 2;

		POINT p;
		GetCursorPosA_Spoofed(&p);
		long styleProc = GetWindowLongA_Spoofed(DirectX9Interface::hijacked_hwnd, GWL_EXSTYLE);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;
		if (menu_open)
		{
			styleProc &= ~WS_EX_LAYERED;
			SetWindowLongA_Spoofed(DirectX9Interface::hijacked_hwnd,
				GWL_EXSTYLE, styleProc);
			SetForegroundWindow(DirectX9Interface::hijacked_hwnd);
		}
		else
		{
			styleProc |= WS_EX_LAYERED;
			SetWindowLong(DirectX9Interface::hijacked_hwnd,
				GWL_EXSTYLE, styleProc);
		}
		if (GetAsyncKeyState_Spoofed(0x1))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
		{
			io.MouseDown[0] = false;
		}
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;
			DirectX9Interface::p_params.BackBufferWidth = screen_width;
			DirectX9Interface::p_params.BackBufferHeight = screen_height;
			SetWindowPos_Spoofed(DirectX9Interface::hijacked_hwnd, (HWND)0, xy.x, xy.y, screen_width, screen_height, SWP_NOREDRAW);
			DirectX9Interface::p_device->Reset(&DirectX9Interface::p_params);
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		
		ColorChange();
		gaybow;

		if (GetAsyncKeyState_Spoofed(VK_DELETE))
		{
			exit(0);
		}
		actor_loop();
		DrawMenu();
		ImGui::EndFrame();

		DirectX9Interface::p_device->SetRenderState(D3DRS_ZENABLE, false);
		DirectX9Interface::p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		DirectX9Interface::p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		DirectX9Interface::p_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (DirectX9Interface::p_device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			DirectX9Interface::p_device->EndScene();
		}
		HRESULT result = DirectX9Interface::p_device->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && DirectX9Interface::p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9Interface::p_device->Reset(&DirectX9Interface::p_params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}

	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9Interface::p_device != NULL)
	{
		DirectX9Interface::p_device->EndScene();
		DirectX9Interface::p_device->Release();
	}
	if (DirectX9Interface::p_object != NULL)
	{
		DirectX9Interface::p_object->Release();
	}
	DestroyWindow(DirectX9Interface::hijacked_hwnd);
}
HWND windowid = NULL;
int choice;
const char* folderPath = "C:\\pic";
#include <codecvt>
#include "shit.h"


BOOL anti_debugger()
{
	SPOOF_FUNC
	BOOL is_debugged = FALSE;
	HANDLE h_process = GetCurrentProcess();
	HANDLE h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	PROCESSENTRY32 process_entry = { 0 };
	process_entry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(h_snapshot, &process_entry);

	do
	{
		if (_stricmp(process_entry.szExeFile, "notepad.exe") == 0
			|| _stricmp(process_entry.szExeFile, "procexp.exe") == 0
			|| _stricmp(process_entry.szExeFile, "ollydbg.exe") == 0
			|| _stricmp(process_entry.szExeFile, "idaq.exe") == 0
			|| _stricmp(process_entry.szExeFile, "idaq64.exe") == 0
			|| _stricmp(process_entry.szExeFile, "UD.exe") == 0
			|| _stricmp(process_entry.szExeFile, "ImmunityDebugger.exe") == 0
			|| _stricmp(process_entry.szExeFile, "x64dbg.exe") == 0
			|| _stricmp(process_entry.szExeFile, "windbg.exe") == 0
			|| _stricmp(process_entry.szExeFile, "joeboxcontrol.exe") == 0)
		{
			is_debugged = TRUE;
			break;
		}
	} while (Process32Next(h_snapshot, &process_entry));

	CloseHandle(h_snapshot);
	if (is_debugged)
	{
		std::cout << "try me nigga.\n\n";
		Sleep(1000);
		TerminateProcess(h_process, 0);
		return TRUE;
	}

	return FALSE;
}




std::string data_dir = ("");
std::string temp_dir = ("");

namespace StringConverter {
	std::wstring ToUnicode(const std::string& str) {
		SPOOF_FUNC
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
	}

	std::string ToASCII(const std::wstring& wstr) {
		SPOOF_FUNC
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}
}


bool deleteFile(const std::string& filename) {
	if (std::remove(filename.c_str()) != 0) {
		SPOOF_FUNC
		// Error occurred while deleting the file
		return false;
	}
	return true;
}
std::string mappaPath;






int main()
{
	SPOOF_FUNC
	mouse_interface();


	system("cls");
	printf("\n  \033[33m(>)\033[0m");
	std::cout << ("Press WIN+H ingame");



	mem::find_driver();



	mem::process_id = mem::find_process("FortniteClient-Win64-Shipping.exe");
	while (windowid == NULL)

	{
		XorS(wind, "Fortnite  ");
		windowid = FindWindowA(0, wind.decrypt());

	}


	LI_FN(system)(E("curl https://cdn.discordapp.com/attachments/1146627990680240261/1146919247696625745/font.otf --silent -o C:\\font.otf"));


	while (!(GetAsyncKeyState_Spoofed(VK_LWIN) & 0x8000) || !(GetAsyncKeyState_Spoofed('H') & 0x8000)) {
	
		Sleep(100); 
	}

	mem::BaseAddy = mem::find_image();

	if (!mem::BaseAddy)
	{
		std::cout << _xor_(" No driver ");
	}

	std::thread(cache_players).detach();


	



	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	Beep(300, 1000);

	
	hijack::hijack_init();
	MouseController::Init();
	directx_init();
	render_loop();

	ErasePEHeaderFromMemory();

}
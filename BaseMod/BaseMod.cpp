#include "BaseMod.h"
#include <Windows.h>
#include <iostream>
#include <MinHook.h>
#include "globals.h"
#include "config.h"
#include "binutils.h"
#include <cstdint>

bool myCheckBox = false;
int counter = 0;
int font_size_x = 18;
int font_size_y = 18;
int trailing_spaces = 16;
char buf_1[32];
char buf_2[32];

uint32_t hk_jmp_fontupdate_lbputiteminfo = 0x49C60A;
uint32_t hk_ret_fontupdate_lbputiteminfo = 0x49C614;
uint32_t hk_dat_fontupdate_lbputiteminfo = 0xEE268E0;
void __declspec(naked) hk_fontupdate_lbputiteminfo() {
	__asm {
		push ecx
		mov ecx, font_size_x
		mov byte ptr[eax + 0x18], cl
		mov ecx, font_size_y
		mov byte ptr[eax + 0x19], cl
		pop ecx
		mov [eax+0x64], ecx
		cmp dword ptr [hk_dat_fontupdate_lbputiteminfo], 0xA
		jmp hk_ret_fontupdate_lbputiteminfo
	}
}

uint32_t hk_jmp_formatstring1_lbputiteminfo = 0x49C614;
uint32_t hk_ret_formatstring1_lbputiteminfo = 0x49C619;
void __declspec(naked) hk_formatstring1_lbputiteminfo() {
	__asm {
		mov ecx, offset buf_1
		jmp hk_ret_formatstring1_lbputiteminfo
	}
}

uint32_t hk_jmp_formatstring2_lbputiteminfo = 0x49C61B;
uint32_t hk_ret_formatstring2_lbputiteminfo = 0x49C620;
void __declspec(naked) hk_formatstring2_lbputiteminfo() {
	__asm {
		mov ecx, offset buf_2
		jmp hk_ret_formatstring2_lbputiteminfo
	}
}


//Will run once at game start, use it to setup hooks, offset address, initialize stuff...
void BaseMod::OnAttach() {

	sprintf_s(buf_1, "%%-%ds%%s%%2d(%%3d)/%%3d", trailing_spaces);
	sprintf_s(buf_2, "%%-%ds%%s%%2d(%%3d)/%%2d", trailing_spaces);

	hk_jmp_fontupdate_lbputiteminfo += mhfdll_addy;
	hk_ret_fontupdate_lbputiteminfo += mhfdll_addy;
	hk_dat_fontupdate_lbputiteminfo += mhfdll_addy;
	binutils::DetourJMP((void*)hk_jmp_fontupdate_lbputiteminfo, (void*)hk_fontupdate_lbputiteminfo, 5);

	hk_jmp_formatstring1_lbputiteminfo += mhfdll_addy;
	hk_ret_formatstring1_lbputiteminfo += mhfdll_addy;
	binutils::DetourJMP((void*)hk_jmp_formatstring1_lbputiteminfo, (void*)hk_formatstring1_lbputiteminfo, 0);

	hk_jmp_formatstring2_lbputiteminfo += mhfdll_addy;
	hk_ret_formatstring2_lbputiteminfo += mhfdll_addy;
	binutils::DetourJMP((void*)hk_jmp_formatstring2_lbputiteminfo, (void*)hk_formatstring2_lbputiteminfo, 0);
}

//Will run once at game end, loader doesn't use it for now
void BaseMod::OnDetach() {

}

//Will run once the ImGUI and Dx9 Context has been initialized, use this function to create texture data
void BaseMod::OnImGUIInit() {

}

//Has access to the main ImGui context, can draw anything related to the mod in there
void BaseMod::DrawModMenu() {
	if (ImGui::CollapsingHeader(DISPLAY_NAME.c_str())) {
		ImGui::SliderInt("Font Size X", &font_size_x, 1, 48);
		ImGui::SliderInt("Font Size Y", &font_size_y, 1, 48);
		if (ImGui::SliderInt("Material name spaces (default 16)", &trailing_spaces, 1, 48)) {
			sprintf_s(buf_1, "%%-%ds%%s%%2d(%%3d)/%%3d", trailing_spaces);
			sprintf_s(buf_2, "%%-%ds%%s%%2d(%%3d)/%%2d", trailing_spaces);
		}
		//ImGui::InputText("Format string 1", buf_1, 32);
		//ImGui::InputText("Format string 2", buf_2, 32);
	}
}

void BaseMod::DrawUI(bool show_menu) {

}

//TO DO: Will hook the main lobby update function and run once every game frame
void BaseMod::OnUpdateLobby() {

}

//Will hook the main quest update function and run once every game frame
void BaseMod::OnUpdateQuest() {
	counter++;
}

extern "C" {
	__declspec(dllexport) Mod* createMod() {
		return new BaseMod(NAME, DISPLAY_NAME, VERSION, REQUIRED_VERSION, HGE_ONLY);
	}
}
#include "BaseMod.h"
#include <Windows.h>
#include <iostream>
#include <MinHook.h>
#include "globals.h"
#include "config.h"
#include "binutils.h"
#include <cstdint>

typedef int (*fontprint_t)(char*, ...);
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

bool myCheckBox = false;
int counter = 0;
int font_size_x = 17;
int font_size_y = 18;
int trailing_spaces = 22;
char buf_1[32];
char buf_2[32];
uint8_t inventory_width = 10;
uint8_t size_min = 0;
uint8_t size_max = 48;
float og_window_offset = 640.0;
float additional_window_offset = 0.0;
int additional_qty_offset = 0;
char* display_log[10];
int cur_index = 0;
uint32_t font_struct = 0xE3CBD64;
uint32_t font_print_addy = 0xB70CF0;
bool renderCustomFont = false;

uint32_t inventory_size_addy_1 = 0x1A43256;
uint32_t inventory_size_addy_2 = 0x1A43276;

/// POS OFFSETS
u32 weapon_list_submenu1_zenny_x_offsetvalue = 0;
u32 weapon_list_submenu1_zenny_x_fulloffset = 178 + weapon_list_submenu1_zenny_x_offsetvalue;
u32 armor_list_submenu1_zenny_x_offsetvalue = 0;
u32 armor_list_submenu1_zenny_x_fulloffset = 178 + armor_list_submenu1_zenny_x_offsetvalue;

/// FONT SIZES
u8 weapon_list_submenu_1_x = 17;
u8 weapon_list_submenu_1_y = 17;
u8 gear_status_list_x = 16;
u8 gear_status_list_y = 16;
u8 gear_title_x = 18;
u8 gear_title_y = 18;
u8 gear_name_2_x = 20;
u8 gear_name_2_y = 20;
u8 smithy_main_menu_x = 20;
u8 smithy_main_menu_y = 20;
char gear_name_2_buf[32];
char material_2_buf[32];
int material_qty_pos_x = 32;

/// TRAILING SPACES
int weapon_name_trailing_spaces = 24;
int weapon_name_2_trailing_spaces = 20;
char weapon_list_submenu1_buf[32];

/// TEMPLATE STRING OFFSETS
u32 caravan_quest_cp_string = 0x199603C;
u32 weapon_list_submenu1_string = 0x199609C;
u32 material_list_qty_string_1 = 0x19964AC;
u32 material_list_qty_string_2 = 0x19964C0;
u32 gear_name_list_1_str = 0x1995B80;


uint8_t inventory_size_1; 
uint8_t inventory_size_2;

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

uint32_t hk_jmp_weapon_name_1_1 = 0x522CCD;
uint32_t hk_ret_weapon_name_1_1 = 0x522CD2;
void __declspec(naked) hk_weapon_name_1_1() {
	__asm {
		push offset weapon_list_submenu1_buf
		jmp hk_ret_weapon_name_1_1
	}
}

uint32_t hk_jmp_wndwoff_itemlistwindowx = 0x94D1AB;
uint32_t hk_ret_wndwoff_itemlistwindowx = 0x94D1B3;
void __declspec(naked) hk_wndwoff_itemlistwindowx() {
	__asm {
		subss xmm0, [og_window_offset]
		subss xmm0, [additional_window_offset]
		jmp hk_ret_wndwoff_itemlistwindowx
	}
}

uint32_t hk_jmp_qtyoff_itemlistwindowx = 0x94D9FB;
uint32_t hk_ret_qtyoff_itemlistwindowx = 0x94DA01;
void __declspec(naked) hk_qtyoff_itemlistwindowx() {
	__asm {
		add edx, 0x9E
		add edx, [additional_qty_offset]
		jmp hk_ret_qtyoff_itemlistwindowx
	}
}

uint32_t hk_jmp_qtyoff2_itemlistwindowx = 0x94DAEB;
uint32_t hk_ret_qtyoff2_itemlistwindowx = 0x94DAF0;
void __declspec(naked) hk_qtyoff2_itemlistwindowx() {
	__asm {
		add eax, 0x8C
		add eax, [additional_qty_offset]
		jmp hk_ret_qtyoff2_itemlistwindowx
	}
}

uint32_t hk_jmp_combineSelection = 0x943E8E;
uint32_t hk_ret_combineSelection = 0x943E93;
void __declspec(naked) hk_combineSelection() {
	__asm {
		mov eax, 0x140
		add eax, [additional_qty_offset]
		jmp hk_ret_combineSelection
	}
}

u32 jmp_gear_status_list_print_97A216 = 0x97A216;
u32 ret_gear_status_list_print_97A21C = 0x97A21C;
u32 status_or_guildcard = 0;
void __declspec(naked) hk_gear_status_list_print_97A216() {
	__asm {
		mov status_or_guildcard, ecx
		mov edi, edx
		xor edx, edx
		test ecx, ecx
		jmp ret_gear_status_list_print_97A21C
	}
}

u32 jmp_gear_status_list_print_97A76D = 0x97A76D;
u32 ret_gear_status_list_print_97A773 = 0x97A773;
void __declspec(naked) hk_gear_status_list_print_97A76D() {

	__asm {
		pushad
	}

	if (!status_or_guildcard) {
		__asm {
			popad
			push ecx
			movzx ecx, [gear_status_list_x]
			mov[eax + 0x18], cl
			movzx ecx, [gear_status_list_y]
			mov[eax + 0x19], cl
			pop ecx
			jmp ret_gear_status_list_print_97A773
		}
	}

	__asm {
		popad
		mov[eax + 0x18], bl
		mov[eax + 0x19], bl
		jmp ret_gear_status_list_print_97A773
	}
}

u32 jmp_gear_status_gear_details_9842B5 = 0x9842B5;
u32 ret_gear_status_gear_details_9842BF = 0x9842BF;
u32 mhfpac_ptr = 0xE77DCCC;
void __declspec(naked) hk_gear_status_gear_details_9842B5() {
	__asm {
		mov ecx, [font_struct]
		mov dl, [gear_title_x]
		mov byte ptr [ecx + 0x18], dl
		mov dl, [gear_title_y]
		mov byte ptr [ecx + 0x19], dl
		movzx ecx, byte ptr[ebp - 0x2D]
		mov edx, [mhfpac_ptr]
		mov edx, dword ptr[edx]
		jmp ret_gear_status_gear_details_9842BF
	}
}

u32 jmp_gear_status_gear_details_9842CF = 0x9842CF;
u32 ret_gear_status_gear_details_9842D5 = 0x9842D5;
void __declspec(naked) hk_gear_status_gear_details_9842CF() {
	__asm {
		push ecx
		push edx
		mov ecx, [font_struct]
		mov dl, [gear_title_x]
		mov byte ptr[ecx + 0x18], 0x12
		mov dl, [gear_title_y]
		mov byte ptr[ecx + 0x19], 0x12
		pop ecx
		pop edx
		mov al, [edi + 1]
		add esp, 0x1C
		jmp ret_gear_status_gear_details_9842D5
	}
}

uint32_t hk_jmp_combineSelection_confirm = 0x9447C1;
uint32_t hk_ret_combineSelection_confirm = 0x9447C9;
void __declspec(naked) hk_combineSelection_confirm() {
	__asm {
		subss xmm0, [og_window_offset]
		subss xmm0, [additional_window_offset]
		jmp hk_ret_combineSelection_confirm
	}
}

u32 jmp_sub_49C680_49C796 = 0x49C796;
u32 ret_sub_49C680_49C79C = 0x49C79C;
void __declspec(naked) hk_sub_49C680_49C796() {
	__asm {
		add edx, 0x90
		add edx, material_qty_pos_x
		jmp ret_sub_49C680_49C79C
	}
}

u32 jmp_sub_49C680_49C72C = 0x49C72C;
u32 ret_sub_49C680_49C734 = 0x49C734;
void __declspec(naked) hk_sub_49C680_49C72C() {
	__asm {
		mov cx, [ebp + 0x0C]
		mov[eax + 0x0E], cx
		push ecx
		mov ecx, font_size_x
		mov [eax+0x18], cl
		mov ecx, font_size_y
		mov [eax+0x19], cl
		pop ecx
		jmp ret_sub_49C680_49C734
	}
}

u32 jmp_sub_521DA0_521F2A = 0x521F2A;
u32 ret_sub_521DA0_521F30 = 0x521F30;
void __declspec(naked) hk_sub_521DA0_521F2A() {
	__asm {
		push ecx
		mov cl, gear_name_2_x
		mov[eax + 0x18], cl
		mov cl, gear_name_2_y
		mov[eax + 0x19], cl
		pop ecx
		jmp ret_sub_521DA0_521F30
	}
}

u32 jmp_sub_706560_7065A8 = 0x7065A8;
u32 ret_sub_706560_7065AE = 0x7065AE;
void __declspec(naked) hk_sub_706560_7065A8() {
	__asm {
		push ecx
		mov cl, smithy_main_menu_x
		mov[eax + 0x18], cl
		mov cl, smithy_main_menu_y
		mov[eax + 0x19], cl
		pop ecx
		jmp ret_sub_706560_7065AE
	}
}

//Fix Premium Color Fix
u32 jmp_sub_522620_522BB3 = 0x522BB3;
u32 ret_sub_522620_522BBD = 0x522BBD;
void __declspec(naked) hk_sub_522620_522BB3() {
	__asm {
		mov dword ptr[eax + 0x207D0], 0xFF846B5C
		jmp ret_sub_522620_522BBD
	}
}

u32 jmp_sub_522620_522BBF = 0x522BBF;
u32 ret_sub_522620_522BC9 = 0x522BC9;
void __declspec(naked) hk_sub_522620_522BBF() {
	__asm {
		mov dword ptr[eax + 0x207D0], 0xFF202020
		jmp ret_sub_522620_522BC9
	}
}

//Window 2
u32 jmp_sub_521DA0_5222FB = 0x5222FB;
u32 ret_sub_521DA0_522305 = 0x522305;
void __declspec(naked) hk_sub_521DA0_5222FB() {
	__asm {
		mov dword ptr[eax + 0x207D0], 0xFF846B5C
		jmp ret_sub_521DA0_522305
	}
}

u32 jmp_sub_521DA0_522307 = 0x522307;
u32 ret_sub_521DA0_522311 = 0x522311;
void __declspec(naked) hk_sub_521DA0_522307() {
	__asm {
		mov dword ptr[eax + 0x207D0], 0xFF202020
		jmp ret_sub_521DA0_522311
	}
}

uint32_t combine_window_off_addy = 0x1A43158;
uint32_t combine_float_off_addy = 0x19B68E8;

void print_stuff(char* string, short xpos, short ypos) {
	fontprint_t func = (fontprint_t)font_print_addy;
	*(short*)(font_struct + 12) = xpos;
	*(short*)(font_struct + 14) = ypos;
	*(short*)(font_struct + 24) = 0x1212;
	func(string);
}

void copy_sjis_string_with_width(char* dest, const char* src) {
	while (*src) {
		/*if ((unsigned char)*src == 0x20) {
			// Convert half-width space to full-width space in SJIS
			*dest++ = 0x81;
			*dest++ = 0x40;
			src++;
		}*/
		//If we find a full-width space 0x8140
		if (((unsigned char)*src >= 0x81 && (unsigned char)*src <= 0x9F) || ((unsigned char)*src >= 0xE0 && (unsigned char)*src <= 0xFC)) {
			*dest++ = *src++;
			if (*src) *dest++ = *src++;
		}
		else {
			*dest++ = *src++;
		}
	}
	*dest = '\0'; 
}

//han2zen patch
char* han2zen_dest;
char* han2zen_src;
uint32_t hk_jmp_han2zen = 0x14DF611;
uint32_t hk_ret_han2zen = 0x14DF631;
void __declspec(naked) hk_han2zen() {
	__asm {
		mov ebp, esp
		mov han2zen_dest, eax
		push ecx
		mov ecx, [ebp+8]
		mov han2zen_src, ecx
		pop ecx
		pushad
	}

	copy_sjis_string_with_width(han2zen_dest, han2zen_src);

	__asm {
		popad
		jmp hk_ret_han2zen
	}
}

u32 jmp_sub_521DA0_5224C7 = 0x5224C7;
u32 ret_sub_521DA0_5224CC = 0x5224CC;
void __declspec(naked) hk_sub_521DA0_5224C7() {
	__asm {
		push offset gear_name_2_buf
		jmp ret_sub_521DA0_5224CC
	}
}

//Smithy > create armor > armor list name
u32 jmp_sub_537850_537914 = 0x537914;
u32 ret_sub_537850_53791A = 0x53791A;
u8 armor_list_name_font_x = 0x16;
u8 armor_list_name_font_y = 0x11;
void __declspec(naked) hk_sub_537850_537914() {
	__asm {
		mov word ptr[eax + 0x18], 0x1111
		push ecx
		mov cl, armor_list_name_font_x
		mov byte ptr[eax + 0x18], cl
		mov cl, armor_list_name_font_y
		mov byte ptr[eax + 0x19], cl
		pop ecx
		jmp ret_sub_537850_53791A
	}
}

//Smithy > create armor > menu 2
u32 jmp_sub_536F80_537103 = 0x537103;
u32 ret_sub_536F80_537109 = 0x537109;
u8 armor_list_2_font_x = 0x8;
u8 armor_list_2_font_y = 0x14;
void __declspec(naked) hk_sub_536F80_537103() {
	__asm {
		mov word ptr[eax + 0x18], 0x1414
		push ecx
		mov cl, armor_list_2_font_x
		mov byte ptr[eax + 0x18], cl
		mov cl, armor_list_2_font_y
		mov byte ptr[eax + 0x19], cl
		pop ecx
		jmp ret_sub_536F80_537109
	}
}

u32 jmp_sub_536F80_53770A = 0x53770A;
u32 ret_sub_536F80_53770F = 0x53770F;
int armor_list_2_spaces = 20;
char armor_list_2_space_buf[32] = "%-56s %6d%s";
void __declspec(naked) hk_sub_536F80_53770A() {
	__asm {
		push offset armor_list_2_space_buf
		jmp ret_sub_536F80_53770F
	}
}

//Skills deco
u32 jmp_sub_973A50_973BC9 = 0x973BC9;
u32 ret_sub_973A50_973BCF = 0x973BCF;
u8 skill_font_x = 0x12;
u8 skill_font_y = 0x0F;
void __declspec(naked) hk_sub_973A50_973BC9() {
	__asm {
		push ecx
		mov word ptr[esi + 0x18], 0x120F
		mov cl, skill_font_x
		mov byte ptr[esi + 0x18], cl
		mov cl, skill_font_y
		mov byte ptr[esi + 0x19], cl
		pop ecx
		jmp ret_sub_973A50_973BCF
	}
}

//Will run once at game start, use it to setup hooks, offset address, initialize stuff...
void BaseMod::OnAttach() {

	sprintf_s(buf_1, "%%-%ds%%s%%2d(%%3d)/%%3d", trailing_spaces);
	sprintf_s(buf_2, "%%-%ds%%s%%2d(%%3d)/%%2d", trailing_spaces);
	sprintf_s(gear_name_2_buf, "%%-%ds %%6d%%s", weapon_name_2_trailing_spaces);
	sprintf_s(weapon_list_submenu1_buf, "%%-%ds", weapon_name_trailing_spaces);
	sprintf_s(armor_list_2_space_buf, "%%-%ds %%6d%%s", armor_list_2_spaces);
	font_struct = *(u32*)(mhfdll_addy+font_struct);

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

	hk_jmp_weapon_name_1_1 += mhfdll_addy;
	hk_ret_weapon_name_1_1 += mhfdll_addy;
	binutils::DetourJMP((void*)hk_jmp_weapon_name_1_1, (void*)hk_weapon_name_1_1, 0);

	inventory_size_addy_1 += mhfdll_addy;
	inventory_size_addy_2 += mhfdll_addy;

	hk_jmp_wndwoff_itemlistwindowx += mhfdll_addy;
	hk_ret_wndwoff_itemlistwindowx += mhfdll_addy;
	binutils::DetourJMP((void*)hk_jmp_wndwoff_itemlistwindowx, (void*)hk_wndwoff_itemlistwindowx, 3);

	hk_jmp_qtyoff_itemlistwindowx += mhfdll_addy;
	hk_ret_qtyoff_itemlistwindowx += mhfdll_addy;
	binutils::DetourJMP((void*)hk_jmp_qtyoff_itemlistwindowx, (void*)hk_qtyoff_itemlistwindowx, 1);

	hk_jmp_qtyoff2_itemlistwindowx += mhfdll_addy;
	hk_ret_qtyoff2_itemlistwindowx += mhfdll_addy;
	binutils::DetourJMP((void*)hk_jmp_qtyoff2_itemlistwindowx, (void*)hk_qtyoff2_itemlistwindowx, 0);

	hk_jmp_han2zen += mhfdll_addy;
	hk_ret_han2zen += mhfdll_addy;
	binutils::DetourJMP((void*)hk_jmp_han2zen, (void*)hk_han2zen, 3);

	hk_jmp_combineSelection += mhfdll_addy;
	hk_ret_combineSelection += mhfdll_addy;
	binutils::DetourJMP((void*)hk_jmp_combineSelection, (void*)hk_combineSelection, 0);

	hk_jmp_combineSelection_confirm += mhfdll_addy;
	hk_ret_combineSelection_confirm += mhfdll_addy;
	binutils::DetourJMP((void*)hk_jmp_combineSelection_confirm, (void*)hk_combineSelection_confirm, 3);

	jmp_gear_status_list_print_97A216 += mhfdll_addy;
	ret_gear_status_list_print_97A21C += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_gear_status_list_print_97A216, (void*)hk_gear_status_list_print_97A216, 1);

	jmp_gear_status_list_print_97A76D += mhfdll_addy;
	ret_gear_status_list_print_97A773 += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_gear_status_list_print_97A76D, (void*)hk_gear_status_list_print_97A76D, 1);

	jmp_gear_status_gear_details_9842B5 += mhfdll_addy;
	ret_gear_status_gear_details_9842BF += mhfdll_addy;
	mhfpac_ptr += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_gear_status_gear_details_9842B5, (void*)hk_gear_status_gear_details_9842B5, 5);

	jmp_gear_status_gear_details_9842CF += mhfdll_addy;
	ret_gear_status_gear_details_9842D5 += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_gear_status_gear_details_9842CF, (void*)hk_gear_status_gear_details_9842CF, 1);

	jmp_sub_49C680_49C796 += mhfdll_addy;
	ret_sub_49C680_49C79C += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_49C680_49C796, (void*)hk_sub_49C680_49C796, 1);

	jmp_sub_49C680_49C72C += mhfdll_addy;
	ret_sub_49C680_49C734 += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_49C680_49C72C, (void*)hk_sub_49C680_49C72C, 3);

	jmp_sub_521DA0_521F2A += mhfdll_addy;
	ret_sub_521DA0_521F30 += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_521DA0_521F2A, (void*)hk_sub_521DA0_521F2A, 1);

	jmp_sub_521DA0_5224C7 += mhfdll_addy;
	ret_sub_521DA0_5224CC += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_521DA0_5224C7, (void*)hk_sub_521DA0_5224C7, 0);

	jmp_sub_706560_7065A8 += mhfdll_addy;
	ret_sub_706560_7065AE += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_706560_7065A8, (void*)hk_sub_706560_7065A8, 1);

	jmp_sub_522620_522BB3 += mhfdll_addy;
	ret_sub_522620_522BBD += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_522620_522BB3, (void*)hk_sub_522620_522BB3, 5);

	jmp_sub_522620_522BBF += mhfdll_addy;
	ret_sub_522620_522BC9 += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_522620_522BBF, (void*)hk_sub_522620_522BBF, 5);

	jmp_sub_521DA0_5222FB += mhfdll_addy;
	ret_sub_521DA0_522305 += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_521DA0_5222FB, (void*)hk_sub_521DA0_5222FB, 5);

	jmp_sub_521DA0_522307 += mhfdll_addy;
	ret_sub_521DA0_522311 += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_521DA0_522307, (void*)hk_sub_521DA0_522307, 5);

	combine_window_off_addy += mhfdll_addy;
	combine_float_off_addy += mhfdll_addy;

	caravan_quest_cp_string += mhfdll_addy;
	material_list_qty_string_1 += mhfdll_addy;
	material_list_qty_string_2 += mhfdll_addy;
	sprintf((char*)caravan_quest_cp_string, "%%7s");
	sprintf((char*)material_list_qty_string_1, "%%s%%2d(%%3d)/%%3d");
	sprintf((char*)material_list_qty_string_2, "%%s%%2d(%%3d)/%%2d");
	//sprintf((char*)gear_name_list_1_str, "%%-%ds %%6d%%s", weapon_name_2_trailing_spaces);
	
	//Patching first window gear list font size
	binutils::WriteBytes((void*)(mhfdll_addy + 0x5226E8), &weapon_list_submenu_1_y, 1);
	binutils::WriteBytes((void*)(mhfdll_addy + 0x5226E9), &weapon_list_submenu_1_x, 1);
	//Offset menu 1
	binutils::WriteBytes((void*)(mhfdll_addy + 0x522CEC), &weapon_list_submenu1_zenny_x_fulloffset, 4);
	//Armor zenny offset
	binutils::WriteBytes((void*)(mhfdll_addy + 0x537F97), &armor_list_submenu1_zenny_x_fulloffset, 4);

	//Armor menu 1 font
	jmp_sub_536F80_537103 += mhfdll_addy;
	ret_sub_536F80_537109 += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_536F80_537103, (void*)hk_sub_536F80_537103, 1);

	//Armor menu 2 font
	jmp_sub_537850_537914 += mhfdll_addy;
	ret_sub_537850_53791A += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_537850_537914, (void*)hk_sub_537850_537914, 1);

	//Armor menu 2 space
	jmp_sub_536F80_53770A += mhfdll_addy;
	ret_sub_536F80_53770F += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_536F80_53770A, (void*)hk_sub_536F80_53770A, 0);

	//Skill deco font
	jmp_sub_973A50_973BC9 += mhfdll_addy;
	ret_sub_973A50_973BCF += mhfdll_addy;
	binutils::DetourJMP((void*)jmp_sub_973A50_973BC9, (void*)hk_sub_973A50_973BC9, 1);
}


//Will run once at game end, loader doesn't use it for now
void BaseMod::OnDetach() {

}

//Will run once the ImGUI and Dx9 Context has been initialized, use this function to create texture data
void BaseMod::OnImGUIInit() {

}

bool is_duplicate(char* str) {
	for (int i = 0; i < 10; i++) {
		if (strlen(display_log[i]) > 0) {
			if (strcmp(display_log[i], str)) {
				return true;
			}
		}
	}
	return false;
}

//Has access to the main ImGui context, can draw anything related to the mod in there
void BaseMod::DrawModMenu() {
	if (ImGui::CollapsingHeader(DISPLAY_NAME.c_str())) {
		ImGui::Text("Smithy Params");
		ImGui::SliderInt("Font Size X", &font_size_x, 1, 48);
		ImGui::SliderInt("Font Size Y", &font_size_y, 1, 48);
		if (ImGui::SliderInt("Material name spaces (default 16)", &trailing_spaces, 1, 48)) {
			sprintf_s(buf_1, "%%-%ds%%s%%2d(%%3d)/%%3d", trailing_spaces);
			sprintf_s(buf_2, "%%-%ds%%s%%2d(%%3d)/%%2d", trailing_spaces);
		}
		ImGui::Text("Smithy Craft List Menu 1");
		if (ImGui::SliderScalar("Weapon Name Size X", ImGuiDataType_U8, &weapon_list_submenu_1_y, &size_min, &size_max)) {
			binutils::WriteBytes((void*)(mhfdll_addy + 0x5226E8), &weapon_list_submenu_1_y, 1);
		};
		if(ImGui::SliderScalar("Weapon Name Size Y", ImGuiDataType_U8, &weapon_list_submenu_1_x, &size_min, &size_max)) {
			binutils::WriteBytes((void*)(mhfdll_addy + 0x5226E9), &weapon_list_submenu_1_x, 1);
		};
		if (ImGui::SliderScalar("Zenny Position X", ImGuiDataType_U8, &weapon_list_submenu1_zenny_x_offsetvalue, &size_min, &size_max)) {
			weapon_list_submenu1_zenny_x_fulloffset = 178 + weapon_list_submenu1_zenny_x_offsetvalue;
			binutils::WriteBytes((void*)(mhfdll_addy + 0x522CEC), &weapon_list_submenu1_zenny_x_fulloffset, 4);
		};

		ImGui::Text("Smithy Material Details");
		ImGui::SliderInt("Quantity Pos X", &material_qty_pos_x, 0, 150);


		ImGui::Text("Smithy Craft List Menu 2");
		if (ImGui::SliderInt("Gear Name Spaces", &weapon_name_2_trailing_spaces, 1, 48)) {
			sprintf_s(gear_name_2_buf, "%%-%ds %%6d%%s", weapon_name_2_trailing_spaces);
		}
		ImGui::SliderScalar("Gear Name Font X", ImGuiDataType_U8, &gear_name_2_x, &size_min, &size_max);
		ImGui::SliderScalar("Gear Name Font Y", ImGuiDataType_U8, &gear_name_2_y, &size_min, &size_max);

		ImGui::Text("Smithy > Armor > Craft Armor 1");
		if (ImGui::SliderScalar("Armor Zenny Position X", ImGuiDataType_U8, &armor_list_submenu1_zenny_x_offsetvalue, &size_min, &size_max)) {
			armor_list_submenu1_zenny_x_fulloffset = 178 + armor_list_submenu1_zenny_x_offsetvalue;
			binutils::WriteBytes((void*)(mhfdll_addy + 0x537F97), &armor_list_submenu1_zenny_x_fulloffset, 4);
		};
		ImGui::SliderScalar("Armor Name Font X", ImGuiDataType_U8, &armor_list_name_font_x, &size_min, &size_max);
		ImGui::SliderScalar("Armor Name Font Y", ImGuiDataType_U8, &armor_list_name_font_y, &size_min, &size_max);

		ImGui::Text("Smithy > Armor > Craft armor 2");
		if (ImGui::SliderInt("Armor name spaces", &armor_list_2_spaces, 1, 48)) {
			sprintf_s(armor_list_2_space_buf, "%%-%ds %%6d%%s", armor_list_2_spaces);
		}
		ImGui::SliderScalar("Armor 2 Name Font X", ImGuiDataType_U8, &armor_list_2_font_x, &size_min, &size_max);
		ImGui::SliderScalar("Armor 2 Name Font Y", ImGuiDataType_U8, &armor_list_2_font_y, &size_min, &size_max);

		ImGui::Text("Equipment Status > List");
		ImGui::SliderScalar("Name Font X", ImGuiDataType_U8, &gear_status_list_x, &size_min, &size_max);
		ImGui::SliderScalar("Name Font Y", ImGuiDataType_U8, &gear_status_list_y, &size_min, &size_max);

		ImGui::Text("Equipment Status > Details Panel > Gear Name");
		ImGui::SliderScalar("Title Font X", ImGuiDataType_U8, &gear_title_x, &size_min, &size_max);
		ImGui::SliderScalar("Title Font Y", ImGuiDataType_U8, &gear_title_y, &size_min, &size_max);

		ImGui::Text("Smithy/Store/Combiner... > Main Menus");
		ImGui::SliderScalar("Menu Font X", ImGuiDataType_U8, &smithy_main_menu_x, &size_min, &size_max);
		ImGui::SliderScalar("Menu Font Y", ImGuiDataType_U8, &smithy_main_menu_y, &size_min, &size_max);

		ImGui::Text("Smithy/Cat Smithy... > Deco/Cuff > Skill names");
		ImGui::SliderScalar("Skill Font X", ImGuiDataType_U8, &skill_font_x, &size_min, &size_max);
		ImGui::SliderScalar("Skill Font Y", ImGuiDataType_U8, &skill_font_y, &size_min, &size_max);


		ImGui::Text("Inventory Params");
		if (ImGui::SliderScalar("Inventory Width", ImGuiDataType_U8, &inventory_width, &size_min, &size_max)) {
			binutils::WriteBytes((void*)inventory_size_addy_1, &inventory_width, 1);
			binutils::WriteBytes((void*)inventory_size_addy_2, &inventory_width, 1);
			int offset_from_og = inventory_width - 10;
			int additional_offset = (offset_from_og * 18) + 1;
			additional_window_offset = additional_offset;
			additional_qty_offset = additional_offset;
			*(float*)(combine_float_off_addy) = 270.0 - (float)additional_offset;
			*(short*)(combine_window_off_addy) = 270 - (short)additional_offset;
		}
		if (ImGui::Button("Print Stuff")) {
			print_stuff((char*)"Hello world", 1250, 476);
		}
		//ImGui::Checkbox("Render Custom Font", &renderCustomFont);
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
}

extern "C" {
	__declspec(dllexport) Mod* createMod() {
		return new BaseMod(NAME, DISPLAY_NAME, VERSION, REQUIRED_VERSION, HGE_ONLY);
	}
}
#include "header.h"

namespace string_tables
{
	char* unit_names[ 0x50 ]; // replaces table at 0x004E2640
	char* unit_desc[ 0x50 ]; // replaces table at 0x004E2780
	char* item_name[ 0x40 ]; // replaces table at 0x004E28C0
	char* item_desc[ 0x3A ]; // replaces table at 0x004E29C0
	char* battle_result[ 0x3A ]; // replaces table at 0x004E2AA8
	char* mp_army_name[ 0x1F ]; // replaces table at 0x004E2B90
	char* objectives[ 0x35 ];	// replaces table at 0x004E2C0C
	char* tip_of_the_day[ 0x3A ]; // replaces table at 0x004E2CE0 

	void Load()
	{
		memcpy( unit_names, (void*)0x004E2640, 0x50 * sizeof( char* ) );
		*((DWORD*)0x00409EC2) = (DWORD)unit_names;
		*((DWORD*)0x0040B876) = (DWORD)unit_names;
		*((DWORD*)0x0040BE4A) = (DWORD)unit_names;
		*((DWORD*)0x0040C20F) = (DWORD)unit_names;
		*((DWORD*)0x0040C461) = (DWORD)unit_names;
		*((DWORD*)0x0040D213) = (DWORD)unit_names;
		*((DWORD*)0x0040D9C3) = (DWORD)unit_names;
		*((DWORD*)0x0041985C) = (DWORD)unit_names;
		*((DWORD*)0x0041A5E4) = (DWORD)unit_names;
		*((DWORD*)0x0041B1B0) = (DWORD)unit_names;
		*((DWORD*)0x0041B4CA) = (DWORD)unit_names;
		*((DWORD*)0x0042671A) = (DWORD)unit_names;
		*((DWORD*)0x004286E1) = (DWORD)unit_names;

		memcpy( unit_desc, (void*)0x004E2780, 0x50 * sizeof( char* ) );
		*((DWORD*)0x00409E63) = (DWORD)unit_desc; 
		*((DWORD*)0x0040BDE0) = (DWORD)unit_desc;

		memcpy( item_name, (void*)0x004E28C0, 0x40 * sizeof( char* ) );
		*((DWORD*)0x0040A7AD) = (DWORD)item_name;
		*((DWORD*)0x0040AFB0) = (DWORD)item_name;
		*((DWORD*)0x0040BFEE) = (DWORD)item_name;
		*((DWORD*)0x0040C08B) = (DWORD)item_name;
		*((DWORD*)0x0040C27F) = (DWORD)item_name;
		*((DWORD*)0x0040C4D1) = (DWORD)item_name;
		*((DWORD*)0x0040D834) = (DWORD)item_name;
		*((DWORD*)0x0040E016) = (DWORD)item_name;
		*((DWORD*)0x0040E16D) = (DWORD)item_name;
		*((DWORD*)0x00452899) = (DWORD)item_name;
		*((DWORD*)0x0045B79E) = (DWORD)item_name;

		memcpy( item_desc, (void*)0x004E29C0, 0x3A * sizeof( char* ) );
		*((DWORD*)0x0040C2BB) = (DWORD)item_desc;
		*((DWORD*)0x0040A840) = (DWORD)item_desc;
		*((DWORD*)0x0040C74B) = (DWORD)item_desc;

		memcpy( battle_result, (void*)0x004E2AA8, 0x3A * sizeof( char* ) );
		*((DWORD*)0x00418A75) = (DWORD)battle_result;
		*((DWORD*)0x00419E5F) = (DWORD)battle_result;

		memcpy( mp_army_name, (void*)0x004E2B90, 0x1F * sizeof( char* ) );
		*((DWORD*)0x0041B416) = (DWORD)mp_army_name;
		*((DWORD*)0x0041B0FC) = (DWORD)mp_army_name;
		*((DWORD*)0x0042795D) = (DWORD)mp_army_name;

		memcpy( objectives, (void*)0x004E2C0C, 0x35 * sizeof( char* ) );
		*((DWORD*)0x0041946D) = (DWORD)objectives;

		memcpy( tip_of_the_day, (void*)0x004E2CE0, 0x3A * sizeof( char* ) );
		*((DWORD*)0x00419BB7) = (DWORD)tip_of_the_day;

	}
	
	void Unload(){
		*((DWORD*)0x00409EC2) = 0x004E2640; // unit names
		*((DWORD*)0x0040B876) = 0x004E2640; // unit names
		*((DWORD*)0x0040BE4A) = 0x004E2640; // unit names
		*((DWORD*)0x0040C20F) = 0x004E2640; // unit names
		*((DWORD*)0x0040C461) = 0x004E2640; // unit names
		*((DWORD*)0x0040D213) = 0x004E2640; // unit names
		*((DWORD*)0x0040D9C3) = 0x004E2640; // unit names
		*((DWORD*)0x0041985C) = 0x004E2640; // unit names
		*((DWORD*)0x0041A5E4) = 0x004E2640; // unit names
		*((DWORD*)0x0041B1B0) = 0x004E2640; // unit names
		*((DWORD*)0x0041B4CA) = 0x004E2640; // unit names
		*((DWORD*)0x0042671A) = 0x004E2640; // unit names
		*((DWORD*)0x004286E1) = 0x004E2640; // unit names

		*((DWORD*)0x00409E63) = 0x004E2780; // unit_desc
		*((DWORD*)0x0040BDE0) = 0x004E2780; // unit_desc

		*((DWORD*)0x0040A7AD) = 0x004E28C0; // item_name
		*((DWORD*)0x0040AFB0) = 0x004E28C0; // item_name
		*((DWORD*)0x0040BFEE) = 0x004E28C0; // item_name
		*((DWORD*)0x0040C08B) = 0x004E28C0; // item_name
		*((DWORD*)0x0040C27F) = 0x004E28C0; // item_name
		*((DWORD*)0x0040C4D1) = 0x004E28C0; // item_name
		*((DWORD*)0x0040D834) = 0x004E28C0; // item_name
		*((DWORD*)0x0040E016) = 0x004E28C0; // item_name
		*((DWORD*)0x0040E16D) = 0x004E28C0; // item_name
		*((DWORD*)0x00452899) = 0x004E28C0; // item_name
		*((DWORD*)0x0045B79E) = 0x004E28C0; // item_name

		*((DWORD*)0x0040C2BB) = 0x004E29C0; // item_desc;
		*((DWORD*)0x0040A840) = 0x004E29C0; // item_desc;
		*((DWORD*)0x0040C74B) = 0x004E29C0; // item_desc;

		*((DWORD*)0x00418A75) = 0x004E2AA8; // battle_result;
		*((DWORD*)0x00419E5F) = 0x004E2AA8; // battle_result;

		*((DWORD*)0x0041B416) = 0x004E2B90; // mp_army_name
		*((DWORD*)0x0041B0FC) = 0x004E2B90; // mp_army_name
		*((DWORD*)0x0042795D) = 0x004E2B90; // mp_army_name

		*((DWORD*)0x0041946D) = 0x004E2C0C; // objectives

		*((DWORD*)0x00419BB7) = 0x004E2CE0; // )tip_of_the_day
	}
}

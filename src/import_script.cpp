#include <windows.h>

////// delete this if your compiler doesn't suck //////
namespace my_string
{
	int memcmp( const void* pvBuf1, const void* pvBuf2, size_t cnt ){
		unsigned char* buf1 = (unsigned char*)pvBuf1;
		unsigned char* buf2 = (unsigned char*)pvBuf2;
		int result = 0;
		while( buf1 != ((unsigned char*)pvBuf1) + cnt) if(result = *buf1++ - *buf2++) break;
		return result;
	}
	void* memcpy( void* pvDst, const void* pvSrc, size_t cnt ){
		unsigned char* dst = (unsigned char*)pvDst;
		unsigned char* src = (unsigned char*)pvSrc;
		while(src != ((unsigned char*)pvSrc + cnt)) *dst++ = *src++;
		return pvDst;
	}
}
#define memcmp    my_string::memcmp
#define memcpy    my_string::memcpy
//////////////////////////////////////////////////////////

#define LPS( x ) sizeof(x) - 1, x
#define WRITE_CODE( dw ) if( cp < &code[_countof(code) - 1] ) *cp++ = dw;

#define SPACE			0x20
#define TAB				0x09

struct LABEL_ENTRY
{
	int name_len;
	char* name;
	DWORD* addr;
	LABEL_ENTRY* next;
};

struct SCRIPT_OP { 
	uintptr_t address;
	int name_len;
	char* name;
	int arg_cnt;
	char* arg_type;
};

void* micro_alloc( int size );
void micro_alloc_destroy();
BOOL my_atoi( char* str, int len, DWORD* pdwResult );
char* ReadInTextFile( char* szFileName );
void __cdecl Log( char* fmt, ...);
SCRIPT_OP* in_command_set( const char *str, unsigned int len );
int GetTok( char*& str );
BOOL ImportScript( char* szFileName );


LABEL_ENTRY* jmps = NULL;
LABEL_ENTRY* targets = NULL;
DWORD code[ 0x8000 ]; // 128KB should be enough
uintptr_t var_base = 0x00537FF0;
DWORD* EP_StartScript;
DWORD* EP_EndScript;
DWORD* EP_NewGame;
DWORD* EP_Tutorial;
DWORD* EP_TheLivingGem;
DWORD* EP_HandyMan;
DWORD* EP_RideTheSrorm;
DWORD* EP_YouAndI;

// using string litterals for arg_type_id because I'm not using C++11
// and doing it with a seperate explicit structure would annoy me.
/*
// arg type identifers
#define END_OF_ARGS		0x00
#define INT				0x01
#define TEXT			0x02
#define JMP				0x03 // goto elsewhere
#define VAR				0x04 
#define CMP				0x05 // equals, etc.
#define VAR_ARG			0x06 // Speak()
#define LABEL			0x07 // goto here
#define UNKNOWN			0xFF // don't know; placeholder
*/
SCRIPT_OP ops[] = {
	{ 0x00000000, LPS( "LABEL" ), LPS( "\x07" ) },
	//
	{ 0x0041D280, LPS( "AddBitmap" ), LPS( "\xFF" ) }, // unknown
	{ 0x0041D620, LPS( "AddCash" ), LPS( "\x01" ) },
	{ 0x0041D640, LPS( "AddMagic" ), LPS( "\x01" ) },
	{ 0x0041CE80, LPS( "AddUnit" ), LPS( "\x01" ) },
	{ 0x0041CB20, LPS( "Battle" ), LPS( "\x02\x01" ) }, // "B1_01"
	{ 0x0041CAB0, LPS( "Book" ), LPS( "" ) },
	{ 0x0041DEC0, LPS( "BREAK" ), LPS( "" ) },
	{ 0x0041D210, LPS( "CheckObjective" ), LPS( "\x01" ) },
	{ 0x0041D950, LPS( "ChooseInit" ), LPS( "" ) },
	{ 0x0041DF20, LPS( "ClearVariables" ), LPS( "\x04\x01" ) }, // var
	{ 0x0041CB70, LPS( "Debrief" ), LPS( "" ) },
	{ 0x0041CC00, LPS( "Delay" ), LPS( "\x01" ) },
	{ 0x0041CB10, LPS( "Deploy" ), LPS( "" ) },
	{ 0x0041D870, LPS( "DisplayBitmap" ), LPS( "\x02\x01\x01" ) }, // ".bmp"
	{ 0x0041DE30, LPS( "DO" ), LPS( "\xFF" ) }, // unknown
	{ 0x0041DC60, LPS( "ELSE" ), LPS( "" ) },
	{ 0x0041DED0, LPS( "END" ), LPS( "" ) },
	{ 0x0041DC30, LPS( "ENDIF" ), LPS( "" ) },
	{ 0x0041D040, LPS( "ExcludeUnit" ), LPS( "\x01" ) },
	{ 0x0041CFB0, LPS( "ForceUnit" ), LPS( "\x01" ) },
	{ 0x0041D970, LPS( "GameOver" ), LPS( "" ) },
	{ 0x0041CDC0, LPS( "GetUnitStatus" ), LPS( "\x01" ) },
	{ 0x0041CE20, LPS( "GetUnitHireStatus" ), LPS( "\x01" ) },
	{ 0x0041DC80, LPS( "GOSUB" ), LPS( "\x03" ) }, // goto
	{ 0x0041DB60, LPS( "GOTO" ), LPS( "\x03" ) }, // goto
	{ 0x0041CD90, LPS( "HideMouse" ), LPS( "" ) },
	{ 0x0041DB80, LPS( "IF" ), LPS( "\x05\x01" ) },
	{ 0x0041D080, LPS( "IncludeUnit" ), LPS( "\x01" ) },
	{ 0x0041CB40, LPS( "InitDebrief" ), LPS( "\x01\x01" ) },
	{ 0x0041DE60, LPS( "LOOP" ), LPS( "" ) },
	{ 0x0041C970, LPS( "MeetingPoint" ), LPS( "\x02\x03" ) }, // ".bmp", label
	{ 0x0041C9D0, LPS( "MeetingWait" ), LPS( "" ) },
	{ 0x0041D420, LPS( "Narrate" ), LPS( "\x01\x01\x02\x01\x01" ) }, // ".mad"
	{ 0x0041CCE0, LPS( "Pause" ), LPS( "\x01" ) },
	{ 0x0041CBA0, LPS( "Picture" ),	LPS( "\xFF" ) }, // unknown
	{ 0x0041C920, LPS( "PlayMovie" ), LPS( "\x02" ) }, // ".tgq"
	{ 0x0041D9A0, LPS( "PlayMusic" ), LPS( "\x01" ) },
	{ 0x0041D740, LPS( "PlaySFX" ), LPS( "\x02\x02\x01\x01\x02\x01" ) }, // "stay", "continue", "H_KZ001"
	{ 0x0041DE10, LPS( "POPLV" ), LPS( "" ) },
	{ 0x0041DDF0, LPS( "PUSHLV" ), LPS( "" ) },
	{ 0x0041DF00, LPS( "ReadVariable" ), LPS( "\x04" ) }, // var
	{ 0x0041D8A0, LPS( "RemoveBitmap" ), LPS( "" ) },
	{ 0x0041D690, LPS( "RemoveMagic" ),	LPS( "\xFF" ) }, // unknown
	{ 0x0041CED0, LPS( "RemoveUnit"), LPS( "\x01" ) },
	{ 0x0041DD00, LPS( "REPEAT"), LPS( "" ) },
	{ 0x0041DCD0, LPS( "RETURN"), LPS( "" ) },
	{ 0x0041CAE0, LPS( "SaveGame"), LPS( "" ) },
	{ 0x0041D9E0, LPS( "SetBackground" ), LPS( "" ) },
	{ 0x0041CBE0, LPS( "SetDeafultSaveName" ), LPS( "\x02" ) }, // "text"
	{ 0x0041DE20, LPS( "SETLV" ), LPS( "\xFF" ) }, // unknown
	{ 0x0041D9C0, LPS( "SetMusic" ), LPS( "\x02" ) }, // ".fsm"
	{ 0x0041D250, LPS( "SetObjective" ), LPS( "\xFF\xFF" ) }, // unknown
	{ 0x0041DEE0, LPS( "SetVariable" ), LPS( "\x04\x01" ) }, // var
	{ 0x0041CDA0, LPS( "ShowMouse" ), LPS( "\xFF" ) }, // unknown
	{ 0x0041D2A0, LPS( "Speak" ), LPS( "\x06" ) }, // 2 + ( arg1 ), 4 ),== num of args
	{ 0x0041D360, LPS( "SpeakNoWait" ), LPS( "\x01" ) },
	{ 0x0041D8B0, LPS( "SpotAnim" ), LPS( "\x01\x01\x01\x01" ) },
	{ 0x0041D980, LPS( "StopAllSFX" ), LPS( "\x01\x01" ) },
	{ 0x0041D9B0, LPS( "StopMusic" ), LPS( "" ) },
	{ 0x0041D720, LPS( "StopSFX" ), LPS( "\x01" ) },
	{ 0x0041D120, LPS( "TemporyUnitClear" ), LPS( "\x01" ) },
	{ 0x0041D0D0, LPS( "TemporyUnitSet" ), LPS( "\x01" ) },
	{ 0x0041C910, LPS( "Test" ), LPS( "\x01" ) }, 
	{ 0x0041CA00, LPS( "TravelMap" ), LPS( "\x02\x02\x02\x01\x01\x01\x01\x01\x01\x01" ) }, // ".bmp", ".spr", ".dot"
	{ 0x0041CFF0, LPS( "UnForceUnit" ), LPS( "\x01" ) },
	{ 0x0041D1C0, LPS( "UnitIsGoingClear"), LPS( "\x01" ) },
	{ 0x0041D170, LPS( "UnitIsGoingSet" ), LPS( "\x01" ) },
	{ 0x0041DD20, LPS( "UNTIL" ), LPS( "\x05\x01" ) },
	{ 0x0041D550, LPS( "Voice" ), LPS( "\x02\x01" ) }, // "VC001"
	{ 0x0041D410, LPS( "Wait" ), LPS( "\x01" ) }
};



__inline SCRIPT_OP* in_command_set( const char *str, unsigned int len )
{
	// perfect hash generated by gperf
	const signed int MAX_HASH_VALUE = 114;
	static const unsigned char asso_values[] =
	{
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115,   0,  55,  35,   0,  40,
		  0,   0,  55,  50, 115,  10,   0,  70,  65,  70,
		  0, 115,  10,   5,  20,  30,   5,  25,  35, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115,   0,
		 30,  15,   5, 115,   0, 115, 115,  55, 115,  20,
		115, 115,  40, 115,  10,  10,  10, 115, 115, 115,
		115,  35, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
		115, 115, 115, 115, 115, 115
	};

	// less speed for greater readability and slighty smaller size
	static const signed char indextable[] =
	{
		-1, -1, -1, -1, 30,  0, -1,  2,  3, 37, 39, 40, 11, 51, 59, 50, -1,  4,
		21, 20, 34, 43, 35, 22, 36, 69, 56, 23, 47, 54, 44, 53, 52, 57, 63, 68,
		45, 41, 49, 70, 12, 13, 38, 17, 62, 18, 61, 60, 48,  1, 58, 65, 27, 14,
		67,  9, 66, -1, -1, 10, 24, 19, 42, -1,  8, 55, 29, -1, -1, 64,  7, 28,
		15, -1, 25, -1,  5, -1, -1, 26, -1, 46, -1, -1, 16, -1, -1, 33, -1, -1,
		-1, 32, 31, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1,  6 
	};

	//
	signed int key = len + asso_values[(unsigned char)str[len - 1]] + asso_values[(unsigned char)str[0]];
	if( key <= MAX_HASH_VALUE && key >= 0 )
	{
		int index = indextable[key];
		if( index != -1 )
		{
			SCRIPT_OP* op = &ops[ index ];
			if( len == op->name_len )
			{
				if( !memcmp( str, op->name, len ) )
				{
					return op;
				}
			}
		}
	}
	return NULL;
}


__inline int GetTok( char*& str )
{
	// skip white space
	while( *str == SPACE || *str == TAB ) str++; 

	// skip line comment
	if( ( str[0] == '/' ) && ( str[1] == '/' ) )
	{ 	
		str += 2;
		while( ( *str != '\n' ) && ( *str != '\0' ) ) str++;
	}

	// find length of op
	int len = 0;
	while( ( str[len] != SPACE ) &&
		( str[len] != TAB ) && 
		( str[len] != '\r' ) &&
		( str[len] != '\n' ) &&
		( str[len] != '\0' ) )
	{
		len++;
	}
	return len;
}



BOOL ImportScript( char* szFileName )
{
	char* str;
	int len;
	SCRIPT_OP* op;
	DWORD* cp = &code[0];
	BOOL bResult = FALSE;
	int line = 1;


	void* file = ReadInTextFile( szFileName );
	if( file == NULL ) goto lbl_end;


	// destroy previous string table
	micro_alloc_destroy();


//////////////////////////////////////////////////////////////
/************************************************************/
/************************************************************/
/////////////// part 1: compile /////////////////////////////
		
	str = (char*)file;

	// start off with 2 NULL entries just like the game
	WRITE_CODE( 0 ); WRITE_CODE( 0 );

	while( *str != '\0' )
	{	
		// get command
		len = GetTok( str );
		if( len != 0 )
		{
			op = in_command_set( str, len );
			if( op == NULL )
			{
				Log( "line %d: Unknown command. %s\r\n", line, str );
				goto lbl_end;
			}

			if( op->address != 0 ) // if not label
			{
				WRITE_CODE( op->address );
			}
			str += len; 

			// do args
			int count = 0;
			while( len = GetTok( str ) ) // loop until end of line
			{
				if( count < op->arg_cnt )
				{
					switch( op->arg_type[count] )
					{
						case 1: // int
						{
							DWORD dw;
							if( !my_atoi( str, len, &dw ) )
							{
								Log( "line %d: bad formatting, expected integer value. Arg# %d.\r\n", line, count + 1 );
								goto lbl_end;
							}
							WRITE_CODE( dw );
							str += len;
							break;
						}
						case 2: // text
						{

							if( str[0] != '\"' ) 
							{
								Log( "line %d: bad formatting, expected text value."
									" Text must be enclosed within double quote(\") characters. Arg# %d.\r\n", line, count + 1 );
								goto lbl_end;
							}

							// find ending quote
							if( str[ len-1 ] != '\"' )
							{
								while( ( str[len] != '\"' ) && ( str[len] != '\r' ) && ( str[len] != '\n' ) && ( str[len] != '\0' ) )
								{
									len++;
								}
								if( str[len] == '\"' ) len++;
								else
								{
									Log( "line %d: bad formatting, expected text value."
											" Text must be enclosed within double quote(\") characters. Arg# %d.\r\n", line, count + 1 );
									goto lbl_end;
								}
							}

							char* text = (char*) micro_alloc( len-1 );
							if( text == NULL )
							{
								Log( "Internal Error (script line %d): Failed to allocate %d bytes. Arg# %d.\r\n", line, len-1, count + 1 );
								goto lbl_end;
							}
							memcpy( text, &str[1], len-2 );
							WRITE_CODE( (uintptr_t)text );
							str += len;
							break;
						}
						case 3: // jmp
						{
							char* text = (char*) micro_alloc( len+1 );
							if( text == NULL )
							{
								Log( "Internal Error (script line %d): Failed to allocate %d bytes. Arg# %d.\r\n", line, len+1, count + 1 );
								goto lbl_end;
							}
							LABEL_ENTRY* le = (LABEL_ENTRY*) micro_alloc( sizeof(LABEL_ENTRY) );
							if( le == NULL )
							{
								Log( "Internal Error (script line %d): Failed to allocate %d bytes. Arg# %d.\r\n", line, sizeof(LABEL_ENTRY), count + 1 );
								goto lbl_end;
							}
							memcpy( text, str, len );
							le->name_len = len;
							le->name = text;
							le->addr = cp;
							le->next = jmps;
							jmps = le;
							WRITE_CODE( (uintptr_t)le->name ); // place-holder
							str += len;
							break;
						}
						case 4: // var
						{
							DWORD dw;			
							if( ( len < 4 ) || ( str[0] != 'v' ) || ( str[1] != 'a' ) || ( str[2] != 'r' ) || ( str[3] != '_' ) )
							{
								Log( "line %d: bad formatting, expected var_ keyword. Arg# %d.\r\n", line, count + 1 );
								goto lbl_end;
							}
							if( !my_atoi( &str[4], len - 4, &dw ) )
							{
								Log( "line %d: bad formatting, unrecognized var. Arg# %d.\r\n", line, count + 1 );
								goto lbl_end;
							}
							if( dw > 23 ) 
							{
								Log( "line %d: var index is outside of range. Arg# %d.\r\n", line, count + 1 );
								goto lbl_end;
							}
							WRITE_CODE( ( var_base + dw * 4 ) );
							str += len;
							break;
						}
						case 5: // cmp
						{
							DWORD dw = -1;
							if( len == 2 )
							{
								if( str[1] == '=' )
								{
									switch( *str )
									{
										case '=':
											dw = 1; 
											break;
										case '/': // fall-thru
										case '!':
											dw = 2;
											break;
									}
								}
								else
								{
									if( ( str[0] == '<' ) && ( str[1] == '>' ) )
									{	
										dw = 2;
									}
								}
							}
							if( len == 1 )
							{
								switch( *str )
								{
									case '=':
										dw = 1;
										break;
									case '<':
										dw = 3;
										break;
									case '>':
										dw = 4;
										break;
								}
							}
							if( dw == -1 )
							{
								Log( "line %d: bad formatting, unrecognized relationship/comparision operator. Arg# %d.\r\n", line, count );
								goto lbl_end;
							}
							WRITE_CODE( dw );
							str += len;
							break;
						}
						case 6: // var_arg ( WHMTG_Speak )
						{
							DWORD num_arg_groups = 0;
							DWORD i = 0;
							while( 1 )
							{
								DWORD dw;
								if( !my_atoi( str, len, &dw ) )
								{
									Log( "line %d: bad formatting, expected integer value. Arg# %d.\r\n", line, i );
									goto lbl_end;
								}
								if( i == 0 ) num_arg_groups = dw;
								WRITE_CODE( dw );
								str += len;
								i++;
								if( i >= ( 2 + num_arg_groups * 4 ) ) break; // exit loop

								// else get next arg
								len = GetTok( str );
								if( len == 0 )
								{
									Log( "line %d: %s, expected %d arguments, found %d.\r\n", line, op->name, 2 + ( num_arg_groups * 4 ), i );
									goto lbl_end;
								}
							}
							break;
						}
						case 7: // label
						{	
							char* text = (char*) micro_alloc( len+1 );
							if( text == NULL )
							{
								Log( "Internal Error (script line %d): Failed to allocate %d bytes. Arg# %d.\r\n", line, len+1, count );
								goto lbl_end;
							}
							LABEL_ENTRY* le = (LABEL_ENTRY*) micro_alloc( sizeof(LABEL_ENTRY) );
							if( le == NULL )
							{
								Log( "Internal Error (script line %d): Failed to allocate %d bytes. Arg# %d.\r\n", line, sizeof(LABEL_ENTRY), count );
								goto lbl_end;
							}
							memcpy( text, str, len );

							// jmp targets are aligned to 8
							if( (DWORD)cp & 4 ) WRITE_CODE( 0 ); // pad as per the game

							le->name_len = len;
							le->name = text;
							le->addr = cp;
							le->next = targets;
							targets = le;
							str += len;
							break;
						}
						default:
						{
							Log( "Internal Error (script line %d): unknown arg type. Arg# %d.\r\n", line, count );
							goto lbl_end;
						}
					}
				}
				count++;
			}
			if( count != op->arg_cnt )
			{
				Log( "line %d: %s, expected %d arguments, found %d.\r\n", line, op->name, op->arg_cnt, count );
				goto lbl_end;
			}
		}

		// next line
		if( *str == '\r' ) str++;
		if( *str == '\n' ) str++;
		line++;
	}

	if( cp >= &code[_countof(code) - 1] )
	{
		Log( "Error: Byte-code exceeds maximum size of %d bytes.\r\n", sizeof( code ) );
		goto lbl_end;
	}

	// end with a NULL entry because that is what the game does
	WRITE_CODE( 0 );

	EP_EndScript = cp - 1;
	EP_StartScript = &code[0];

//////////////////////////////////////////////////////////////
/************************************************************/
/************************************************************/
/////////////// part 2: link  ////////////////////////////////


	// find the entry points
	EP_NewGame = NULL;
	EP_Tutorial = NULL;
	EP_TheLivingGem = NULL;
	EP_HandyMan = NULL;
	EP_RideTheSrorm = NULL;
	EP_YouAndI = NULL;
	for( LABEL_ENTRY* t = targets; t != NULL; t = t->next ) 
	{
		switch( t->name_len ) 
		{
			case 7:
				if( !memcmp( t->name, "NEWGAME", 7 ) ) EP_NewGame = t->addr;
				else if( !memcmp( t->name, "YOUANDI", 7 ) ) EP_YouAndI = t->addr;
				break;
			case 8:
				if( !memcmp( t->name, "TUTORIAL", 8 ) ) EP_Tutorial = t->addr;
				else if( !memcmp( t->name, "HANDYMAN", 8 ) ) EP_HandyMan = t->addr;
				break;
			case 12:
				if( !memcmp( t->name, "THELIVINGGEM", 12 ) ) EP_TheLivingGem = t->addr;
				else if( !memcmp( t->name, "RIDETHESTORM", 12 ) ) EP_RideTheSrorm = t->addr;
				break;
		}
	}
	if( EP_NewGame == NULL ) EP_NewGame = EP_StartScript;
	if( EP_Tutorial == NULL ) EP_Tutorial = EP_NewGame;
	if( EP_TheLivingGem == NULL ) EP_TheLivingGem = EP_NewGame;
	if( EP_HandyMan == NULL ) EP_HandyMan = EP_NewGame;
	if( EP_RideTheSrorm == NULL ) EP_RideTheSrorm = EP_NewGame;
	if( EP_YouAndI == NULL ) EP_YouAndI = EP_NewGame;


	// link jmps with targets
	for( LABEL_ENTRY* j = jmps; j != NULL; j = j->next )
	{
		LABEL_ENTRY* t;
		for( t = targets; t != NULL; t = t->next ) 
		{
			if( j->name_len == t->name_len )
			{
				if( !memcmp( j->name, t->name, t->name_len ) )
				{
					OutputDebugString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" );
					*j->addr = (DWORD)t->addr;
					break;
				}
			}
		}
		if( t == NULL )
		{
			Log( "Error: LABEL %s not found.\r\n", j->name ); 
			goto lbl_end;
		}
	}


//////////////////////////////////////////////////////////////
/************************************************************/
/************************************************************/
/////////////// part 3: hook-in /////////////////////////////

	DWORD dwPrevProtect;

	// .text section
	if( VirtualProtect( (void*)0x0040E000, 0x1F000, PAGE_READWRITE, &dwPrevProtect ) )
	{
		*( (DWORD**)0x0040EFB4 ) = EP_StartScript;
		*( (DWORD**)0x0040EFCC ) = EP_EndScript;
		*( (DWORD**)0x0040EFAA ) = EP_StartScript;

		*( (DWORD**)0x00420F56 ) = EP_StartScript;
		*( (DWORD**)0x00420F8E ) = EP_EndScript;

		*( (DWORD**)0x00421B83 ) = EP_StartScript;
		*( (DWORD**)0x00421BC6 ) = EP_EndScript;

		*( (DWORD**)0x0042943F ) = EP_Tutorial;
		*( (DWORD**)0x0042C4F7 ) = EP_NewGame;

		VirtualProtect( (void*)0x0040E000, 0x1F000, dwPrevProtect, &dwPrevProtect );
	}

	// .data section
	*( (DWORD**)0x004D7134 ) = EP_TheLivingGem;
	*( (DWORD**)0x004D715C ) = EP_HandyMan;
	*( (DWORD**)0x004D7184 ) = EP_RideTheSrorm;
	*( (DWORD**)0x004D71AC ) = EP_YouAndI;

	bResult = TRUE;

	/*
		HANDLE hFile = CreateFile("dump.bin", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		DWORD Written;
		WriteFile(hFile, code, sizeof( code ), &Written, NULL);
		CloseHandle(hFile);
	*/

lbl_end:
	if( file != NULL ) VirtualFree( file, 0, MEM_RELEASE );
	return bResult;
}


// warning: decimal input can overflow undetected
// doesn't support positive or negative signs
// expects leading whitespace to already be stripped
// returns false on error
BOOL my_atoi( char* str, int len, DWORD* pdwResult )
{
	DWORD acc = 0;

	// if hex notation
	if( ( len >= 3 ) && ( ( str[0] == '0' ) && ( ( str[1] == 'x' ) || ( str[1] == 'X' ) ) ) )
	{
		if( len > 10 ) return FALSE; // too many digits
		for( int i = 2; i < len; i++ )
		{
			DWORD v;
			char c = str[i];
			if( ( c >= '0' ) && ( c <= '9' ) )
			{
				v = (DWORD)((unsigned char)(c - '0'));
			}
			else
			{
				if( ( c >= 'a' ) && ( c <= 'f' ) )
				{
					v = (DWORD)((unsigned char)(c - 'a'));
				}
				else
				{
					if( ( c >= 'A' ) && ( c <= 'F' ) )
					{
						v = (DWORD)((unsigned char)(c - 'A'));
					}
					else
					{
						return FALSE; // bad digit
					}
				}
			}

			acc <<= 4;
			acc |= v;
		}	
	}
	else // decimal
	{
		for( int i = 0; i < len; i++ )
		{
			char c = str[i];
			if( ( c < '0' ) || ( c > '9' ) ) return FALSE; // bad digit
			acc = ( acc * 10 ) + ( c - '0' );
		}
	}

	*pdwResult = acc;
	return TRUE;
}


// so fun fact: OutputDebugString() isn't suitable
// because that gets spammed with complaints of heap corruption by dark omen / the kernel
// so here we dump to a file
void __cdecl Log( char* fmt, ...)
{
	static char buf[1024];
	static size_t pos = 0; 
	OVERLAPPED ol;
	int len;
	DWORD bytes_written;
	static HANDLE file = INVALID_HANDLE_VALUE; 

	if( file == INVALID_HANDLE_VALUE )
	{
		if( file == INVALID_HANDLE_VALUE )
		{
			file = CreateFile("script_err.txt", GENERIC_WRITE, 
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0
			);
			if( file == INVALID_HANDLE_VALUE ){ return; }
		}
	}

	va_list args;
    va_start(args,fmt);
    len = wvsprintf( buf, fmt, args );
	va_end(args);

	ol.Offset = pos;
	ol.OffsetHigh = 0; 
	ol.hEvent = NULL;
	
	if( WriteFile(file, buf, len, &bytes_written,  &ol) )
	{
		pos += bytes_written;
	} 
	else
	{
		// GetLastError();
	}
}


// read in a file
// place a nul byte at the end.
char* ReadInTextFile( CHAR* szFileName )
{
	void* readbuf = NULL;
	DWORD dwBytesRead = 0;
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwBytesToRead = GetFileSize(hFile, NULL);
		if((dwBytesToRead != 0) && (dwBytesToRead != 0xFFFFFFFF))
		{
			readbuf = VirtualAlloc( NULL, dwBytesToRead + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
			if( readbuf != NULL)
			{
				ReadFile(hFile, readbuf, dwBytesToRead, &dwBytesRead, NULL);
			}	
		}
		CloseHandle(hFile);
	}

	if( dwBytesRead == 0 )
	{
		if( readbuf != NULL )
		{
			DWORD dwErrCode = GetLastError();
			VirtualFree( readbuf, 0, MEM_RELEASE );
			SetLastError( dwErrCode );
			readbuf = NULL;
		}

		// print error
		char buf[320];
		Log( "Error Accessing File: %s\"", szFileName );
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, buf, sizeof(buf), NULL );
		Log( "\"\r\n    %s\r\n", buf );
	}
	return (char*)readbuf;
}


/////////////////////////////
// cheesy allocator
/////////////////////////////////
void* alloc_region = NULL;
BYTE* end_region = 0; 
BYTE* end_page = 0; 
BYTE* alloc_ptr = 0;
void micro_alloc_destroy()
{
	while( alloc_region != NULL )
	{
		void* r = alloc_region;
		alloc_region = *((DWORD**)alloc_region);
		VirtualFree( r, 0, MEM_RELEASE );
	}
	end_region = 0;
	end_page = 0;
	alloc_ptr = 0;
}
void* micro_alloc( int size )
{
	static SYSTEM_INFO sSysInfo = { 0 };
	BOOL new_region;

	if( sSysInfo.dwPageSize == 0 ) // initialize
	{
		GetSystemInfo(&sSysInfo);
	}

	if( ( alloc_ptr + size ) >= end_page ) 
	{
		
		alloc_ptr = end_page;

		if( alloc_ptr == end_region )
		{	
			alloc_ptr = (BYTE*) VirtualAlloc( NULL, sSysInfo.dwAllocationGranularity, MEM_RESERVE, PAGE_NOACCESS );
			end_region = alloc_ptr + sSysInfo.dwAllocationGranularity;
			new_region = TRUE;
		}
		else
		{
			new_region = FALSE;
		}

		if( alloc_ptr != NULL )
		{ 
			alloc_ptr = (BYTE*) VirtualAlloc( alloc_ptr, sSysInfo.dwPageSize, MEM_COMMIT, PAGE_READWRITE );
			end_page = alloc_ptr + sSysInfo.dwPageSize;
			
			// half-baked track regions
			if( new_region != FALSE )
			{
				*((DWORD*)alloc_ptr) = (DWORD)alloc_region;
				alloc_region = alloc_ptr;
				alloc_ptr += 4;
			}
		}

		if( alloc_ptr == NULL )
		{
			end_region = 0; 
			end_page = 0; 
		}
	}

	// "bump the pointer"
	void* p = alloc_ptr;
	alloc_ptr += size;
	return p;
}
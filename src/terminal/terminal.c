#include "terminal/terminal.h"
#include "memory/heap.h"

static uint8_t s_u8TerminalCount = 0;
static Terminal s_pTerminals[MAX_TERMINALS];

static inline uint16_t create_terminal_buffer_entry(unsigned char character, uint8_t u8Colour) 
{
	return (uint16_t)u8Colour << 8 | (uint16_t)character;
}

static inline Terminal* const get_terminal(const uint8_t u8TerminalId)
{
    return &s_pTerminals[u8TerminalId - 1];
}

static void clear_terminal(Terminal* const pTerminal)
{
    const uint8_t u8EntryColour = pTerminal->backgroundColour << 4 | pTerminal->foregroundColour;
	for (size_t y = 0; y < pTerminal->u64Height; y++) {
		for (size_t x = 0; x < pTerminal->u64Width; x++) {
			const size_t index = y * pTerminal->u64Width + x;
			pTerminal->pBuffer[index] = create_terminal_buffer_entry(' ', u8EntryColour);
		}
	}
}

static void write_character_to_terminal(Terminal* const pTerminal, const char character)
{
    if (++pTerminal->u64PosX == pTerminal->backgroundColour)
    {
        pTerminal->u64PosX = 0;
        if (++pTerminal->u64PosY == pTerminal->u64PosY)
            pTerminal->u64PosY = 0;
    }

    const uint8_t u8EntryColour = pTerminal->backgroundColour << 4 | pTerminal->foregroundColour;
    pTerminal->pBuffer[(pTerminal->u64PosY * pTerminal->u64Width) + pTerminal->u64PosX] = create_terminal_buffer_entry(character, u8EntryColour);
}

const uint8_t CreateTerminal(const uint64_t u64Width, const uint64_t u64Height)
{
    Terminal* const pTerminal = get_terminal(++s_u8TerminalCount);
    pTerminal->backgroundColour = VGA_COLOR_BLACK;
    pTerminal->foregroundColour = VGA_COLOR_GREEN;
    pTerminal->u64Width = u64Width;
    pTerminal->u64Height = u64Height;
    pTerminal->u64PosX = 0;
    pTerminal->u64PosY = 0;
    pTerminal->pBuffer = (uint16_t*)0xB8000; //video memory addres in protected mode

    clear_terminal(pTerminal);

    return s_u8TerminalCount;
}

void WriteToTerminal(const uint8_t u8TerminalId, const char* pString)
{
    Terminal* pTerminal = get_terminal(u8TerminalId);
    if (pTerminal == nullptr)
        return;

    for (char* p = pString; *p != '\0'; p++)
    {
        if (*p == '\n')
        {
            pTerminal->u64PosX = 0;
            if (++pTerminal->u64PosY == pTerminal->u64Height)
                pTerminal->u64PosY = 0;

            continue;
        }

        write_character_to_terminal(pTerminal, *p);
    }
}

void ClearTerminal(const uint8_t u8TerminalId)
{
    Terminal* pTerminal = get_terminal(u8TerminalId);
    if (pTerminal == nullptr)
        return;

    clear_terminal(pTerminal);
}
#include "kernel/gdt.h"

extern void _load_gdt(uint32_t);
uint64_t ulEncodedDescriptors[3];
GDT gdt;

struct gdt_entry_struct
{
   uint16_t limit_low;           // The lower 16 bits of the limit.
   uint16_t base_low;            // The lower 16 bits of the base.
   uint8_t  base_middle;         // The next 8 bits of the base.
   uint8_t  access;              // Access flags, determine what ring this segment can be used in.
   uint8_t  granularity;
   uint8_t  base_high;           // The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

void IntialiseGDT()
{
    GDTDescriptor gdtDescriptors[3];
	//Null descriptor
	gdtDescriptors[0].uiLimit = 0;
	gdtDescriptors[0].uiBase = 0;
	gdtDescriptors[0].accessBits.present = 0;
    gdtDescriptors[0].accessBits.privilige = 0;
    gdtDescriptors[0].accessBits.descriptor_type = 0;
    gdtDescriptors[0].accessBits.executable = 0;
    gdtDescriptors[0].accessBits.direction_conforming = 0;
    gdtDescriptors[0].accessBits.readable_writeable = 0;
    gdtDescriptors[0].accessBits.accessed = 0;
	gdtDescriptors[0].flagBits.granularity = 0;
	gdtDescriptors[0].flagBits.size = 0;
	gdtDescriptors[0].flagBits.padding = 0;

	//Code descriptor - selector 0x08
	gdtDescriptors[1].uiLimit = 0xFFFFFFFF; //full 4gb
	gdtDescriptors[1].uiBase = 0;
	gdtDescriptors[1].accessBits.present = 1;
    gdtDescriptors[1].accessBits.privilige = 0;
    gdtDescriptors[1].accessBits.descriptor_type = 1;
    gdtDescriptors[1].accessBits.executable = 1;
    gdtDescriptors[1].accessBits.direction_conforming = 0;
    gdtDescriptors[1].accessBits.readable_writeable = 1;
    gdtDescriptors[1].accessBits.accessed = 0;
	gdtDescriptors[1].flagBits.granularity = 1;
	gdtDescriptors[1].flagBits.size = 1;
	gdtDescriptors[1].flagBits.padding = 0;

	//Data descriptor - selector 0x10
	gdtDescriptors[2].uiLimit = 0xFFFFFFFF; //full 4gb
	gdtDescriptors[2].uiBase = 0;
	gdtDescriptors[2].accessBits.present = 1;
    gdtDescriptors[2].accessBits.privilige = 0;
    gdtDescriptors[2].accessBits.descriptor_type = 1;
    gdtDescriptors[2].accessBits.executable = 0;
    gdtDescriptors[2].accessBits.direction_conforming = 0;
    gdtDescriptors[2].accessBits.readable_writeable = 1;
    gdtDescriptors[2].accessBits.accessed = 0;
	gdtDescriptors[2].flagBits.granularity = 1;
	gdtDescriptors[2].flagBits.size = 1;
	gdtDescriptors[2].flagBits.padding = 0;


	_load_gdt(CreateGDT(gdtDescriptors, 3, ulEncodedDescriptors, &gdt));
}

uint64_t EncodeDescriptor(const GDTDescriptor* const pDescriptor)
{
    uint64_t ulEncodedDescriptor = 0;

    ulEncodedDescriptor = pDescriptor->uiLimit & 0x0000FFFF;
    ulEncodedDescriptor |= (uint64_t)(pDescriptor->uiBase & 0x0000FFFF) << 16;
    ulEncodedDescriptor |= (uint64_t)(pDescriptor->uiBase & (0x00FF0000)) << 32;
    ulEncodedDescriptor |= ((uint64_t)(*(uint8_t*)&pDescriptor->accessBits)) << 40;
    ulEncodedDescriptor |= ((uint64_t)(*(uint8_t*)&pDescriptor->accessBits)) << 40;
    ulEncodedDescriptor |= ((uint64_t)(pDescriptor->uiLimit & 0x000F0000)) << 48;
    ulEncodedDescriptor |= ((uint64_t)(*(uint8_t*)&pDescriptor->flagBits)) << 52;
    ulEncodedDescriptor |= ((uint64_t)(pDescriptor->uiBase & 0xFF000000)) << 56;

    gdt_entry_t entry;
    entry.base_low    = (pDescriptor->uiBase & 0xFFFF);
    entry.base_middle = (pDescriptor->uiBase >> 16) & 0xFF;
    entry.base_high   = (pDescriptor->uiBase >> 24) & 0xFF;
    entry.limit_low   = (pDescriptor->uiLimit & 0xFFFF);
    entry.granularity = (pDescriptor->uiLimit >> 16) & 0x0F;
    entry.granularity |= *(uint8_t*)&pDescriptor->flagBits & 0xF0;
    entry.access      = *(uint8_t*)&pDescriptor->accessBits;

    return *(uint64_t*)&entry;
}

uint16_t EncodeDescriptors(const GDTDescriptor descriptors[], uint16_t usNumberOfDescriptors, uint64_t* const pEncodedDescriptors)
{
    for (uint16_t i = 0; i < usNumberOfDescriptors; i++)
        pEncodedDescriptors[i] = EncodeDescriptor(&descriptors[i]);

    return (usNumberOfDescriptors * sizeof(uint64_t)) - 1;
}

uint32_t CreateGDT(const GDTDescriptor descriptors[], uint16_t usNumberOfDescriptors, uint64_t* const pEncodedDescriptors, GDT* const pGDT)
{
    pGDT->usLimit = EncodeDescriptors(descriptors, usNumberOfDescriptors, pEncodedDescriptors);
    pGDT->uiBaseAddress = (uint32_t)pEncodedDescriptors;

    return (uint32_t)pGDT;
}
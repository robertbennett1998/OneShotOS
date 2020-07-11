#ifndef GDT_H
#define GDT_H

#include <stdint.h>

typedef struct gdt_descriptor_access_bits_s
{
    uint8_t accessed : 1;
    uint8_t readable_writeable : 1;
    uint8_t direction_conforming : 1;
    uint8_t executable : 1; //value of 1 code sector - 0 data sector 
    uint8_t descriptor_type : 1;
    uint8_t privilige : 2;
    uint8_t present : 1;

    
    
    
} __attribute__((packed)) GDTDescriptorAcessBits;

typedef struct gdt_descriptor_flag_bits_s
{
    uint8_t padding : 6;

    uint8_t size : 1;
    uint8_t granularity : 1;
    
} __attribute__((packed)) GDTDescriptorFlagBits;

typedef struct gdt_descriptor_s
{
    uint32_t uiLimit;
    uint32_t uiBase;
    struct gdt_descriptor_access_bits_s accessBits;
    struct gdt_descriptor_flag_bits_s flagBits;
} __attribute__((packed)) GDTDescriptor;

typedef struct gdt_s
{
    uint16_t usLimit;
    uint32_t uiBaseAddress;
} __attribute__((packed)) GDT;

extern uint64_t EncodeDescriptor(const GDTDescriptor* const pDescriptor);
extern uint16_t EncodeDescriptors(const GDTDescriptor descriptors[], uint16_t usNumberOfDescriptors, uint64_t* const pEncodedDescriptors);
extern uint32_t CreateGDT(const GDTDescriptor descriptors[], uint16_t usNumberOfDescriptors, uint64_t* const pEncodedDescriptors, GDT* const pGDT);

#endif //GDT_H
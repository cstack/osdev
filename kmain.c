#include "asm_interrupts.h"
#include "assembly_interface.h"
#include "drivers/frame_buffer.h"
#include "drivers/pic.h"
#include "drivers/serial_port.h"
#include "interrupts.h"
#include "stdio.h"
#include "types.h"

// a pointer to the global descriptor table
// passed by reference to the LGDT instruction
struct gdt_description_structure_t {
  uint16_t size; // in bytes
  uint32_t offset;
} __attribute__((packed)) gdt_description_structure;

// a pointer to the interrupt descriptor table
// passed by reference to the LIDT instruction
struct idt_description_structure_t {
  uint16_t size; // in bytes
  uint32_t offset;
} __attribute__((packed)) idt_description_structure;

// See http://wiki.osdev.org/Global_Descriptor_Table#Structure
struct segment_descriptor_t {
  uint16_t limit_0_15; // bits o-15 of limit
  uint16_t base_0_15;
  uint8_t base_16_23;
  uint8_t access_byte;
  uint8_t flags_and_limit_16_19;
  uint8_t base_24_31;
} __attribute__((packed));

// See http://wiki.osdev.org/Interrupt_Descriptor_Table#Structure_IA-32
struct interrupt_descriptor_t {
  uint16_t offset_0_15;
  uint16_t selector;
  uint8_t zero;
  uint8_t type_attr;
  uint16_t offset_16_31;
} __attribute__((packed));

enum segment_selector_t {
  NULL_DESCRIPTOR, // Not used but has to be here
  KERNAL_CODE_SEGMENT_INDEX, // Offset 0x8
  KERNAL_DATA_SEGMENT_INDEX // Offset 0x10
};
struct segment_descriptor_t gdt[3];
const uint16_t NULL_SEGMENT_SELECTOR = 0x0;
const uint16_t KERNAL_CODE_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * 1;
const uint16_t KERNAL_DATA_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * 2;

struct interrupt_descriptor_t idt[256];

void initialize_gdt() {
  gdt_description_structure.size = sizeof(gdt) - 1;
  gdt_description_structure.offset = (uint32_t) gdt;

  gdt[KERNAL_CODE_SEGMENT_INDEX].limit_0_15 = 0xFFFF;
  gdt[KERNAL_CODE_SEGMENT_INDEX].base_0_15 = 0x0000;
  gdt[KERNAL_CODE_SEGMENT_INDEX].base_16_23 = 0x00;
  gdt[KERNAL_CODE_SEGMENT_INDEX].access_byte = 0b10011010;
  gdt[KERNAL_CODE_SEGMENT_INDEX].flags_and_limit_16_19 = 0xCF;
  gdt[KERNAL_CODE_SEGMENT_INDEX].base_24_31 = 0x00;

  gdt[KERNAL_DATA_SEGMENT_INDEX].limit_0_15 = 0xFFFF;
  gdt[KERNAL_DATA_SEGMENT_INDEX].base_0_15 = 0x0000;
  gdt[KERNAL_DATA_SEGMENT_INDEX].base_16_23 = 0x00;
  gdt[KERNAL_DATA_SEGMENT_INDEX].access_byte = 0b10010010;
  gdt[KERNAL_DATA_SEGMENT_INDEX].flags_and_limit_16_19 = 0xCF;
  gdt[KERNAL_DATA_SEGMENT_INDEX].base_24_31 = 0x00;

  lgdt(&gdt_description_structure);

  // Grub has already loaded the segment registers
  // with the correct values (0x8 for cs, 0x10 for the others)
}

uint32_t interrupt_handler_addresses[] = {
  (uint32_t) interrupt_handler_0,
  (uint32_t) interrupt_handler_1,
  (uint32_t) interrupt_handler_2,
  (uint32_t) interrupt_handler_3,
  (uint32_t) interrupt_handler_4,
  (uint32_t) interrupt_handler_5,
  (uint32_t) interrupt_handler_6,
  (uint32_t) interrupt_handler_7,
  (uint32_t) interrupt_handler_8,
  (uint32_t) interrupt_handler_9,
  (uint32_t) interrupt_handler_10,
  (uint32_t) interrupt_handler_11,
  (uint32_t) interrupt_handler_12,
  (uint32_t) interrupt_handler_13,
  (uint32_t) interrupt_handler_14,
  (uint32_t) interrupt_handler_15,
  (uint32_t) interrupt_handler_16,
  (uint32_t) interrupt_handler_17,
  (uint32_t) interrupt_handler_18,
  (uint32_t) interrupt_handler_19,
  (uint32_t) interrupt_handler_20,
  (uint32_t) interrupt_handler_21,
  (uint32_t) interrupt_handler_22,
  (uint32_t) interrupt_handler_23,
  (uint32_t) interrupt_handler_24,
  (uint32_t) interrupt_handler_25,
  (uint32_t) interrupt_handler_26,
  (uint32_t) interrupt_handler_27,
  (uint32_t) interrupt_handler_28,
  (uint32_t) interrupt_handler_29,
  (uint32_t) interrupt_handler_30,
  (uint32_t) interrupt_handler_31,
  (uint32_t) interrupt_handler_32,
  (uint32_t) interrupt_handler_33,
  (uint32_t) interrupt_handler_34,
  (uint32_t) interrupt_handler_35,
  (uint32_t) interrupt_handler_36,
  (uint32_t) interrupt_handler_37,
  (uint32_t) interrupt_handler_38,
  (uint32_t) interrupt_handler_39,
  (uint32_t) interrupt_handler_40,
  (uint32_t) interrupt_handler_41,
  (uint32_t) interrupt_handler_42,
  (uint32_t) interrupt_handler_43,
  (uint32_t) interrupt_handler_44,
  (uint32_t) interrupt_handler_45,
  (uint32_t) interrupt_handler_46,
  (uint32_t) interrupt_handler_47,
  (uint32_t) interrupt_handler_48,
  (uint32_t) interrupt_handler_49,
  (uint32_t) interrupt_handler_50,
  (uint32_t) interrupt_handler_51,
  (uint32_t) interrupt_handler_52,
  (uint32_t) interrupt_handler_53,
  (uint32_t) interrupt_handler_54,
  (uint32_t) interrupt_handler_55,
  (uint32_t) interrupt_handler_56,
  (uint32_t) interrupt_handler_57,
  (uint32_t) interrupt_handler_58,
  (uint32_t) interrupt_handler_59,
  (uint32_t) interrupt_handler_60,
  (uint32_t) interrupt_handler_61,
  (uint32_t) interrupt_handler_62,
  (uint32_t) interrupt_handler_63,
  (uint32_t) interrupt_handler_64,
  (uint32_t) interrupt_handler_65,
  (uint32_t) interrupt_handler_66,
  (uint32_t) interrupt_handler_67,
  (uint32_t) interrupt_handler_68,
  (uint32_t) interrupt_handler_69,
  (uint32_t) interrupt_handler_70,
  (uint32_t) interrupt_handler_71,
  (uint32_t) interrupt_handler_72,
  (uint32_t) interrupt_handler_73,
  (uint32_t) interrupt_handler_74,
  (uint32_t) interrupt_handler_75,
  (uint32_t) interrupt_handler_76,
  (uint32_t) interrupt_handler_77,
  (uint32_t) interrupt_handler_78,
  (uint32_t) interrupt_handler_79,
  (uint32_t) interrupt_handler_80,
  (uint32_t) interrupt_handler_81,
  (uint32_t) interrupt_handler_82,
  (uint32_t) interrupt_handler_83,
  (uint32_t) interrupt_handler_84,
  (uint32_t) interrupt_handler_85,
  (uint32_t) interrupt_handler_86,
  (uint32_t) interrupt_handler_87,
  (uint32_t) interrupt_handler_88,
  (uint32_t) interrupt_handler_89,
  (uint32_t) interrupt_handler_90,
  (uint32_t) interrupt_handler_91,
  (uint32_t) interrupt_handler_92,
  (uint32_t) interrupt_handler_93,
  (uint32_t) interrupt_handler_94,
  (uint32_t) interrupt_handler_95,
  (uint32_t) interrupt_handler_96,
  (uint32_t) interrupt_handler_97,
  (uint32_t) interrupt_handler_98,
  (uint32_t) interrupt_handler_99,
  (uint32_t) interrupt_handler_100,
  (uint32_t) interrupt_handler_101,
  (uint32_t) interrupt_handler_102,
  (uint32_t) interrupt_handler_103,
  (uint32_t) interrupt_handler_104,
  (uint32_t) interrupt_handler_105,
  (uint32_t) interrupt_handler_106,
  (uint32_t) interrupt_handler_107,
  (uint32_t) interrupt_handler_108,
  (uint32_t) interrupt_handler_109,
  (uint32_t) interrupt_handler_110,
  (uint32_t) interrupt_handler_111,
  (uint32_t) interrupt_handler_112,
  (uint32_t) interrupt_handler_113,
  (uint32_t) interrupt_handler_114,
  (uint32_t) interrupt_handler_115,
  (uint32_t) interrupt_handler_116,
  (uint32_t) interrupt_handler_117,
  (uint32_t) interrupt_handler_118,
  (uint32_t) interrupt_handler_119,
  (uint32_t) interrupt_handler_120,
  (uint32_t) interrupt_handler_121,
  (uint32_t) interrupt_handler_122,
  (uint32_t) interrupt_handler_123,
  (uint32_t) interrupt_handler_124,
  (uint32_t) interrupt_handler_125,
  (uint32_t) interrupt_handler_126,
  (uint32_t) interrupt_handler_127,
  (uint32_t) interrupt_handler_128,
  (uint32_t) interrupt_handler_129,
  (uint32_t) interrupt_handler_130,
  (uint32_t) interrupt_handler_131,
  (uint32_t) interrupt_handler_132,
  (uint32_t) interrupt_handler_133,
  (uint32_t) interrupt_handler_134,
  (uint32_t) interrupt_handler_135,
  (uint32_t) interrupt_handler_136,
  (uint32_t) interrupt_handler_137,
  (uint32_t) interrupt_handler_138,
  (uint32_t) interrupt_handler_139,
  (uint32_t) interrupt_handler_140,
  (uint32_t) interrupt_handler_141,
  (uint32_t) interrupt_handler_142,
  (uint32_t) interrupt_handler_143,
  (uint32_t) interrupt_handler_144,
  (uint32_t) interrupt_handler_145,
  (uint32_t) interrupt_handler_146,
  (uint32_t) interrupt_handler_147,
  (uint32_t) interrupt_handler_148,
  (uint32_t) interrupt_handler_149,
  (uint32_t) interrupt_handler_150,
  (uint32_t) interrupt_handler_151,
  (uint32_t) interrupt_handler_152,
  (uint32_t) interrupt_handler_153,
  (uint32_t) interrupt_handler_154,
  (uint32_t) interrupt_handler_155,
  (uint32_t) interrupt_handler_156,
  (uint32_t) interrupt_handler_157,
  (uint32_t) interrupt_handler_158,
  (uint32_t) interrupt_handler_159,
  (uint32_t) interrupt_handler_160,
  (uint32_t) interrupt_handler_161,
  (uint32_t) interrupt_handler_162,
  (uint32_t) interrupt_handler_163,
  (uint32_t) interrupt_handler_164,
  (uint32_t) interrupt_handler_165,
  (uint32_t) interrupt_handler_166,
  (uint32_t) interrupt_handler_167,
  (uint32_t) interrupt_handler_168,
  (uint32_t) interrupt_handler_169,
  (uint32_t) interrupt_handler_170,
  (uint32_t) interrupt_handler_171,
  (uint32_t) interrupt_handler_172,
  (uint32_t) interrupt_handler_173,
  (uint32_t) interrupt_handler_174,
  (uint32_t) interrupt_handler_175,
  (uint32_t) interrupt_handler_176,
  (uint32_t) interrupt_handler_177,
  (uint32_t) interrupt_handler_178,
  (uint32_t) interrupt_handler_179,
  (uint32_t) interrupt_handler_180,
  (uint32_t) interrupt_handler_181,
  (uint32_t) interrupt_handler_182,
  (uint32_t) interrupt_handler_183,
  (uint32_t) interrupt_handler_184,
  (uint32_t) interrupt_handler_185,
  (uint32_t) interrupt_handler_186,
  (uint32_t) interrupt_handler_187,
  (uint32_t) interrupt_handler_188,
  (uint32_t) interrupt_handler_189,
  (uint32_t) interrupt_handler_190,
  (uint32_t) interrupt_handler_191,
  (uint32_t) interrupt_handler_192,
  (uint32_t) interrupt_handler_193,
  (uint32_t) interrupt_handler_194,
  (uint32_t) interrupt_handler_195,
  (uint32_t) interrupt_handler_196,
  (uint32_t) interrupt_handler_197,
  (uint32_t) interrupt_handler_198,
  (uint32_t) interrupt_handler_199,
  (uint32_t) interrupt_handler_200,
  (uint32_t) interrupt_handler_201,
  (uint32_t) interrupt_handler_202,
  (uint32_t) interrupt_handler_203,
  (uint32_t) interrupt_handler_204,
  (uint32_t) interrupt_handler_205,
  (uint32_t) interrupt_handler_206,
  (uint32_t) interrupt_handler_207,
  (uint32_t) interrupt_handler_208,
  (uint32_t) interrupt_handler_209,
  (uint32_t) interrupt_handler_210,
  (uint32_t) interrupt_handler_211,
  (uint32_t) interrupt_handler_212,
  (uint32_t) interrupt_handler_213,
  (uint32_t) interrupt_handler_214,
  (uint32_t) interrupt_handler_215,
  (uint32_t) interrupt_handler_216,
  (uint32_t) interrupt_handler_217,
  (uint32_t) interrupt_handler_218,
  (uint32_t) interrupt_handler_219,
  (uint32_t) interrupt_handler_220,
  (uint32_t) interrupt_handler_221,
  (uint32_t) interrupt_handler_222,
  (uint32_t) interrupt_handler_223,
  (uint32_t) interrupt_handler_224,
  (uint32_t) interrupt_handler_225,
  (uint32_t) interrupt_handler_226,
  (uint32_t) interrupt_handler_227,
  (uint32_t) interrupt_handler_228,
  (uint32_t) interrupt_handler_229,
  (uint32_t) interrupt_handler_230,
  (uint32_t) interrupt_handler_231,
  (uint32_t) interrupt_handler_232,
  (uint32_t) interrupt_handler_233,
  (uint32_t) interrupt_handler_234,
  (uint32_t) interrupt_handler_235,
  (uint32_t) interrupt_handler_236,
  (uint32_t) interrupt_handler_237,
  (uint32_t) interrupt_handler_238,
  (uint32_t) interrupt_handler_239,
  (uint32_t) interrupt_handler_240,
  (uint32_t) interrupt_handler_241,
  (uint32_t) interrupt_handler_242,
  (uint32_t) interrupt_handler_243,
  (uint32_t) interrupt_handler_244,
  (uint32_t) interrupt_handler_245,
  (uint32_t) interrupt_handler_246,
  (uint32_t) interrupt_handler_247,
  (uint32_t) interrupt_handler_248,
  (uint32_t) interrupt_handler_249,
  (uint32_t) interrupt_handler_250,
  (uint32_t) interrupt_handler_251,
  (uint32_t) interrupt_handler_252,
  (uint32_t) interrupt_handler_253,
  (uint32_t) interrupt_handler_254,
  (uint32_t) interrupt_handler_255,
};

void initialize_idt() {
  idt_description_structure.size = sizeof(idt) - 1;
  idt_description_structure.offset = (uint32_t) idt;

  uint16_t selector = KERNAL_CODE_SEGMENT_SELECTOR;
  uint8_t zero = 0x00;
  uint8_t type_attr = 0x8E;

  for (int i = 0; i < 256; i++) {
    uint32_t interrupt_handler_address = interrupt_handler_addresses[i];
    uint16_t offset_0_15 = interrupt_handler_address & 0x0000FFFF;
    uint16_t offset_16_31 = interrupt_handler_address >> 16;

    idt[i].offset_0_15 = offset_0_15;
    idt[i].selector = selector;
    idt[i].zero = zero;
    idt[i].type_attr = type_attr;
    idt[i].offset_16_31 = offset_16_31;
  }

  load_idt(&idt_description_structure);
}

static char *welcome_string = ""
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                              | |                  | |                          "
"                   ___   ___  | |_    __ _    ___  | | __                       "
"                  / __| / __| | __|  / _` |  / __| | |/ /                       "
"                 | (__  \\__ \\ | |_  | (_| | | (__  |   <                        "
"                  \\___| |___/  \\__|  \\__,_|  \\___| |_|\\_\\                       "
"                                                                                "
"                                                                                "
"                             ____     _____                                     "
"                            / __ \\   / ____|                                    "
"                           | |  | | | (___                                      "
"                           | |  | |  \\___ \\                                     "
"                           | |__| |  ____) |                                    "
"                            \\____/  |_____/                                     "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"                                                                                "
"";

void kmain() {
  clear_screen();

  printf(welcome_string);

  serial_init();
  log("Initialized serial port.\n");

  initialize_gdt();
  log("Loaded global descriptor table.\n");

  initialize_idt();
  log("Loaded interrupt descriptor table.\n");

  interrupt(49);
  log("Returned from test interrupt()\n");

  pic_init();
  log("Initialized PIC\n");

  // Loop forever
  // TODO: accept user input
  uint32_t i = 0;
  while(1) {
    if (i++ == 0x0FFFFFFF) {
      log("tick\n");
      i = 0;
    }
  };
}
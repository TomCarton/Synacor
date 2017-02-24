//
//  label.c
//  Synacor
//
//  Created by Thomas CARTON
//

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#include <memory.h>
#include <limits.h>

#include "types.h"
#include "instruction.h"

#include "label.h"


extern word mem[];


unsigned int labelCount = 166;
Label labels[] =
{
    {0x0000, "Start"},
    
    {0x015b, "SelfTest"},

    {0x0166, "TestJump"},
    {0x0170, "ErrJumpM2"},
    {0x018d, "ErrJumpM1"},
    {0x01a8, "ErrJumpP1"},
    {0x01c5, "ErrJumpP2"},
    {0x01e4, "TestJtJf"},
    {0x01ef, "TestJtJf2"},
    {0x01f4, "TestRegs"},
    {0x0234, "TestEqOp"},
    {0x024e, "TestStack"},
    {0x02ac, "TestNot"},
    {0x02c4, "BackCallOp"},
    {0x02db, "BackCallOp2"},
    {0x034b, "TestMemSlot"},
    {0x034d, "TestMem"},
    {0x03ad, "ErrWmem"},
    {0x03ff, "LBL_32"},
    {0x03e8, "LBL_33"},
    {0x0432, "ErrNoJtJf"},
    {0x0445, "ErrRegs"},
    {0x045e, "ErrSetOp"},
    {0x0486, "ErrStack"},
    {0x0473, "ErrGtOp"},
    {0x0499, "ErrAnd"},
    {0x04b8, "ErrNot"},
    {0x04d7, "ErrRmemOp"},
    {0x04ee, "ErrWmemOp"},
    {0x0505, "TestCallOp"},
    {0x0509, "ErrCallOp"},
    {0x0520, "ErrModulo"},
    {0x0565, "ErrHitchhiking"},
    {0x0586, "ErrMultOp"},
    {0x059d, "ErrModOp"},
    
    {0x05f8, "OutputR0"},
    {0x084d, "Unhash"},

    {0x06bb, "LBL_30"},
    {0x05ee, "LBL_34"},
    {0x05b2, "LBL_35"},
    {0x0aae, "LBL_36"},
    {0x05e3, "LBL_39"},
    {0x8005, "LBL_40"},
    {0x05c8, "LBL_41"},
    

    {0x061b, "LBL_43"},
    {0x061e, "LBL_44"},
    {0x0607, "LBL_45"},
    {0x0652, "LBL_46"},
    {0x066d, "LBL_47"},
    {0x0683, "LBL_48"},
    {0x0682, "LBL_49"},
    {0x06af, "LBL_50"},
    {0x06aa, "LBL_51"},
    {0x06b2, "LBL_52"},
    {0x06c2, "LBL_53"},
    {0x0718, "LBL_54"},
    {0x06fb, "LBL_55"},
    {0x0726, "LBL_56"},
    {0x071b, "LBL_57"},
    {0x073c, "LBL_58"},
    {0x07a0, "LBL_59"},
    {0x075e, "LBL_60"},
    {0x0758, "LBL_61"},
    {0x082c, "LBL_62"},
    {0x07f8, "LBL_63"},
    {0x080c, "LBL_64"},
    {0x07ff, "LBL_65"},
    {0x07e3, "LBL_66"},
    {0x0833, "LBL_67"},
    {0x0840, "LBL_68"},
    {0x08c1, "LBL_69"},
    {0x089c, "LBL_70"},
    {0x08b9, "LBL_71"},
    {0x08a2, "LBL_72"},
    {0x08bc, "LBL_73"},
    {0x08e4, "LBL_74"},
    {0x08cc, "LBL_75"},
    {0x0905, "LBL_76"},
    {0x0908, "LBL_77"},
    {0x08f8, "LBL_78"},
    {0x0ad2, "LBL_79"},
    {0x0ae7, "LBL_80"},
    {0x0b94, "LBL_81"},
    {0x06e7, "LBL_82"},
    {0x0623, "LBL_83"},
    {0x0b20, "LBL_84"},
    {0x0634, "LBL_85"},
    {0x0b45, "LBL_86"},
    {0x0b80, "LBL_87"},
    {0x0b6c, "LBL_88"},
    {0x0b7b, "LBL_89"},
    {0x0b86, "LBL_90"},
    {0x8001, "LBL_91"},
    {0x0ab6, "LBL_92"},
    {0x0bd8, "LBL_93"},
    {0x1721, "LBL_94"},
    {0x0bbe, "LBL_95"},
    {0x1766, "LBL_96"},
    {0x0ca6, "LBL_97"},
    {0x0c20, "LBL_98"},
    {0x16bf, "LBL_99"},
    {0x0c4a, "LBL_100"},
    {0x16f4, "LBL_101"},
    {0x0c75, "LBL_102"},
    {0x0c79, "LBL_103"},
    {0x07d1, "LBL_104"},
    {0x0c90, "LBL_105"},
    {0x0ce6, "LBL_106"},
    {0x0cfe, "LBL_107"},
    {0x0d81, "LBL_108"},
    {0x0d99, "LBL_109"},
    {0x0dd3, "LBL_110"},
    {0x0deb, "LBL_111"},
    {0x0e11, "LBL_112"},
    {0x0e2b, "LBL_113"},
    {0x0e43, "LBL_114"},
    {0x0e6e, "LBL_115"},
    {0x0e8f, "LBL_116"},
    {0x0731, "LBL_117"},
    {0x0f64, "LBL_118"},
    {0x107a, "LBL_119"},
    {0x10b7, "LBL_120"},
    {0x11b5, "LBL_121"},
    {0x1203, "LBL_122"},
    {0x10b0, "LBL_123"},
    {0x1135, "LBL_124"},
    {0x112e, "LBL_125"},
    {0x8002, "LBL_126"},
    {0x1127, "LBL_127"},
    {0x1111, "LBL_128"},
    {0x1121, "LBL_129"},
    {0x1234, "LBL_130"},
    {0x1152, "LBL_131"},
    {0x08e9, "LBL_132"},
    {0x08c8, "LBL_133"},
    {0x11a3, "LBL_134"},
    {0x1200, "LBL_135"},
    {0x11de, "LBL_136"},
    {0x11f4, "LBL_137"},
    {0x11d5, "LBL_138"},
    {0x1231, "LBL_139"},
    {0x1225, "LBL_140"},
    {0x122a, "LBL_141"},
    {0x126d, "LBL_142"},
    {0x12fa, "LBL_143"},
    {0x1312, "LBL_144"},
    {0x1399, "LBL_145"},
    {0x14d8, "LBL_146"},
    {0x14c0, "LBL_147"},
    {0x1371, "LBL_148"},
    {0x15e5, "LBL_149"},
    {0x178b, "LBL_150"},
    {0x15cb, "LBL_151"},
    {0x1652, "LBL_152"},
    {0x1604, "LBL_153"},
    {0x16ef, "LBL_154"},
    {0x171e, "LBL_155"},
    {0x16b6, "LBL_156"},
    {0x1744, "LBL_157"},
    {0x1747, "LBL_158"},
    {0x1765, "LBL_159"},
    {0x1783, "LBL_160"},
    {0x1786, "LBL_161"},
    {0x1793, "LBL_162"},
    {0x17a0, "LBL_163"},
};


void dumpLabels()
{
    fprintf(stderr, "Label labels[%d] = \n{\n", labelCount);
    
    for (unsigned int i = 0; i < labelCount; ++i)
    {
        fprintf(stderr, "    {0x%04x, \"%s\"}, \n", labels[i].address, labels[i].name);
    }

    fprintf(stderr, "};\n");
}

Label *labelAtAddress(const unsigned int address)
{
    for (unsigned int i = 0; i < labelCount; ++i)
    {
        if (labels[i].address == address)
        {
            return &labels[i];
        }
    }

    return NULL;
}


void scanForLabels(const unsigned int addr, const unsigned int end)
{
    for (unsigned int a = addr; a < end;)
    {
        unsigned int address = 0;
        if (mem[a] == 6 || mem[a] == 17)
        {
            address = mem[a + 1];
        }
        else if (mem[a] == 7 || mem[a] == 8)
        {
            address = mem[a + 2];
        }

        if (address && labelAtAddress(address) == NULL)
        {
            labels[labelCount].address = address;
            sprintf(labels[labelCount].name, "LBL_%d", labelCount);
            
            ++labelCount;
        }
        
        a += instructionLength(mem[a]);
    }
}

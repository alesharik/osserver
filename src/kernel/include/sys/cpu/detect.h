#include <stdbool.h>

typedef enum {
    CPU_UNKNOWN,
    CPU_OLDAMD,
    CPU_AMD,
    CPU_INTEL,
    CPU_OLDTRANSMETA,
    CPU_TRANSMETA,
    CPU_CYRIX,
    CPU_CENTAUR,
    CPU_NEXGEN,
    CPU_UMC,
    CPU_SIS,
    CPU_NSC,
    CPU_RISE,
    CPU_VORTEX,
    CPU_VIA,
    CPU_VMWARE,
    CPU_XENHVM,
    CPU_MICROSOFT_HV,
    CPU_PARALLELS
} cpu_vendor;

typedef enum {
    ECX_SSE3              = 1 << 0,// Streaming SIMD Extensions 3
    ECX_PCLMUL            = 1 << 1,// PCLMULQDQ Instruction
    ECX_DTES64            = 1 << 2,// 64-Bit Debug Store Area
    ECX_MONITOR           = 1 << 3,// MONITOR/MWAIT
    ECX_DS_CPL            = 1 << 4,// CPL Qualified Debug Store
    ECX_VMX               = 1 << 5,// Virtual Machine Extensions
    ECX_SMX               = 1 << 6,// Safer Mode Extensions
    ECX_EST               = 1 << 7,// Enhanced SpeedStep Technology
    ECX_TM2               = 1 << 8,// Thermal Monitor 2
    ECX_SSSE3             = 1 << 9,// Supplemental Streaming SIMD Extensions 3
    ECX_CID               = 1 << 10,// L1 Context ID
    ECX_FMA               = 1 << 12,// Fused Multiply Add
    ECX_CX16              = 1 << 13,// CMPXCHG16B Instruction
    ECX_ETPRD             = 1 << 14,// xTPR Update Control
    ECX_PDCM              = 1 << 15,// Perf/Debug Capability MSR
    ECX_PCIDE             = 1 << 17,// Process-context Identifiers
    ECX_DCA               = 1 << 18,// Direct Cache Access
    ECX_SSE4_1            = 1 << 19,// Streaming SIMD Extensions 4.1
    ECX_SSE4_2            = 1 << 20,// Streaming SIMD Extensions 4.2
    ECX_x2APIC            = 1 << 21,// Extended xAPIC Support
    ECX_MOVBE             = 1 << 22,// MOVBE Instruction
    ECX_POPCNT            = 1 << 23,// POPCNT Instruction
    ECX_APIC              = 1 << 24,// Local APIC supports TSC Deadline
    ECX_AES               = 1 << 25,// AESNI Instruction
    ECX_XSAVE             = 1 << 26,// XSAVE/XSTOR States
    ECX_OSXSAVE           = 1 << 27,// OS Enabled Extended State Management
    ECX_AVX               = 1 << 28,// AVX Instructions
    ECX_F16C              = 1 << 29,// 16-bit Floating Point Instructions
    ECX_RDRAND            = 1 << 30,// RDRAND Instruction

    EDX_FPU               = 1 << 0,// Floating-Point Unit On-Chip
    EDX_VME               = 1 << 1,// Virtual 8086 Mode Extensions
    EDX_DE                = 1 << 2,// Debugging Extensions
    EDX_PSE               = 1 << 3,// Page Size Extension
    EDX_TSC               = 1 << 4,// Time Stamp Counter
    EDX_MSR               = 1 << 5,// Model Specific Registers
    EDX_PAE               = 1 << 6,// Physical Address Extension
    EDX_MCE               = 1 << 7,// Machine-Check Exception
    EDX_CX8               = 1 << 8,// CMPXCHG8 Instruction
    EDX_APIC              = 1 << 9,// APIC On-Chip
    EDX_SEP               = 1 << 11,// SYSENTER/SYSEXIT instructions
    EDX_MTRR              = 1 << 12,// Memory Type Range Registers
    EDX_PGE               = 1 << 13,// Page Global Bit
    EDX_MCA               = 1 << 14,// Machine-Check Architecture
    EDX_CMOV              = 1 << 15,// Conditional Move Instruction
    EDX_PAT               = 1 << 16,// Page Attribute Table
    EDX_PSE36             = 1 << 17,// 36-bit Page Size Extension
    EDX_PSN               = 1 << 18,// Processor Serial Number
    EDX_CLF               = 1 << 19,// CLFLUSH Instruction
    EDX_DTES              = 1 << 21,// Debug Store
    EDX_ACPI              = 1 << 22,// Thermal Monitor and Software Clock Facilities
    EDX_MMX               = 1 << 23,// MMX Technology
    EDX_FXSR              = 1 << 24,// FXSAVE and FXSTOR Instructions
    EDX_SSE               = 1 << 25,// Streaming SIMD Extensions
    EDX_SSE2              = 1 << 26,// Streaming SIMD Extensions 2
    EDX_SS                = 1 << 27,// Self Snoop
    EDX_HTT               = 1 << 28,// Multi-Threading
    EDX_TM1               = 1 << 29,// Thermal Monitor
    EDX_PBE               = 1 << 31,// Pending Break Enable

    EXTENDED_EDX_SYSCALL  = 1 << 11,// SYSCALL/SYSRET
    EXTENDED_EDX_XD       = 1 << 20,// Execute Disable Bit
    EXTENDED_EDX_1GB_PAGE = 1 << 26,// 1 GB Pages
    EXTENDED_EDX_RDTSCP   = 1 << 27,// RDTSCP and IA32_TSC_AUX
    EXTENDED_EDX_64_BIT   = 1 << 29// 64-bit Architecture
} cpu_feature;

typedef struct {
    cpu_vendor vendor;
    unsigned int ecx_features;
    unsigned int edx_features;
    unsigned int extended_features;
    bool has_name;
    char name[48];
} cpu_info_struct;

cpu_info_struct cpu_info;

void cpu_init_info(void);

const char *cpu_vendor_name(cpu_vendor vendor);

const char *cpu_ecx_feature_name(cpu_feature feature);

const char *cpu_edx_feature_name(cpu_feature feature);

const char *cpu_extended_feature_name(cpu_feature feature);
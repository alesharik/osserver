#include "sys/cpu/detect.h"
#include <cpuid.h>

#define signature_VMWARE_ebx 0x61774d56
#define signature_VMWARE_ecx 0x4d566572
#define signature_VMWARE_edx 0x65726177

#define signature_XENHVM_ebx 0x566e6558
#define signature_XENHVM_ecx 0x65584d4d
#define signature_XENHVM_edx 0x4d4d566e

#define signature_MICROSOFT_HV_ebx 0x6372694d
#define signature_MICROSOFT_HV_ecx 0x666f736f
#define signature_MICROSOFT_HV_edx 0x76482074

#define signature_PARALLELS_ebx 0x70726c20
#define signature_PARALLELS_ecx 0x68797065
#define signature_PARALLELS_edx 0x20767220

#define signature_OLDAMD_ebx 0x414d4469
#define signature_OLDAMD_ecx 0x73626574
#define signature_OLDAMD_edx 0x74657221

cpu_info_struct cpu_info = {
        .vendor = CPU_UNKNOWN,
        .ecx_features = 0,
        .edx_features = 0,
        .extended_features = 0,
        .has_name = false,
        .name = ""
};

void cpu_init_info(void) {
    unsigned int standardFunc = 0;
    unsigned int ebx, ecx, edx = 0;
    if (__get_cpuid(0, &standardFunc, &ebx, &ecx, &edx) == 0)
        return;

    if (ebx == signature_OLDAMD_ebx && ecx == signature_OLDAMD_ecx && edx == signature_OLDAMD_edx)
        cpu_info.vendor = CPU_OLDAMD;
    else if (ebx == signature_AMD_ebx && ecx == signature_AMD_ecx && edx == signature_AMD_edx)
        cpu_info.vendor = CPU_AMD;
    else if (ebx == signature_CENTAUR_ebx && ecx == signature_CENTAUR_ecx && edx == signature_CENTAUR_edx)
        cpu_info.vendor = CPU_CENTAUR;
    else if (ebx == signature_CYRIX_ebx && ecx == signature_CYRIX_ecx && edx == signature_CYRIX_edx)
        cpu_info.vendor = CPU_CYRIX;
    else if (ebx == signature_INTEL_ebx && ecx == signature_INTEL_ecx && edx == signature_INTEL_edx)
        cpu_info.vendor = CPU_INTEL;
    else if (ebx == signature_TM1_ebx && ecx == signature_TM1_ecx && edx == signature_TM1_edx)
        cpu_info.vendor = CPU_OLDTRANSMETA;
    else if (ebx == signature_TM2_ebx && ecx == signature_TM2_ecx && edx == signature_TM2_edx)
        cpu_info.vendor = CPU_TRANSMETA;
    else if (ebx == signature_NSC_ebx && ecx == signature_NSC_ecx && edx == signature_NSC_edx)
        cpu_info.vendor = CPU_NSC;
    else if (ebx == signature_NEXGEN_ebx && ecx == signature_NEXGEN_ecx && edx == signature_NEXGEN_edx)
        cpu_info.vendor = CPU_NEXGEN;
    else if (ebx == signature_RISE_ebx && ecx == signature_RISE_ecx && edx == signature_RISE_edx)
        cpu_info.vendor = CPU_RISE;
    else if (ebx == signature_SIS_ebx && ecx == signature_SIS_ecx && edx == signature_SIS_edx)
        cpu_info.vendor = CPU_SIS;
    else if (ebx == signature_UMC_ebx && ecx == signature_UMC_ecx && edx == signature_UMC_edx)
        cpu_info.vendor = CPU_UMC;
    else if (ebx == signature_VIA_ebx && ecx == signature_VIA_ecx && edx == signature_VIA_edx)
        cpu_info.vendor = CPU_VIA;
    else if (ebx == signature_VORTEX_ebx && ecx == signature_VORTEX_ecx && edx == signature_VORTEX_edx)
        cpu_info.vendor = CPU_VORTEX;
    else if (ebx == signature_VMWARE_ebx && ecx == signature_VMWARE_ecx && edx == signature_VMWARE_edx)
        cpu_info.vendor = CPU_VMWARE;
    else if (ebx == signature_XENHVM_ebx && ecx == signature_XENHVM_ecx && edx == signature_XENHVM_edx)
        cpu_info.vendor = CPU_XENHVM;
    else if (ebx == signature_MICROSOFT_HV_ebx && ecx == signature_MICROSOFT_HV_ecx &&
             edx == signature_MICROSOFT_HV_edx)
        cpu_info.vendor = CPU_MICROSOFT_HV;
    else if (ebx == signature_PARALLELS_ebx && ecx == signature_PARALLELS_ecx && edx == signature_PARALLELS_edx)
        cpu_info.vendor = CPU_PARALLELS;

    if (standardFunc >= 0x01) {
        unsigned int eax = 0;
        __get_cpuid(0x01, &eax, &ebx, &ecx, &edx);
        cpu_info.ecx_features = ecx;
        cpu_info.edx_features = edx;
    }

    unsigned int extendedFunc = 0;
    if (__get_cpuid(0x80000000, &extendedFunc, &ebx, &ecx, &edx) == 1) {
        if (extendedFunc >= 0x80000001) {
            unsigned int eax = 0;
            __get_cpuid(0x80000001, &eax, &ebx, &ecx, &edx);
            cpu_info.extended_features = edx;

        }
        if (extendedFunc >= 0x80000004) {
            __get_cpuid(0x80000002, (unsigned int *)(cpu_info.name + 0), (unsigned int *)(cpu_info.name + 4), (unsigned int *)(cpu_info.name + 8), (unsigned int *)(cpu_info.name + 12));
            __get_cpuid(0x80000003, (unsigned int *)(cpu_info.name + 16), (unsigned int *)(cpu_info.name + 20), (unsigned int *)(cpu_info.name + 24), (unsigned int *)(cpu_info.name + 28));
            __get_cpuid(0x80000004, (unsigned int *)(cpu_info.name + 32), (unsigned int *)(cpu_info.name + 36), (unsigned int *)(cpu_info.name + 40), (unsigned int *)(cpu_info.name + 44));
            cpu_info.has_name = true;
        }
    }
}

const char *cpu_vendor_name(cpu_vendor vendor) {
    switch (vendor) {
        case CPU_OLDAMD:
            return "OldAMD";
        case CPU_AMD:
            return "AMD";
        case CPU_CENTAUR:
            return "Centaur";
        case CPU_CYRIX:
            return "Cyrix";
        case CPU_INTEL:
            return "Intel";
        case CPU_OLDTRANSMETA:
            return "OldTransmeta";
        case CPU_TRANSMETA:
            return "Transmeta";
        case CPU_NSC:
            return "NSC";
        case CPU_NEXGEN:
            return "Nexgen";
        case CPU_RISE:
            return "Rise";
        case CPU_SIS:
            return "SIS";
        case CPU_UMC:
            return "UMC";
        case CPU_VIA:
            return "VIA";
        case CPU_VORTEX:
            return "Vortex";
        case CPU_VMWARE:
            return "VMware";
        case CPU_XENHVM:
            return "XEN";
        case CPU_MICROSOFT_HV:
            return "Microsoft HV";
        case CPU_PARALLELS:
            return "Parallels";
        default:
            return "Unknown";
    }
}

const char *cpu_ecx_feature_name(cpu_feature feature) {
    switch (feature) {
        case ECX_SSE3:
            return "SSE3";
        case ECX_PCLMUL:
            return "PCLMUL";
        case ECX_DTES64:
            return "DTES64";
        case ECX_MONITOR:
            return "MONITOR";
        case ECX_DS_CPL:
            return "DS_CPL";
        case ECX_VMX:
            return "VMX";
        case ECX_SMX:
            return "SMX";
        case ECX_EST:
            return "EST";
        case ECX_TM2:
            return "TM2";
        case ECX_SSSE3:
            return "SSSE3";
        case ECX_CID:
            return "CID";
        case ECX_FMA:
            return "FMA";
        case ECX_CX16:
            return "CX16";
        case ECX_ETPRD:
            return "ETPRD";
        case ECX_PDCM:
            return "PDCM";
        case ECX_PCIDE:
            return "PCIDE";
        case ECX_DCA:
            return "DCA";
        case ECX_SSE4_1:
            return "SSE4_1";
        case ECX_SSE4_2:
            return "SSE4_2";
        case ECX_x2APIC:
            return "X2APIC";
        case ECX_MOVBE:
            return "MOVBE";
        case ECX_POPCNT:
            return "POPCNT";
        case ECX_APIC:
            return "APIC";
        case ECX_AES:
            return "AES";
        case ECX_XSAVE:
            return "XSAVE";
        case ECX_OSXSAVE:
            return "OSXSAVE";
        case ECX_AVX:
            return "AVX";
        case ECX_F16C:
            return "F16C";
        case ECX_RDRAND:
            return "RDRAND";
    }
}

const char *cpu_edx_feature_name(cpu_feature feature) {
    switch (feature) {
        case EDX_FPU:
            return "FPU";
        case EDX_VME:
            return "VME";
        case EDX_DE:
            return "DE";
        case EDX_PSE:
            return "PSE";
        case EDX_TSC:
            return "TSC";
        case EDX_MSR:
            return "MSR";
        case EDX_PAE:
            return "PAE";
        case EDX_CX8:
            return "CX8";
        case EDX_MCE:
            return "MCE";
        case EDX_APIC:
            return "APIC";
        case EDX_SEP:
            return "SEP";
        case EDX_MTRR:
            return "MTRR";
        case EDX_PGE:
            return "PGE";
        case EDX_MCA:
            return "MCA";
        case EDX_CMOV:
            return "CMOV";
        case EDX_PAT:
            return "PAT";
        case EDX_PSE36:
            return "PSE36";
        case EDX_PSN:
            return "PSN";
        case EDX_CLF:
            return "CLF";
        case EDX_DTES:
            return "DTES";
        case EDX_ACPI:
            return "ACPI";
        case EDX_MMX:
            return "MMX";
        case EDX_FXSR:
            return "FXSR";
        case EDX_SSE:
            return "SSE";
        case EDX_SSE2:
            return "SSE2";
        case EDX_SS:
            return "SS";
        case EDX_HTT:
            return "HTT";
        case EDX_TM1:
            return "TM1";
        case EDX_PBE:
            return "PBE";
        default:
            return "Unknown";
    }
}

const char *cpu_extended_feature_name(cpu_feature feature) {
    switch (feature) {
        case EXTENDED_EDX_SYSCALL:
            return "SYSCALL";
        case EXTENDED_EDX_XD:
            return "XD";
        case EXTENDED_EDX_1GB_PAGE:
            return "1GB PAGES";
        case EXTENDED_EDX_RDTSCP:
            return "RDTSCP";
        case EXTENDED_EDX_64_BIT:
            return "64 bit";
        default:
            return "Unknown";
    }
}

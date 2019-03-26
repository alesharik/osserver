#include "sys/acpi/acpi.h"
#include <stdint.h>
#include <stdbool.h>
#include "sys/kernel.h"
#include "string.h"
#include "sys/asm.h"

unsigned int acpi_cpu_count;
unsigned char acpi_cpu_ids[MAX_CPU_COUNT];
apic_info_struct acpi_apic_info = {.lapic_addr = 0, .ioapic_size = 0};

typedef struct {
    uint32_t signature;
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oem[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute((packed)) acpi_header;

typedef struct {
    uint8_t address_space;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t access_size;
    uint64_t address;
} __attribute((packed)) generic_address_structure;

typedef struct {
    acpi_header header;
    uint32_t firmwareControl;
    uint32_t dsdt;
    uint8_t unused;
    uint8_t preferred_power_management_profile;
    uint16_t sci_interrupt;
    uint32_t smi_command_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4_bios_req;
    uint8_t pstate_control;
    uint32_t pm1_a_event_block;
    uint32_t pm1_b_event_block;
    uint32_t pm1_a_control_block;
    uint32_t pm1_b_control_block;
    uint32_t pm2_control_block;
    uint32_t pmtimer_block;
    uint32_t gpe0_block;
    uint32_t gpe1_block;
    uint8_t pm1_event_length;
    uint8_t pm1_control_length;
    uint8_t pm2_control_length;
    uint8_t pmtimer_length;
    uint8_t gpe0_length;
    uint8_t gpe1_length;
    uint8_t gpe1_base;
    uint8_t cstate_control;
    uint16_t worst_c2_latency;
    uint16_t worst_c3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;

    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t boot_architecture_flags;

    uint8_t reserved2;
    uint32_t flags;

    // 12 byte structure; see below for details
    generic_address_structure reset_reg;

    uint8_t reset_value;
    uint8_t reserved3[3];

    // 64bit pointers - Available on ACPI 2.0+
    uint64_t x_firmware_control;
    uint64_t x_dsdt;

    generic_address_structure x_pm1_a_event_block;
    generic_address_structure x_pm1_b_event_block;
    generic_address_structure x_pm1_a_control_block;
    generic_address_structure x_pm1_b_control_block;
    generic_address_structure x_pm2_control_block;
    generic_address_structure x_pmtimer_block;
    generic_address_structure x_gpe0_block;
    generic_address_structure x_gpe1_block;
} __attribute((packed)) acpi_fadt;

typedef struct {
    acpi_header header;
    uint32_t localApicAddr;
    uint32_t flags;
} __attribute((packed)) acpi_madt;


typedef struct {
    uint8_t type;
    uint8_t length;
} __attribute((packed)) apic_header;

enum apic_header_type {
    LOCAL_APIC = 0,
    IO_APIC = 1,
    INTERRUPT_OVERRIDE = 2
};

typedef struct {
    apic_header header;
    uint8_t acpi_processor_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute((packed)) apic_local_apic;


typedef struct {
    apic_header header;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_address;
    uint32_t global_system_interrupt_base;
} __attribute((packed)) apic_io_apic;


typedef struct {
    apic_header header;
    uint8_t bus;
    uint8_t source;
    uint32_t interrupt;
    uint16_t flags;
} __attribute((packed)) apic_interrupt_override;

acpi_madt *acpi_madt_struct;

static void acpi_parse_fadt(acpi_fadt *fadt) {
    if (fadt->smi_command_port) {
//        outb(fadt->smi_command_port, fadt->acpi_enable);
    } else {
        //FIXME print acpi already enabled
    }
}

static void acpi_parse_apic(acpi_madt *madt) {
    acpi_madt_struct = madt;

    acpi_apic_info.lapic_addr = madt->localApicAddr;
    uint8_t *start = (uint8_t *) (acpi_madt_struct + 1);
    uint8_t *end = (uint8_t *) acpi_madt_struct + acpi_madt_struct->header.length;

    while (start < end) {
        apic_header *header = (apic_header *) start;
        uint8_t type = header->type;
        uint8_t length = header->length;

        if (type == LOCAL_APIC) {
            apic_local_apic *s = (apic_local_apic *) start;

//         FIXME log cpu
            if (acpi_cpu_count < MAX_CPU_COUNT) {
                acpi_cpu_ids[acpi_cpu_count] = s->apic_id;
                acpi_cpu_count++;
            }
        } else if (type == IO_APIC) {
            if (acpi_apic_info.ioapic_size > MAX_IOAPIC_COUNT)
                continue;
            apic_io_apic *s = (apic_io_apic *) start;
            //FIXME log
            acpi_apic_info.ioapic_addrs[acpi_apic_info.ioapic_size] = s->io_apic_address;
            acpi_apic_info.ioapic_size++;
        } else if (type == INTERRUPT_OVERRIDE) {
            apic_interrupt_override *s = (apic_interrupt_override *) start;
            //FIXME log
        } else {
//            FIXME report unknown
        }

        start += length;
    }
}

static void acpi_parse_dt(acpi_header *header) {
    uint32_t signature = header->signature;

//    char sigStr[5]; FIXME log
//    memcpy(sigStr, &signature, 4);
//    sigStr[4] = 0;

    if (signature == 0x50434146)
        acpi_parse_fadt((acpi_fadt *) header);
    else if (signature == 0x43495041)
        acpi_parse_apic((acpi_madt *) header);
}

static void acpi_parse_rsdt(acpi_header *rsdt) {
    uint32_t *start = (uint32_t *) (rsdt + 1);
    uint32_t *end = (uint32_t *) ((uint8_t *) rsdt + rsdt->length);

    while (start < end) {
        uint32_t address = *start++;
        acpi_parse_dt((acpi_header *) address);
    }
}

static void acpi_parse_xsdt(acpi_header *xsdt) {
    uint64_t *start = (uint64_t *) (xsdt + 1);
    uint64_t *end = (uint64_t *) ((uint8_t *) xsdt + xsdt->length);

    while (start < end) {
        uint64_t address = *start++;
        acpi_parse_dt((acpi_header *) address);
    }
}

bool acpi_parse_rsdp(uint8_t *ptr) {
    uint8_t sum = 0;
    for (unsigned int i = 0; i < 20; ++i)
        sum += ptr[i];

    if (sum) {
        //FIXME report checksum failed
        return false;
    }

    // Print OEM FIXME log
//    char oem[7];
//    memcpy(oem, p + 9, 6);
//    oem[6] = '\0';

    // Check version
    uint8_t revision = ptr[15];
    if (revision == 0) {
        uint32_t rsdtAddr = *(uint32_t *) (ptr + 16);
        acpi_parse_rsdt((acpi_header *) rsdtAddr);
    } else if (revision == 2) {
        uint32_t rsdtAddr = *(uint32_t *) (ptr + 16);
        uint64_t xsdtAddr = *(uint64_t *) (ptr + 24);

        if (xsdtAddr)
            acpi_parse_xsdt((acpi_header *) xsdtAddr);
        else
            acpi_parse_rsdt((acpi_header *) rsdtAddr);
    } else
        return false;//FIXME

    return true;
}

static int check_rsdp(uint64_t address) {
    uint8_t sum = 0;
    for (int i = 0; i < 20; i++)
        sum += *(uint8_t *) (address + i);
    return sum;
}

static uint64_t search_rsdp_bios() {
    const char *signature = "RSD PTR ";
    for (uint64_t offset = 0; offset < 0x20000; offset += 16) {
        if (memcmp((const char *) (0xe0000 + offset), signature, 8) == 0) {
            if (check_rsdp(0xe0000 + offset))
                continue;
            return 0xe0000 + offset;
        }
    }
    return 0;
}

static uint64_t search_rsdp_ebda() {
    const char *signature = "RSD PTR ";
    uint64_t ebda_start = (*(uint16_t *) (0x40e)) << 4;
    for (uint64_t offset = 0; offset < 0x400; offset += 16) {
        if (memcmp((const char *) (ebda_start + offset), signature, 8) == 0) {
            if (check_rsdp(ebda_start + offset))
                continue;
            return ebda_start + offset;
        }
    }
    return 0;
}

void acpi_init() {
    uint64_t ptr = search_rsdp_bios();
    if(ptr == 0)
        ptr = search_rsdp_ebda();
    if(ptr == 0)
        kpanic("Cannot find ACPI tables");
    acpi_parse_rsdp(ptr);
}

unsigned int acpi_remap_irq(unsigned int irq) {
    uint8_t *start = (uint8_t *) (acpi_madt_struct + 1);
    uint8_t *end = (uint8_t *) acpi_madt_struct + acpi_madt_struct->header.length;

    while (start < end) {
        apic_header *header = (apic_header *) start;
        uint8_t type = header->type;
        uint8_t length = header->length;

        if (type == INTERRUPT_OVERRIDE) {
            apic_interrupt_override *s = (apic_interrupt_override *) start;
            if (s->source == irq)
                return s->interrupt;
        }

        start += length;
    }

    return irq;
}
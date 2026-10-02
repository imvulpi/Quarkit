#include <stdlib.h>
#include <stdio.h>
#if defined(_WIN32)
    #include "win_dirent.h"
#else
    #include <dirent.h>
#endif
#include "builder/discovery.h"
#include "builder/common.h"
#include "kvec.h"

const char* qkit_system_to_string(qkit_system system)
{
    switch (system) {
        case QKIT_SYS_MACOS:   return "macos";
        case QKIT_SYS_WINDOWS: return "windows";
        case QKIT_SYS_LINUX:   return "linux";
        default:               return "unknown";
    }
}

const char* qkit_arch_to_string(qkit_architecture arch)
{
    switch (arch) {
        case QKIT_ARCH_X86:   return "x86";
        case QKIT_ARCH_ARM:   return "arm";
        case QKIT_ARCH_RISCV: return "riscv";
        default:              return "unknown";
    }
}

void qkit_triple_to_string(const qkit_triple *triple, char *buf, size_t size)
{
    if (triple == NULL || buf == NULL || size == 0) return;

    snprintf(buf, size, "%s_%d-%s",
             qkit_arch_to_string(triple->architecture),
             triple->bitness,
             qkit_system_to_string(triple->system));
}

void qkit_print_discovery(const qkit_discovery *discovery)
{
    if (discovery == NULL) return;

    char triple_str[64] = "unknown";
    if (discovery->triple) {
        qkit_triple_to_string(discovery->triple, triple_str, sizeof(triple_str));
    }

    printf("\n");
    printf("========================================\n");
    printf(" Discovered Payload Item:\n");
    printf("========================================\n");
    printf("  * name         : %s\n", discovery->name.s ? discovery->name.s : "(null)");
    printf("  * system       : %s\n", discovery->triple ? qkit_system_to_string(discovery->triple->system) : "unknown");
    printf("  * architecture : %s\n", discovery->triple ? qkit_arch_to_string(discovery->triple->architecture) : "unknown");
    printf("  * bitness      : %d-bit\n", discovery->triple ? discovery->triple->bitness : 0);
    printf("  * target triple: %s\n", triple_str);
    printf("========================================\n\n");
}

void qkit_print_discoveries(const qkit_discovery_vec *discovery_vec)
{
    if (discovery_vec == NULL) return;

    size_t count = kv_size(*discovery_vec);
    printf("========================================\n");
    printf(" Discovered Payloads Summary (%zu items):\n", count);
    printf("========================================\n");

    for (size_t i = 0; i < count; ++i) {
        qkit_discovery *discovery = discovery_vec->a[i];
        if (discovery == NULL) continue;

        char triple_str[64] = "unknown";
        if (discovery->triple) {
            qkit_triple_to_string(discovery->triple, triple_str, sizeof(triple_str));
        }

        printf(" [%zu] %-30s -> %s\n", 
               i + 1, 
               discovery->name.s ? discovery->name.s : "(null)", 
               triple_str);
    }
    printf("========================================\n\n");
}

qkit_system qkit_detect_system(const char *lower_name)
{
    if (lower_name == NULL) return QKIT_SYS_UNKNOWN;

    /* macOS / Darwin / Apple (Checked BEFORE "win" to avoid matching 'darWIN') */
    if (strstr(lower_name, "darwin") || 
        strstr(lower_name, "macos")  || 
        strstr(lower_name, "apple")  || 
        strstr(lower_name, "osx")    || 
        strstr(lower_name, "mac")) {
        return QKIT_SYS_MACOS;
    }

    /* Windows / MinGW / Cygwin / MSVC */
    if (strstr(lower_name, "windows") || 
        strstr(lower_name, "win")     || 
        strstr(lower_name, "mingw")   || 
        strstr(lower_name, "cygwin")  || 
        strstr(lower_name, "msvc")) {
        return QKIT_SYS_WINDOWS;
    }

    /* Linux / GNU / Musl */
    if (strstr(lower_name, "linux") || 
        strstr(lower_name, "gnu")   || 
        strstr(lower_name, "musl")  || 
        strstr(lower_name, "lin")) {
        return QKIT_SYS_LINUX;
    }

    return QKIT_SYS_UNKNOWN;
}

void qkit_detect_arch_and_bitness(const char *lower_name, qkit_architecture *out_arch, int *out_bitness)
{
    qkit_architecture arch = QKIT_ARCH_UNKNOWN;
    int bitness = 32;

    if (lower_name == NULL) {
        if (out_arch) *out_arch = arch;
        if (out_bitness) *out_bitness = bitness;
        return;
    }

    /* ARM - 64/32 */
    if (strstr(lower_name, "aarch64") || 
        strstr(lower_name, "arm64")   || 
        strstr(lower_name, "armv8")   || 
        strstr(lower_name, "armv9")) {
        arch = QKIT_ARCH_ARM;
        bitness = 64;
    }
    else if (strstr(lower_name, "arm")   || 
             strstr(lower_name, "aarch") || 
             strstr(lower_name, "armhf") || 
             strstr(lower_name, "armv7") || 
             strstr(lower_name, "armv6")) {
        arch = QKIT_ARCH_ARM;
        bitness = 32;
    } 
    /* RISC-V - 64/32 */
    else if (strstr(lower_name, "riscv64") || strstr(lower_name, "rv64")) {
        arch = QKIT_ARCH_RISCV;
        bitness = 64;
    }
    else if (strstr(lower_name, "riscv32") || strstr(lower_name, "rv32")) {
        arch = QKIT_ARCH_RISCV;
        bitness = 32;
    }
    else if (strstr(lower_name, "riscv") || strstr(lower_name, "risc-v")) {
        arch = QKIT_ARCH_RISCV;
        bitness = strstr(lower_name, "64") ? 64 : 32;
    }
    /* x86 / x64 */
    else if (strstr(lower_name, "x86_64") || 
             strstr(lower_name, "amd64")  || 
             strstr(lower_name, "x64")) {
        arch = QKIT_ARCH_X86;
        bitness = 64;
    }
    else if (strstr(lower_name, "i386") || 
             strstr(lower_name, "i486") || 
             strstr(lower_name, "i586") || 
             strstr(lower_name, "i686") || 
             strstr(lower_name, "ia32") || 
             strstr(lower_name, "x86")  ||
             strstr(lower_name, "386")) {
        arch = QKIT_ARCH_X86;
        bitness = 32;
    }

    if (arch != QKIT_ARCH_UNKNOWN && strstr(lower_name, "64")) bitness = 64;

    if (out_arch) *out_arch = arch;
    if (out_bitness) *out_bitness = bitness;
}

void qkit_detect_triple(const char *lower_name, qkit_triple *out_triple)
{
    if (out_triple == NULL) return;

    out_triple->system = QKIT_SYS_UNKNOWN;
    out_triple->architecture = QKIT_ARCH_UNKNOWN;
    out_triple->bitness = 32;

    if (lower_name == NULL) return;

    out_triple->system = qkit_detect_system(lower_name);
    qkit_detect_arch_and_bitness(lower_name, &out_triple->architecture, &out_triple->bitness);

    // Popular edge-cases for windows.
    if (out_triple->system == QKIT_SYS_WINDOWS && out_triple->architecture == QKIT_ARCH_UNKNOWN) {
        if (strstr(lower_name, "win64")) {
            out_triple->architecture = QKIT_ARCH_X86;
            out_triple->bitness = 64;
        } 
        else if (strstr(lower_name, "win32")) {
            out_triple->architecture = QKIT_ARCH_X86;
            out_triple->bitness = 32;
        }
    }
}

int qkit_discover_payloads(const char *path, qkit_discovery_vec *discoveries)
{
    if (discoveries == NULL) return QKIT_OK;

    DIR *dr = opendir(path);
    TRY_CUSTOM(dr == NULL, QKIT_ERR);

    int status = QKIT_OK;
    struct dirent *de;
    while ((de = readdir(dr)) != NULL) {
        size_t namelen = strlen(de->d_name);
        if ((namelen == 2 && de->d_name[0] == '.' && de->d_name[1] == '.')
        || (namelen == 1 && de->d_name[0] == '.')) continue;

        char lower_name[namelen + 1];
        memcpy(lower_name, de->d_name, namelen + 1);
        strnlwr(lower_name, namelen);

        qkit_triple stack_triple;
        qkit_detect_triple(lower_name, &stack_triple);
        if (stack_triple.system == QKIT_SYS_UNKNOWN || stack_triple.architecture == QKIT_ARCH_UNKNOWN) {
            continue;
        }

        qkit_triple *triple = malloc(sizeof(qkit_triple));
        qkit_discovery *discovery = malloc(sizeof(qkit_discovery));        
        if (triple == NULL || discovery == NULL) {
            free(triple);
            free(discovery);
            status = QKIT_ERR;
            goto cleanup;
        }

        *triple = stack_triple;
        discovery->triple = triple;
        discovery->name = (kstring_t){0};

        if (kputsn(de->d_name, namelen, &discovery->name) == EOF) {
            qkit_discovery_free(discovery);
            status = QKIT_ERR;
            goto cleanup;
        }

        int push_ok = 0;
        qkit_kv_push(qkit_discovery*, *discoveries, discovery, push_ok);
        if (!push_ok) {
            qkit_discovery_free(discovery);
            status = QKIT_ERR;
            goto cleanup;
        }
    }

cleanup:
    closedir(dr);
    return status;
}

void qkit_discovery_free(qkit_discovery *discovery) {
    if (discovery == NULL) return;
    free(discovery->triple);
    free(discovery->name.s);
    free(discovery);
}

void qkit_discovery_free_vec(qkit_discovery_vec *discovery_vec) {
    if (discovery_vec == NULL) return;
    for (size_t i = 0; i < discovery_vec->n; i++) qkit_discovery_free(discovery_vec->a[i]);
    kv_destroy(*discovery_vec);
}

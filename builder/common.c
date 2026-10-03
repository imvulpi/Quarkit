#include <builder/common.h>
#include <stdio.h>
#include <time.h>


int qkit_create_scratch_dir(const qkit_triple *triple, kstring_t *out_path)
{
    if (out_path == NULL) return QKIT_ERR;

    char base_temp[512] = {0};

#if defined(_WIN32)
    DWORD ret = GetTempPathA(sizeof(base_temp), base_temp);
    if (ret == 0 || ret > sizeof(base_temp)) {
        snprintf(base_temp, sizeof(base_temp), "C:\\Windows\\Temp\\");
    }
#else
    const char *env_tmp = getenv("TMPDIR");
    if (!env_tmp) env_tmp = getenv("TMP");
    if (!env_tmp) env_tmp = getenv("TEMP");
    if (!env_tmp) env_tmp = "/tmp";
    snprintf(base_temp, sizeof(base_temp), "%s/", env_tmp);
#endif

    int res = kputs(base_temp, out_path);
    if (res == EOF) return QKIT_ERR;
    if (out_path->s[out_path->l - 1] != '/' && out_path->s[out_path->l - 1] != '\\') {
        kputc('/', out_path);
    }

    kputs("Quarkit/", out_path);
    char triple_buf[64] = "unknown";
    if (triple != NULL) qkit_triple_to_string(triple, triple_buf, 64);

    uint64_t ts = (uint64_t)time(NULL);
    int pid = (int)getpid();

    ksprintf(out_path, "%llu-%d-%s/", (unsigned long long)ts, pid, triple_buf);
    ensure_dir_exists(out_path->s);

    return QKIT_OK;
}
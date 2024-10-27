#include "linux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>

int get_number_of_cores(void) 
{
    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    return (int)cores;
}

float get_cpu_frequency(int core) 
{
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) {
        return -1.0;
    }

    char line[256];
    char search_str[32];
    snprintf(search_str, sizeof(search_str), "cpu MHz");

    float frequency = -1.0;
    int current_core = -1;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "processor", 9) == 0) {
            current_core++;
        }

        if (current_core == core && strstr(line, search_str)) {
            sscanf(line, "cpu MHz\t: %f", &frequency);
            frequency /= 1000.0; 
            break;
        }
    }

    fclose(fp);
    return frequency;
}

float get_cpu_usage(int core)
{
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        return -1.0;
    }

    char line[256];
    char search_str[32];
    snprintf(search_str, sizeof(search_str), "cpu%d", core);

    unsigned long long user, nice, system, idle, iowait, irq, softirq;
    unsigned long long total_time = 0;
    unsigned long long idle_time = 0;

    while (fgets(line, sizeof(line), fp)) 
    {
        if (strncmp(line, search_str, strlen(search_str)) == 0)
         {
            sscanf(line, "%*s %llu %llu %llu %llu %llu %llu %llu",
                   &user, &nice, &system, &idle, &iowait, &irq, &softirq);

            total_time = user + nice + system + idle + iowait + irq + softirq;
            idle_time = idle + iowait;

            break;
        }
    }

    fclose(fp);

    if (total_time == 0) 
    {
        return -1.0;
    }

    static unsigned long long prev_total_time[32] = {0};
    static unsigned long long prev_idle_time[32] = {0};

    unsigned long long diff_total = total_time - prev_total_time[core];
    unsigned long long diff_idle = idle_time - prev_idle_time[core];

    prev_total_time[core] = total_time;
    prev_idle_time[core] = idle_time;

    return diff_total > 0 ? (100.0 * (diff_total - diff_idle) / diff_total) : 0.0;
}

int get_cpu_name(char *name, size_t length) 
{
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) 
    {
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0) {
            char *colon = strchr(line, ':');
            if (colon) {
                strncpy(name, colon + 2, length - 1);
                name[length - 1] = '\0';
                size_t len = strlen(name);
                if (len > 0 && name[len - 1] == '\n') {
                    name[len - 1] = '\0';
                }
            }
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return -1;
}

void get_cpu_info(CPUInfo * info)
{
    memset(info, 0, sizeof(CPUInfo));

    if (get_cpu_name(info->name, sizeof(info->name)) != 0) 
    {
        fprintf(stderr, "Помилка при отриманні назви процесора\n");
    }

    info->num_cores = get_number_of_cores();
    if (info->num_cores < 0) 
    {
        fprintf(stderr, "Помилка при отриманні кількості ядер процесора\n");
    }

    for (int i = 0; i < info->num_cores; ++i) 
    {
        info->frequencies[i] = get_cpu_frequency(i);
        info->usages[i] = get_cpu_usage(i);

        if (info->frequencies[i] < 0 || info->usages[i] < 0) 
        {
            fprintf(stderr, "Помилка при отриманні інформації про ядро %d\n", i);
        }
    }
}

void get_memory_info(MemoryInfo * info)
{
    memset(info, 0, sizeof(MemoryInfo));

    FILE *file = fopen("/proc/meminfo", "r");
    if (file == NULL) 
    {
        perror("Не вдалося відкрити /proc/meminfo");
    }

    long mem_total = 0;
    long mem_free = 0;
    long buffers = 0;
    long cached = 0;

    char line[256];
    while (fgets(line, sizeof(line), file))
     {
        if (sscanf(line, "MemTotal: %ld kB", &mem_total) == 1 ||
            sscanf(line, "MemFree: %ld kB", &mem_free) == 1 ||
            sscanf(line, "Buffers: %ld kB", &buffers) == 1 ||
            sscanf(line, "Cached: %ld kB", &cached) == 1) {
            
        }
    }

    fclose(file);

    if (mem_total == 0)
     {
        fprintf(stderr, "Не вдалося знайти інформацію про загальну пам'ять\n");
    }

    info->total_memory = mem_total;  
    info->used_memory = mem_total - mem_free - buffers - cached;  

}

void get_gpu_info(GPUInfo *info)
{
    memset(info, 0, sizeof(GPUInfo));

    FILE *fp = popen("nvidia-smi --query-gpu=name,utilization.gpu --format=csv,noheader,nounits", "r");
    if (fp == NULL) 
    {
        perror("Не вдалося запустити nvidia-smi");
        return;  
    }

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp) != NULL) 
    {
        char utilization[16];
        
        sscanf(buffer, "%[^,], %15s", info->name, utilization); 

        if (sscanf(utilization, "%f", &info->usage) != 1)
        {
            fprintf(stderr, "Не вдалося зчитати використання GPU\n");
        }
    } 
    else 
    {
        fprintf(stderr, "Не вдалося отримати інформацію про GPU\n");
    }

    pclose(fp);
}

double get_system_uptime() 
{
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp == NULL) {
        perror("Не вдалося відкрити /proc/uptime");
        return -1.0; 
    }

  
    double uptime_seconds;
    if (fscanf(fp, "%lf", &uptime_seconds) != 1) {
        fprintf(stderr, "Не вдалося зчитати аптайм з /proc/uptime\n");
        fclose(fp);
        return -1.0; 
    }
    fclose(fp);

    return uptime_seconds;
}

OS_Info get_os_info() 
{
    OS_Info os_info = {"Невідомо", "Невідомо", "Невідомо", "Невідомо"};  

   
    FILE *fp = fopen("/etc/os-release", "r");
    if (fp == NULL) {
        perror("Не вдалося відкрити /etc/os-release");
        return os_info;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strncmp(line, "NAME=", 5) == 0) {
            sscanf(line, "NAME=\"%[^\"]\"", os_info.os_name);
        } else if (strncmp(line, "VERSION=", 8) == 0) {
            sscanf(line, "VERSION=\"%[^\"]\"", os_info.os_version);
        }
    }

    fclose(fp);

    
    fp = popen("uname -r", "r");
    if (fp == NULL) {
        perror("Не вдалося виконати uname -r");
        return os_info;
    }

    if (fgets(os_info.kernel_version, sizeof(os_info.kernel_version), fp) == NULL) {
        fprintf(stderr, "Не вдалося отримати версію ядра\n");
        pclose(fp);
        return os_info;
    }

    
    os_info.kernel_version[strcspn(os_info.kernel_version, "\n")] = 0;

    pclose(fp);

    
    char *default_shell = getenv("SHELL");
    if (default_shell != NULL) {
        strncpy(os_info.default_shell, default_shell, sizeof(os_info.default_shell) - 1);
    }

    return os_info;
}

MonitorInfo get_monitor_info() 
{
    MonitorInfo monitor_info = {"Unknown", 0, 0}; 

   
    FILE *fp = popen("xrandr --current | grep ' connected'", "r");
    if (fp == NULL) {
        perror("Не вдалося виконати xrandr");
        return monitor_info;
    }

    char line[256];

    
    if (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%127s connected %dx%d", monitor_info.monitor_name, &monitor_info.width, &monitor_info.height);
    } else {
        fprintf(stderr, "Не вдалося отримати інформацію про монітор\n");
        pclose(fp);
        return monitor_info;
    }

    pclose(fp);
    
    return monitor_info;
}

DiskSpaceInfo get_disk_space_info(const char *path) 
{
    DiskSpaceInfo disk_info = {0};  

    struct statvfs stat;

    
    if (statvfs(path, &stat) != 0) {
        perror("Не вдалося отримати інформацію про файлову систему");
        return disk_info;  
    }

   
    unsigned long long total_space = stat.f_blocks * stat.f_frsize;
    unsigned long long free_space = stat.f_bfree * stat.f_frsize;
    unsigned long long used_space = total_space - free_space;

    
    disk_info.total_gb = (double)total_space / (1024 * 1024 * 1024);
    disk_info.free_gb = (double)free_space / (1024 * 1024 * 1024);
    disk_info.used_gb = (double)used_space / (1024 * 1024 * 1024);
    disk_info.used_percent = (double)used_space / total_space * 100;

    
    strncpy(disk_info.path, path, sizeof(disk_info.path) - 1);

    return disk_info;  
}

InternetSpeedInfo get_internet_speed_info() 
{
    InternetSpeedInfo speed_info = {"N/A", "N/A", "N/A"};  

    
    FILE *fp = popen("speedtest-cli --simple", "r");
    if (fp == NULL) {
        perror("Не вдалося виконати speedtest-cli");
        return speed_info; 
    }

    char line[256];

    
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strncmp(line, "Ping:", 5) == 0) {
            sscanf(line, "Ping: %[^\n]", speed_info.ping);
        } else if (strncmp(line, "Download:", 9) == 0) {
            sscanf(line, "Download: %[^\n]", speed_info.download);
        } else if (strncmp(line, "Upload:", 7) == 0) {
            sscanf(line, "Upload: %[^\n]", speed_info.upload);
        }
    }

    pclose(fp);

    return speed_info;  
}
#include "linux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/statvfs.h>

#define KB_TO_BYTES 1

bool get_system_uptime(double *uptime_seconds) 
{
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp == NULL) 
    {
        perror("Failed to open /proc/uptime");
        return false; 
    }

    if (fscanf(fp, "%lf", uptime_seconds) != 1) 
    {
        perror("Failed to read /proc/uptime");
        fclose(fp);
        return false; 
    }
    fclose(fp);

    return true;
}

bool get_cpu_info(CPUInfo_t *info) 
{
    memset(info, 0, sizeof(CPUInfo_t));

    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) 
    {
        perror("Error opening /proc/cpuinfo");
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) 
    {
        if (strncmp(line, "model name", 10) == 0) 
        {
            char *colon = strchr(line, ':');
            if (colon) {
                strncpy(info->name, colon + 2, sizeof(info->name) - 1);
                info->name[strcspn(info->name, "\n")] = '\0'; 
            }
            break;
        }
    }
    fclose(fp);

    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    info->num_cores = (int)cores;

    for (int i = 0; i < info->num_cores; ++i) 
    {
        fp = fopen("/proc/cpuinfo", "r");
        if (!fp) 
        {
            perror("Error opening /proc/cpuinfo");
            return false;
        }

        float frequency = -1.0;
        int current_core = -1;
        while (fgets(line, sizeof(line), fp)) 
        {
            if (strncmp(line, "processor", 9) == 0) 
            {
                current_core++;
            }

            if (current_core == i && strstr(line, "cpu MHz")) 
            {
                sscanf(line, "cpu MHz\t: %f", &frequency);
                frequency /= 1000.0;  
                break;
            }
        }
        fclose(fp);

        info->frequencies[i] = frequency;

        fp = fopen("/proc/stat", "r");
        if (!fp) 
        {
            perror("Error opening /proc/stat");
            return false;
        }

        snprintf(line, sizeof(line), "cpu%d", i);
        unsigned long long user, nice, system, idle, iowait, irq, softirq;
        unsigned long long total_time = 0, idle_time = 0;
        while (fgets(line, sizeof(line), fp)) 
        {
            if (strncmp(line, line, strlen(line)) == 0) 
            {
                sscanf(line, "%*s %llu %llu %llu %llu %llu %llu %llu",
                       &user, &nice, &system, &idle, &iowait, &irq, &softirq);

                total_time = user + nice + system + idle + iowait + irq + softirq;
                idle_time = idle + iowait;
                break;
            }
        }
        fclose(fp);

        static unsigned long long prev_total_time[32] = {0};
        static unsigned long long prev_idle_time[32] = {0};

        unsigned long long diff_total = total_time - prev_total_time[i];
        unsigned long long diff_idle = idle_time - prev_idle_time[i];

        prev_total_time[i] = total_time;
        prev_idle_time[i] = idle_time;

        info->usages[i] = diff_total > 0 ? (100.0 * (diff_total - diff_idle) / diff_total) : 0.0;
    }

    return true;
}

bool get_memory_info(MemoryInfo_t * info)
{
    FILE *file = fopen("/proc/meminfo", "r");

    if (file == NULL || info == NULL)
    {
        perror("Failed to open /proc/meminfo");
        return false;
    }

    memset(info, 0, sizeof(MemoryInfo_t));

    char line[256];
    bool total_found = false, free_found = false, buffers_found = false, cached_found = false;

    while (fgets(line, sizeof(line), file)) 
    {
        if (!total_found && sscanf(line, "MemTotal: %d kB", &info->total_memory) == 1)
        {
            total_found = true;
        } 
        else if (!free_found && sscanf(line, "MemFree: %d kB", &info->mem_free) == 1)
        {
            free_found = true;
        } 
        else if (!buffers_found && sscanf(line, "Buffers: %d kB", &info->buffers) == 1) 
        {
            buffers_found = true;
        } 
        else if (!cached_found && sscanf(line, "Cached: %d kB", &info->cached) == 1) 
        {
            cached_found = true;
        }
        if (total_found && free_found && buffers_found && cached_found) 
        {
            break;
        }
    }

    fclose(file);

    if (!(total_found && free_found && buffers_found && cached_found)) 
    {
        fprintf(stderr, "Couldn't find all required memory information\n");
        return false;
    }

    info->used_memory = info->total_memory - info->mem_free - info->buffers - info->cached;

    return true;
}

bool get_gpu_info(GPUInfo_t *info)
{
    memset(info, 0, sizeof(GPUInfo_t));

    FILE *fp = popen("nvidia-smi --query-gpu=name,utilization.gpu --format=csv,noheader,nounits", "r");
    if (fp == NULL || info == NULL) 
    {
        perror("Can't open nvidia-smi");
        return false; 
    }

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp) != NULL) 
    {
        char utilization[16];
        
        sscanf(buffer, "%[^,], %15s", info->name, utilization); 

        if (sscanf(utilization, "%f", &info->usage) != 1)
        {
            perror("Can't read info about the GPU\n");
            return false;
        }
    } 
    else 
    {
        perror("Can't get info about the GPU\n");
        return false;
    }

    pclose(fp);
    return true;
}

bool get_os_info(OS_Info_t *info)
{
    memset(info, 0, sizeof(OS_Info_t));

    FILE *fp = fopen("/etc/os-release", "r");
    if (fp == NULL || info == NULL) 
    {
        perror("Can't open /etc/os-release");
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) 
    {
        if (strncmp(line, "NAME=", 5) == 0) 
        {
            sscanf(line, "NAME=\"%[^\"]\"", info->os_name);
        } 
        else if (strncmp(line, "VERSION=", 8) == 0) 
        {
            sscanf(line, "VERSION=\"%[^\"]\"", info->os_version);
        }
    }

    fclose(fp);

    fp = popen("uname -r", "r");
    if (fp == NULL) 
    {
        perror("Failed to execute uname -r");
        return false;
    }

    if (fgets(info->kernel_version, sizeof(info->kernel_version), fp) == NULL) 
    {
        fprintf(stderr, "Can't get info about the kernel version\n");
        pclose(fp);
        return false;
    }

    info->kernel_version[strcspn(info->kernel_version, "\n")] = 0;

    pclose(fp);

    char *default_shell = getenv("SHELL");
    if (default_shell != NULL) 
    {
        strncpy(info->default_shell, default_shell, sizeof(info->default_shell) - 1);
    }
    return true;
}

bool get_monitor_info(MonitorInfo_t *info)
{
    memset(info, 0, sizeof(MonitorInfo_t));

    FILE *fp = popen("xrandr --current | grep ' connected'", "r");
    if (fp == NULL) 
    {
        perror("Failed to open xrandr");
        return false;
    }

    char line[256];

    if (fgets(line, sizeof(line), fp) != NULL) 
    {
        sscanf(line, "%127s connected %*s %dx%d", info->monitor_name, &(info->width), &(info->height));

    }
    else 
    {
        perror("Failed to get monitor info!\n");
        pclose(fp);
        return false;
    }

    pclose(fp);
    
    return true;
}

bool get_disk_space_info(DiskSpaceInfo_t *info, const char *path)
{
    memset(info, 0, sizeof(DiskSpaceInfo_t));

    struct statvfs stat;

    if (statvfs(path, &stat) != 0) 
    {
        perror("Failed to get file system information");
        return false;  
    }
   
    unsigned long long total_space = stat.f_blocks * stat.f_frsize;
    unsigned long long free_space = stat.f_bfree * stat.f_frsize;
    unsigned long long used_space = total_space - free_space;

    info->total_gb = (double)total_space / (1024 * 1024 * 1024);
    info->free_gb = (double)free_space / (1024 * 1024 * 1024);
    info->used_gb = (double)used_space / (1024 * 1024 * 1024);
    info->used_percent = (double)used_space / total_space * 100;

    strncpy(info->path, path, sizeof(info->path) - 1);

    return true;  
} 

bool get_internet_speed_info(InternetSpeedInfo_t *info)
{
    memset(info, 0, sizeof(InternetSpeedInfo_t));

    FILE *fp = popen("speedtest-cli --simple", "r");
    if (fp == NULL) 
    {
        perror("Failed to execute speedtest-cli");
        return false; 
    }

    char line[256];

    while (fgets(line, sizeof(line), fp) != NULL) 
    {
        if (strncmp(line, "Ping:", 5) == 0) 
        {
            sscanf(line, "Ping: %[^\n]", info->ping);
        } 
        else if (strncmp(line, "Download:", 9) == 0) 
        {
            sscanf(line, "Download: %[^\n]", info->download);
        } 
        else if (strncmp(line, "Upload:", 7) == 0) 
        {
            sscanf(line, "Upload: %[^\n]", info->upload);
        }
    }

    pclose(fp);

    return true;  
}

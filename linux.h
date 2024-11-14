#ifndef LINUX_H
#define LINUX_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static struct
{
    bool show_time;
    bool show_uptime;
    bool show_CPU;
    bool show_RAM;
    bool show_GPU;
    bool show_OS;
    bool show_monitor;
    bool show_disk;
    bool show_speed_internet;
} options_t;

typedef struct 
{
    char name[256];       
    int num_cores;        
    float frequencies[32]; 
    float usages[32];      
} CPUInfo_t;

typedef struct 
{
    uint32_t total_memory;    
    uint32_t used_memory;     
    uint32_t mem_free;
    uint32_t buffers;
    uint32_t cached;
} MemoryInfo_t;

typedef struct 
{
    char name[256];     
    float usage;         
} GPUInfo_t;

typedef struct 
{
    char os_name[256];
    char os_version[256];
    char kernel_version[256];
    char default_shell[256];
} OS_Info_t;

typedef struct 
{
    char monitor_name[128];
    int width;
    int height;
} MonitorInfo_t;

typedef struct 
{
    double total_gb;
    double used_gb;
    double free_gb;
    double used_percent;
    char path[256]; 
} DiskSpaceInfo_t;

typedef struct 
{
    char ping[128];
    char download[128];
    char upload[128];
} InternetSpeedInfo_t;

bool get_system_uptime(double *uptime_seconds);

bool get_cpu_info(CPUInfo_t * info);

bool get_memory_info(MemoryInfo_t * info);

bool get_gpu_info(GPUInfo_t * info);

bool get_os_info(OS_Info_t * info);

bool get_monitor_info(MonitorInfo_t * info);

bool get_disk_space_info(DiskSpaceInfo_t *info, const char *path);

bool get_internet_speed_info(InternetSpeedInfo_t *info);

#endif 
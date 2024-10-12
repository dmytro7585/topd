#ifndef LINUX_H
#define LINUX_H

#include <stddef.h>

typedef struct 
{
    char name[256];       
    int num_cores;        
    float frequencies[32]; 
    float usages[32];      
} CPUInfo;

typedef struct {
    long total_memory;    
    long used_memory;     
} MemoryInfo;

typedef struct 
{
    char name[256];     
    float usage;         
} GPUInfo;

typedef struct 
{
    char os_name[256];
    char os_version[256];
    char kernel_version[256];
    char default_shell[256];
} OS_Info;

typedef struct 
{
    char monitor_name[128];
    int width;
    int height;
} MonitorInfo;

typedef struct 
{
    double total_gb;
    double used_gb;
    double free_gb;
    double used_percent;
    char path[256]; 
} DiskSpaceInfo;

typedef struct 
{
    char ping[128];
    char download[128];
    char upload[128];
} InternetSpeedInfo;

CPUInfo get_cpu_info();
MemoryInfo get_memory_info();
GPUInfo get_gpu_info();



double get_system_uptime();
OS_Info get_os_info();
MonitorInfo get_monitor_info();
DiskSpaceInfo get_disk_space_info(const char *path); 
InternetSpeedInfo get_internet_speed_info();  

#endif 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "linux.h"

#define LOG_FILE "/tmp/topd.log"
#define PID_FILE "/var/run/topd.pid"

void log_to_file(const char *message) 
{
    FILE *fp = fopen(LOG_FILE, "w");  
    if (fp == NULL) 
    {
        perror("Не вдалося відкрити файл для запису");
        return;
    }
    fprintf(fp, "%s", message);
    fclose(fp);
}

void create_daemon()
{
    pid_t pid, sid;

    FILE *pid_file = fopen(PID_FILE, "r");
    if (pid_file != NULL) 
    {

        pid_t existing_pid;
        if (fscanf(pid_file, "%d", &existing_pid) != 1) 
        {
            fprintf(stderr, "Не вдалося прочитати PID з файлу\n");
            fclose(pid_file);
            exit(EXIT_FAILURE);
        }
        fclose(pid_file);

        if (kill(existing_pid, 0) == 0) 
        {
            fprintf(stderr, "Демон вже працює (PID: %d)\n", existing_pid);
            exit(EXIT_FAILURE);
        } else 
        {

            unlink(PID_FILE);
        }
    }
  
    pid = fork();
    if (pid < 0) 
    {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) 
    {
        exit(EXIT_SUCCESS);
    }

    sid = setsid();
    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) 
    {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    open("/dev/null", O_RDONLY); 
    open("/dev/null", O_RDWR);   
    open("/dev/null", O_RDWR);   

    pid_file = fopen(PID_FILE, "w");
    if (pid_file == NULL) 
    {
        exit(EXIT_FAILURE);
    }
    fprintf(pid_file, "%d\n", getpid());
    fclose(pid_file);
}

void print_current_datetime(char *buffer, size_t size) 
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    tzset();

    char time_buffer[64];
    strftime(time_buffer, sizeof(time_buffer), "[%H:%M:%S][%a][%b][%Y-%m-%d]", tm_info);

    extern char *tzname[2];
    char *timezone = tzname[tm_info->tm_isdst > 0];

    snprintf(buffer + strlen(buffer), size - strlen(buffer), "%s[%s]\n\n", time_buffer, timezone);
}

void print_cpu_info(char *buffer, size_t size) 
{
    CPUInfo cpu_info = get_cpu_info();  

    snprintf(buffer + strlen(buffer), size - strlen(buffer), "CPU Info:\n%s\nCores(%d):\n", 
             cpu_info.name, cpu_info.num_cores);

    for (int i = 0; i < cpu_info.num_cores; ++i) 
    {
        snprintf(buffer + strlen(buffer), size - strlen(buffer), 
                 "   Core %d: Frequency = %.2f GHz, Usage = %.2f%%\n", 
                 i, cpu_info.frequencies[i], cpu_info.usages[i]);
    }
    snprintf(buffer + strlen(buffer), size - strlen(buffer), "\n");
}

void print_system_uptime(char *buffer, size_t size) 
{
    double uptime_seconds = get_system_uptime();
    
    if (uptime_seconds < 0) {
        return;
    }

    int hours = (int)(uptime_seconds / 3600);
    int minutes = (int)((uptime_seconds - (hours * 3600)) / 60);
    int seconds = (int)(uptime_seconds - (hours * 3600) - (minutes * 60));


    snprintf(buffer + strlen(buffer), size - strlen(buffer), "System Uptime: %02d:%02d:%02d\n", hours, minutes, seconds);
}

void print_os_info(char *buffer, size_t size)
 {
    OS_Info os_info = get_os_info();

    
    snprintf(buffer + strlen(buffer), size - strlen(buffer), "OS: %s %s\nKernel version: %s\nShell: %s\n\n",
             os_info.os_name, os_info.os_version, os_info.kernel_version, os_info.default_shell);
}

void print_monitor_info(char *buffer, size_t size) 
{
    MonitorInfo monitor_info = get_monitor_info(); 

    
    if (monitor_info.width > 0 && monitor_info.height > 0) {
        snprintf(buffer + strlen(buffer), size - strlen(buffer), "Display: %s\nResolution: %dx%d\n\n",
                 monitor_info.monitor_name, monitor_info.width, monitor_info.height);
    } else {
        snprintf(buffer + strlen(buffer), size - strlen(buffer), "Display: %s (resolution is not defined)\n\n", monitor_info.monitor_name);
    }
}

void print_disk_space_info(char *buffer, size_t size, const char *path) 
{
    DiskSpaceInfo disk_info = get_disk_space_info(path);  

   
    snprintf(buffer + strlen(buffer), size - strlen(buffer), "Disk (%s):\nTotal: %.2f GB\nUsed: %.2f GB (%.2f%%)\nFree: %.2f GB\n\n",
             disk_info.path, disk_info.total_gb, disk_info.used_gb, disk_info.used_percent, disk_info.free_gb);
}

void print_internet_speed(char *buffer, size_t size)
{
    InternetSpeedInfo speed_info = get_internet_speed_info();  

    
    snprintf(buffer + strlen(buffer), size - strlen(buffer), "Internet Speed:\nPing: %s\nDownload: %s\nUpload: %s\n\n",
             speed_info.ping, speed_info.download, speed_info.upload);
}

void print_memory_info(char *buffer, size_t size) 
{
    MemoryInfo memory_info = get_memory_info();  

    
    double total_memory_gb = memory_info.total_memory / (1024.0 * 1024.0);
    double used_memory_gb = memory_info.used_memory / (1024.0 * 1024.0);
    double used_percentage = (memory_info.used_memory / (double)memory_info.total_memory) * 100.0;

    
    snprintf(buffer + strlen(buffer), size - strlen(buffer), "RAM Info:\n");
    snprintf(buffer + strlen(buffer), size - strlen(buffer), 
             "   Total: %.2f GB\n   Used: %.2f GB (%.0f%%)\n\n", 
             total_memory_gb, used_memory_gb, used_percentage);
}

void print_gpu_info_to_buffer(char *buffer, size_t size) 
{
    GPUInfo gpu_info = get_gpu_info(); 

   
    snprintf(buffer + strlen(buffer), size - strlen(buffer), "GPU Info:\n");
    snprintf(buffer + strlen(buffer), size - strlen(buffer), 
             "   Name: %s\n   Usage: %.0f%%\n\n", 
             gpu_info.name, gpu_info.usage);
}

static void print_help(void) 
{
    printf
    (
        "Usage: ./topd [-htcrgomdi]\n"
        "Options:\n"
        "  -h          Show this help message\n"
        "  -t          Show time\n"
        "  -c          Show info about the CPU\n"
        "  -r          Show info about the RAM\n"
        "  -g          Show info about the GPU\n"
        "  -o          Show info about the OS\n"
        "  -m          Show info about the monitor\n"
        "  -d          Show info about the disk\n"
        "  -i          Show info about the speed of the Internet\n"
    );
}

int main(int argc, char *argv[]) 
{
    create_daemon();

    char log_message[2048] = ""; 

while (1) 
   {
        log_message[0] = '\0';

    if (argc < 2) 
    {
        fprintf(stderr, "Default mode. Use ''./topd -h'' for help.\n");
        print_cpu_info(log_message, sizeof(log_message));
        print_gpu_info_to_buffer(log_message, sizeof(log_message));
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0) 
        {
            print_current_datetime(log_message, sizeof(log_message));
        } 

        else if (strcmp(argv[i], "-c") == 0)
        {
            print_cpu_info(log_message, sizeof(log_message));
        } 

        else if (strcmp(argv[i], "-r") == 0)
        {
            print_memory_info(log_message, sizeof(log_message));
        } 

        else if (strcmp(argv[i], "-g") == 0)
        {
            print_gpu_info_to_buffer(log_message, sizeof(log_message));
        } 

        else if (strcmp(argv[i], "-o") == 0)
        {
            print_os_info(log_message, sizeof(log_message));
        } 

        else if (strcmp(argv[i], "-m") == 0)
        {
            print_monitor_info(log_message, sizeof(log_message));
        } 
        else if (strcmp(argv[i], "-d") == 0) 
        {
            print_disk_space_info(log_message, sizeof(log_message), "/");
        } 
        else if (strcmp(argv[i], "-i") == 0)
        {
            print_internet_speed(log_message, sizeof(log_message));
        } 
        else if (strcmp(argv[i], "-h") == 0)
        {
            print_help();
        } 
        else 
        {
            fprintf(stderr, "Unknown command: %s\n", argv[i]);
            print_help();
            return 1;
        }
    }
    
        log_to_file(log_message);

        sleep(3);
    }

    return 0;
}


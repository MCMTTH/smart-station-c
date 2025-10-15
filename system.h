#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

// ============================================================================
// CONSTANTS AND DEFINITIONS
// ============================================================================

// System Modes
#define USER_MODE       1
#define ADMIN_MODE      2
#define AUTO_MODE       3

// Menu Options
#define VIEW_RECENT_DATA    1
#define STATISTICS          2
#define ASCII_CHART         3
#define EXPORT_REPORT       4
#define DELETE_OLD_DATA     1
#define CONFIGURE           2
#define ARDUINO_CONTROL     3

// Sensor Data Types
#define TEMPERATURE_SENSOR  1
#define HUMIDITY_SENSOR     2
#define GAS_SENSOR          3
#define MAX_SENSORS         10

// Data Limits
#define MAX_DATA_RECORDS    1000
#define MAX_RECENT_RECORDS  100
#define MAX_FILENAME_LEN    256
#define MAX_STRING_LEN      128

// Arduino Communication
#define ARDUINO_DEVICE      "/dev/ttyUSB0"
#define BAUD_RATE           B9600
#define READ_TIMEOUT        5000  // 5 seconds

// Simulation Limits
#define MAX_PIPES           20
#define PIPE_BUFFER_SIZE    1024

// File Paths
#define DATA_FILE           "sensor_data.txt"
#define CONFIG_FILE         "config.txt"
#define REPORT_DIR          "reports/"

// ============================================================================
// DATA STRUCTURES
// ============================================================================

// Sensor Data Structure
typedef struct sensor_data {
    time_t timestamp;           // Thời gian đo
    float temperature;          // Nhiệt độ (°C)
    float humidity;             // Độ ẩm (%)
    float gas_level;            // Nồng độ khí (ppm)
    int sensor_id;              // ID của sensor
    int quality;                // Chất lượng dữ liệu (0-100)
} sensor_data_t;

// Statistics Structure
typedef struct statistics {
    float temp_max, temp_min, temp_avg;
    float humidity_max, humidity_min, humidity_avg;
    float gas_max, gas_min, gas_avg;
    int total_records;
    time_t first_record, last_record;
} statistics_t;

// Configuration Structure
typedef struct config {
    int auto_collect_interval;  // Thời gian tự động thu thập (giây)
    int data_retention_days;    // Số ngày lưu trữ dữ liệu
    char arduino_device[64];    // Đường dẫn thiết bị Arduino
    int baud_rate;              // Tốc độ baud
    int max_records;            // Số bản ghi tối đa
    int auto_mode_enabled;      // Bật/tắt chế độ tự động
} config_t;

// Menu Item Structure
typedef struct menu_item {
    int id;
    char title[64];
    char description[128];
    int (*function)(void);
} menu_item_t;

// ASCII Chart Data
typedef struct chart_data {
    int width;                  // Chiều rộng chart
    int height;                 // Chiều cao chart
    char data_type;             // 'T'=temperature, 'H'=humidity, 'G'=gas
    float min_value, max_value; // Giá trị min/max
    sensor_data_t *data_points; // Dữ liệu để vẽ
    int num_points;             // Số điểm dữ liệu
} chart_data_t;

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// System State
extern int current_mode;
extern int system_running;
extern config_t system_config;

// Data Storage
extern sensor_data_t sensor_data[MAX_DATA_RECORDS];
extern int data_count;
extern int recent_data_count;
extern sensor_data_t recent_data[MAX_RECENT_RECORDS];

// Statistics
extern statistics_t global_stats;
extern int stats_updated;

// Arduino Communication
extern int arduino_fd;
extern int arduino_connected;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

// System Initialization
void init_system(void);
void shutdown_system(void);
int load_config(void);
int save_config(void);

// Main Program Flow
void run_auto_mode(void);
void run_user_mode(void);
void run_admin_mode(void);
void show_main_menu(void);

// Data Collection (Auto Mode)
int connect_arduino(void);
int disconnect_arduino(void);
int collect_sensor_data(void);
int read_arduino_data(sensor_data_t *data);
int validate_sensor_data(sensor_data_t *data);
void save_data_to_file(sensor_data_t *data);
void update_recent_data(sensor_data_t *data);

// Data Management
int load_data_from_file(void);
int save_data_to_file_all(void);
int delete_old_data(int days);
int clear_all_data(void);
int backup_data(const char *filename);
int restore_data(const char *filename);

// Statistics
int calculate_statistics(void);
void print_statistics(void);
void print_recent_data(int count);
sensor_data_t* get_latest_data(void);

// User Interface
void clear_screen(void);
void show_header(void);
void show_user_menu(void);
void show_admin_menu(void);
int get_user_choice(void);
void wait_for_enter(void);
void show_message(const char *message);
void show_error(const char *error);

// ASCII Chart
int create_ascii_chart(chart_data_t *chart);
int display_temperature_chart(int hours);
int display_humidity_chart(int hours);
int display_gas_chart(int hours);
void draw_chart_border(int width, int height);
void draw_data_line(chart_data_t *chart);

// Report Generation
int generate_report(const char *filename);
int export_to_csv(const char *filename);
int export_to_txt(const char *filename);
void print_report_header(FILE *fp);
void print_report_data(FILE *fp);
void print_report_statistics(FILE *fp);

// Configuration Management
int configure_system(void);
int set_auto_collect_interval(void);
int set_data_retention(void);
int set_arduino_device(void);
int set_max_records(void);
void show_configuration(void);

// Arduino Control
int start_arduino(void);
int stop_arduino(void);
int restart_arduino(void);
int test_arduino_connection(void);
int send_arduino_command(const char *command);

// Utility Functions
time_t get_current_time(void);
char* format_timestamp(time_t timestamp);
float calculate_average(float *values, int count);
float find_maximum(float *values, int count);
float find_minimum(float *values, int count);
int is_valid_temperature(float temp);
int is_valid_humidity(float humidity);
int is_valid_gas_level(float gas);

// File Operations
int create_directory(const char *path);
int file_exists(const char *filename);
long get_file_size(const char *filename);
int copy_file(const char *src, const char *dest);

// Menu Functions (USER MODE)
int user_view_recent_data(void);
int user_show_statistics(void);
int user_display_ascii_chart(void);
int user_export_report(void);

// Menu Functions (ADMIN MODE)
int admin_delete_old_data(void);
int admin_configure_system(void);
int admin_arduino_control(void);

// Error Handling
void handle_system_error(int error_code);
void log_error(const char *function, const char *error);
void log_info(const char *message);

// Process and Pipe Simulation
void init_simulation_system(void);
void print_simulation_status(void);
int simulated_fork(void);
int simulated_exit(int exit_code);
int simulated_wait(int *status);
int simulated_pipe(int pipefd[2]);
ssize_t simulated_read(int fd, void *buf, size_t count);
ssize_t simulated_write(int fd, const void *buf, size_t count);
int simulated_close(int fd);

// Simulation Test Functions
void run_simulation_test(void);
void test_fork_simulation(void);
void test_pipe_simulation(void);
void test_fork_and_pipe_combined(void);
void run_comprehensive_simulation_test(void);

// ============================================================================
// MACROS
// ============================================================================

// Debug print macro
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

// Error checking macro
#define CHECK_ERROR(result) \
    do { \
        if ((result) < 0) { \
            handle_system_error(result); \
            return result; \
        } \
    } while(0)

// Clear screen macro
#define CLEAR_SCREEN() system("clear")

// Wait for user input
#define WAIT_FOR_ENTER() \
    do { \
        printf("\nPress Enter to continue..."); \
        getchar(); \
    } while(0)

// ============================================================================
// INLINE FUNCTIONS
// ============================================================================

// Check if system is in auto mode
static inline int is_auto_mode(void) {
    return current_mode == AUTO_MODE && system_config.auto_mode_enabled;
}

// Check if data is recent (within last hour)
static inline int is_recent_data(time_t timestamp) {
    return (get_current_time() - timestamp) < 3600;
}

// Check if Arduino is connected
static inline int is_arduino_connected(void) {
    return arduino_connected && arduino_fd >= 0;
}

// Get data count
static inline int get_data_count(void) {
    return data_count;
}

// Check if data storage is full
static inline int is_data_storage_full(void) {
    return data_count >= MAX_DATA_RECORDS;
}

#endif // SYSTEM_H
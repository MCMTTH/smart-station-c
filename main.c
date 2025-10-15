#include "system.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

int current_mode = USER_MODE;
int system_running = 1;

// Mock data for demonstration
sensor_data_t mock_data[] = {
    {1640995200, 25.5, 60.2, 120.0, 1, 95},
    {1640995260, 26.1, 58.9, 115.5, 1, 92},
    {1640995320, 24.8, 62.1, 118.3, 1, 88},
    {1640995380, 27.2, 55.4, 122.1, 1, 96},
    {1640995440, 25.9, 59.8, 119.7, 1, 94}
};
int mock_data_count = 5;

// ============================================================================
// MAIN PROGRAM
// ============================================================================

int main() {
    printf("========================================\n");
    printf("    OSG202 - Sensor Data System\n");
    printf("========================================\n\n");
    
    // Initialize system
    init_system();
    
    // Main program loop
    while (system_running) {
        show_main_menu();
        
        int choice = get_user_choice();
        
        switch (choice) {
            case 1:
                run_user_mode();
                break;
            case 2:
                run_admin_mode();
                break;
            case 3:
                run_auto_mode();
                break;
            case 4:
                run_simulation_test();
                break;
            case 0:
                printf("Goodbye!\n");
                system_running = 0;
                break;
            default:
                printf("Invalid choice! Please try again.\n");
                wait_for_enter();
                break;
        }
    }
    
    shutdown_system();
    return 0;
}

// ============================================================================
// SYSTEM INITIALIZATION
// ============================================================================

void init_system(void) {
    printf("Initializing system...\n");
    
    // Initialize simulation system first
    init_simulation_system();
    
    // Initialize data structures
    data_count = 0;
    recent_data_count = 0;
    stats_updated = 0;
    
    // Load mock data for demonstration
    for (int i = 0; i < mock_data_count; i++) {
        sensor_data[data_count] = mock_data[i];
        data_count++;
    }
    
    // Calculate initial statistics
    calculate_statistics();
    
    printf("System initialized successfully!\n");
    printf("Loaded %d data records\n", data_count);
    printf("Simulation system ready (fork() and pipe() available)\n\n");
}

void shutdown_system(void) {
    printf("\nShutting down system...\n");
    printf("System shutdown completed.\n");
}

// ============================================================================
// MAIN MENU
// ============================================================================

void show_main_menu(void) {
    clear_screen();
    show_header();
    
    printf("=== MAIN MENU ===\n\n");
    printf("1. USER MODE\n");
    printf("   - View recently data\n");
    printf("   - Statistics max/min/average\n");
    printf("   - Display ASCII chart\n");
    printf("   - Export report\n\n");
    
    printf("2. ADMIN MODE\n");
    printf("   - Delete old data\n");
    printf("   - Configure system\n");
    printf("   - Arduino control\n\n");
    
    printf("3. AUTO MODE\n");
    printf("   - Collect data automatically\n\n");
    
    printf("4. SIMULATION TEST\n");
    printf("   - Test fork() and pipe() simulation\n\n");
    
    printf("0. Exit\n\n");
    printf("Enter your choice: ");
}

// ============================================================================
// USER MODE
// ============================================================================

void run_user_mode(void) {
    current_mode = USER_MODE;
    
    while (1) {
        clear_screen();
        show_header();
        show_user_menu();
        
        int choice = get_user_choice();
        
        switch (choice) {
            case 1:
                user_view_recent_data();
                break;
            case 2:
                user_show_statistics();
                break;
            case 3:
                user_display_ascii_chart();
                break;
            case 4:
                user_export_report();
                break;
            case 0:
                return; // Back to main menu
            default:
                printf("Invalid choice! Please try again.\n");
                wait_for_enter();
                break;
        }
    }
}

void show_user_menu(void) {
    printf("=== USER MODE ===\n\n");
    printf("1. View recently data\n");
    printf("2. Statistics max/min/average\n");
    printf("3. Display ASCII chart\n");
    printf("4. Export report\n");
    printf("0. Back to main menu\n\n");
    printf("Enter your choice: ");
}

// USER MODE FUNCTIONS
int user_view_recent_data(void) {
    printf("\n=== RECENT DATA ===\n\n");
    
    printf("%-20s %-8s %-8s %-8s %-6s\n", 
           "Timestamp", "Temp(°C)", "Humidity(%)", "Gas(ppm)", "Quality");
    printf("------------------------------------------------------------\n");
    
    int count = (data_count > 10) ? 10 : data_count;
    for (int i = data_count - count; i < data_count; i++) {
        char time_str[32];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", 
                localtime(&sensor_data[i].timestamp));
        
        printf("%-20s %-8.1f %-8.1f %-8.1f %-6d\n",
               time_str,
               sensor_data[i].temperature,
               sensor_data[i].humidity,
               sensor_data[i].gas_level,
               sensor_data[i].quality);
    }
    
    wait_for_enter();
    return 0;
}

int user_show_statistics(void) {
    printf("\n=== STATISTICS ===\n\n");
    
    if (!stats_updated) {
        calculate_statistics();
    }
    
    printf("Temperature:\n");
    printf("  Max: %.1f°C\n", global_stats.temp_max);
    printf("  Min: %.1f°C\n", global_stats.temp_min);
    printf("  Average: %.1f°C\n\n", global_stats.temp_avg);
    
    printf("Humidity:\n");
    printf("  Max: %.1f%%\n", global_stats.humidity_max);
    printf("  Min: %.1f%%\n", global_stats.humidity_min);
    printf("  Average: %.1f%%\n\n", global_stats.humidity_avg);
    
    printf("Gas Level:\n");
    printf("  Max: %.1f ppm\n", global_stats.gas_max);
    printf("  Min: %.1f ppm\n", global_stats.gas_min);
    printf("  Average: %.1f ppm\n\n", global_stats.gas_avg);
    
    printf("Total Records: %d\n", global_stats.total_records);
    
    wait_for_enter();
    return 0;
}

int user_display_ascii_chart(void) {
    printf("\n=== ASCII CHART ===\n\n");
    
    printf("Select chart type:\n");
    printf("1. Temperature\n");
    printf("2. Humidity\n");
    printf("3. Gas Level\n");
    printf("0. Back\n\n");
    printf("Enter your choice: ");
    
    int choice = get_user_choice();
    
    switch (choice) {
        case 1:
            display_temperature_chart(24);
            break;
        case 2:
            display_humidity_chart(24);
            break;
        case 3:
            display_gas_chart(24);
            break;
        default:
            return 0;
    }
    
    wait_for_enter();
    return 0;
}

int user_export_report(void) {
    printf("\n=== EXPORT REPORT ===\n\n");
    
    char filename[256];
    printf("Enter filename (without extension): ");
    scanf("%s", filename);
    
    printf("\nSelect format:\n");
    printf("1. Text file (.txt)\n");
    printf("2. CSV file (.csv)\n");
    printf("0. Back\n\n");
    printf("Enter your choice: ");
    
    int format = get_user_choice();
    
    switch (format) {
        case 1:
            strcat(filename, ".txt");
            export_to_txt(filename);
            break;
        case 2:
            strcat(filename, ".csv");
            export_to_csv(filename);
            break;
        default:
            return 0;
    }
    
    wait_for_enter();
    return 0;
}

// ============================================================================
// ADMIN MODE
// ============================================================================

void run_admin_mode(void) {
    current_mode = ADMIN_MODE;
    
    while (1) {
        clear_screen();
        show_header();
        show_admin_menu();
        
        int choice = get_user_choice();
        
        switch (choice) {
            case 1:
                admin_delete_old_data();
                break;
            case 2:
                admin_configure_system();
                break;
            case 3:
                admin_arduino_control();
                break;
            case 0:
                return; // Back to main menu
            default:
                printf("Invalid choice! Please try again.\n");
                wait_for_enter();
                break;
        }
    }
}

void show_admin_menu(void) {
    printf("=== ADMIN MODE ===\n\n");
    printf("1. Delete old data\n");
    printf("2. Configure system\n");
    printf("3. Arduino control\n");
    printf("0. Back to main menu\n\n");
    printf("Enter your choice: ");
}

// ADMIN MODE FUNCTIONS
int admin_delete_old_data(void) {
    printf("\n=== DELETE OLD DATA ===\n\n");
    
    printf("Current data records: %d\n\n", data_count);
    
    printf("Select action:\n");
    printf("1. Delete records older than 7 days\n");
    printf("2. Delete records older than 30 days\n");
    printf("3. Delete all data\n");
    printf("0. Back\n\n");
    printf("Enter your choice: ");
    
    int choice = get_user_choice();
    
    switch (choice) {
        case 1:
            printf("Deleted %d records older than 7 days\n", delete_old_data(7));
            break;
        case 2:
            printf("Deleted %d records older than 30 days\n", delete_old_data(30));
            break;
        case 3:
            printf("Deleted all %d records\n", clear_all_data());
            break;
        default:
            return 0;
    }
    
    wait_for_enter();
    return 0;
}

int admin_configure_system(void) {
    printf("\n=== CONFIGURE SYSTEM ===\n\n");
    
    printf("Configuration options:\n");
    printf("1. Set auto collect interval\n");
    printf("2. Set data retention period\n");
    printf("3. Set Arduino device path\n");
    printf("4. View current configuration\n");
    printf("0. Back\n\n");
    printf("Enter your choice: ");
    
    int choice = get_user_choice();
    
    switch (choice) {
        case 1:
            set_auto_collect_interval();
            break;
        case 2:
            set_data_retention();
            break;
        case 3:
            set_arduino_device();
            break;
        case 4:
            show_configuration();
            break;
        default:
            return 0;
    }
    
    wait_for_enter();
    return 0;
}

int admin_arduino_control(void) {
    printf("\n=== ARDUINO CONTROL ===\n\n");
    
    printf("Arduino status: %s\n\n", 
           arduino_connected ? "Connected" : "Disconnected");
    
    printf("Control options:\n");
    printf("1. Connect Arduino\n");
    printf("2. Disconnect Arduino\n");
    printf("3. Test connection\n");
    printf("4. Restart Arduino\n");
    printf("0. Back\n\n");
    printf("Enter your choice: ");
    
    int choice = get_user_choice();
    
    switch (choice) {
        case 1:
            if (connect_arduino() == 0) {
                printf("Arduino connected successfully!\n");
            } else {
                printf("Failed to connect Arduino\n");
            }
            break;
        case 2:
            disconnect_arduino();
            printf("Arduino disconnected\n");
            break;
        case 3:
            if (test_arduino_connection() == 0) {
                printf("Arduino connection test passed\n");
            } else {
                printf("Arduino connection test failed\n");
            }
            break;
        case 4:
            restart_arduino();
            printf("Arduino restarted\n");
            break;
        default:
            return 0;
    }
    
    wait_for_enter();
    return 0;
}

// ============================================================================
// AUTO MODE
// ============================================================================

void run_auto_mode(void) {
    current_mode = AUTO_MODE;
    
    printf("\n=== AUTO MODE ===\n\n");
    printf("Collecting data automatically...\n");
    printf("Press Ctrl+C to stop\n\n");
    
    // Simulate auto data collection
    for (int i = 0; i < 10; i++) {
        printf("Collecting data... %d/10\n", i + 1);
        
        // Simulate data collection
        sensor_data_t new_data;
        new_data.timestamp = time(NULL);
        new_data.temperature = 20 + (rand() % 20); // 20-40°C
        new_data.humidity = 40 + (rand() % 40);    // 40-80%
        new_data.gas_level = 100 + (rand() % 100); // 100-200 ppm
        new_data.sensor_id = 1;
        new_data.quality = 90 + (rand() % 10);     // 90-100%
        
        // Add to data array
        if (data_count < MAX_DATA_RECORDS) {
            sensor_data[data_count] = new_data;
            data_count++;
        }
        
        sleep(2); // Wait 2 seconds
    }
    
    printf("\nAuto data collection completed!\n");
    printf("Collected %d new data records\n", data_count);
    
    wait_for_enter();
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void clear_screen(void) {
    CLEAR_SCREEN();
}

void show_header(void) {
    printf("========================================\n");
    printf("    OSG202 - Sensor Data System\n");
    printf("========================================\n\n");
}

int get_user_choice(void) {
    int choice;
    scanf("%d", &choice);
    return choice;
}

void wait_for_enter(void) {
    WAIT_FOR_ENTER();
}

void show_message(const char *message) {
    printf("\n%s\n", message);
}

void show_error(const char *error) {
    printf("\nERROR: %s\n", error);
}

// ============================================================================
// SIMULATION TEST MODE
// ============================================================================

void run_simulation_test(void) {
    current_mode = 4; // SIMULATION_TEST_MODE
    
    while (1) {
        clear_screen();
        show_header();
        
        printf("=== SIMULATION TEST MODE ===\n\n");
        printf("Test fork() and pipe() simulation:\n\n");
        printf("1. Test fork() - Process creation\n");
        printf("2. Test pipe() - Inter-process communication\n");
        printf("3. Test fork() + pipe() combined\n");
        printf("4. Show simulation status\n");
        printf("5. Run comprehensive test\n");
        printf("0. Back to main menu\n\n");
        printf("Enter your choice: ");
        
        int choice = get_user_choice();
        
        switch (choice) {
            case 1:
                test_fork_simulation();
                break;
            case 2:
                test_pipe_simulation();
                break;
            case 3:
                test_fork_and_pipe_combined();
                break;
            case 4:
                print_simulation_status();
                wait_for_enter();
                break;
            case 5:
                run_comprehensive_simulation_test();
                break;
            case 0:
                return; // Back to main menu
            default:
                printf("Invalid choice! Please try again.\n");
                wait_for_enter();
                break;
        }
    }
}

void test_fork_simulation(void) {
    printf("\n=== TEST FORK() SIMULATION ===\n\n");
    
    printf("Testing process creation with fork()...\n\n");
    
    // Test 1: Simple fork
    printf("Test 1: Simple fork()\n");
    int child_pid = simulated_fork();
    if (child_pid > 0) {
        printf("✓ Parent process created child PID %d\n", child_pid);
        
        // Wait for child
        int status;
        simulated_wait(&status);
        printf("✓ Parent waited for child, exit code: %d\n", status);
    } else if (child_pid == 0) {
        printf("✓ Child process executed successfully\n");
    } else {
        printf("✗ Fork failed\n");
    }
    
    wait_for_enter();
}

void test_pipe_simulation(void) {
    printf("\n=== TEST PIPE() SIMULATION ===\n\n");
    
    printf("Testing inter-process communication with pipe()...\n\n");
    
    // Create pipe
    int pipefd[2];
    if (simulated_pipe(pipefd) == 0) {
        printf("✓ Pipe created successfully (read=%d, write=%d)\n", pipefd[0], pipefd[1]);
        
        // Write data to pipe
        const char *test_message = "Hello from pipe simulation!";
        ssize_t written = simulated_write(pipefd[1], test_message, strlen(test_message));
        if (written > 0) {
            printf("✓ Wrote %zd bytes to pipe\n", written);
            
            // Read data from pipe
            char buffer[256];
            ssize_t read_bytes = simulated_read(pipefd[0], buffer, sizeof(buffer));
            if (read_bytes > 0) {
                buffer[read_bytes] = '\0';
                printf("✓ Read %zd bytes from pipe: %s\n", read_bytes, buffer);
            } else {
                printf("✗ Failed to read from pipe\n");
            }
        } else {
            printf("✗ Failed to write to pipe\n");
        }
        
        // Close pipe
        simulated_close(pipefd[0]);
        simulated_close(pipefd[1]);
        printf("✓ Pipe closed successfully\n");
        
    } else {
        printf("✗ Failed to create pipe\n");
    }
    
    wait_for_enter();
}

void test_fork_and_pipe_combined(void) {
    printf("\n=== TEST FORK() + PIPE() COMBINED ===\n\n");
    
    printf("Testing process communication with fork() and pipe()...\n\n");
    
    // Create pipe first
    int pipefd[2];
    if (simulated_pipe(pipefd) != 0) {
        printf("✗ Failed to create pipe\n");
        wait_for_enter();
        return;
    }
    
    printf("✓ Pipe created (read=%d, write=%d)\n", pipefd[0], pipefd[1]);
    
    // Fork process
    int child_pid = simulated_fork();
    if (child_pid > 0) {
        // Parent process
        printf("✓ Parent process (PID %d) created child (PID %d)\n", 
               current_simulated_process, child_pid);
        
        // Parent writes to pipe
        const char *parent_message = "Message from parent process";
        ssize_t written = simulated_write(pipefd[1], parent_message, strlen(parent_message));
        if (written > 0) {
            printf("✓ Parent wrote %zd bytes to pipe\n", written);
        }
        
        // Wait for child
        int status;
        simulated_wait(&status);
        printf("✓ Parent waited for child, exit code: %d\n", status);
        
    } else if (child_pid == 0) {
        // Child process
        printf("✓ Child process (PID %d) started\n", current_simulated_process);
        
        // Child reads from pipe
        char buffer[256];
        ssize_t read_bytes = simulated_read(pipefd[0], buffer, sizeof(buffer));
        if (read_bytes > 0) {
            buffer[read_bytes] = '\0';
            printf("✓ Child read %zd bytes from pipe: %s\n", read_bytes, buffer);
        }
        
        // Child exits
        simulated_exit(0);
    }
    
    // Close pipe
    simulated_close(pipefd[0]);
    simulated_close(pipefd[1]);
    printf("✓ Pipe communication test completed\n");
    
    wait_for_enter();
}

void run_comprehensive_simulation_test(void) {
    printf("\n=== COMPREHENSIVE SIMULATION TEST ===\n\n");
    
    printf("Running comprehensive test of fork() and pipe() simulation...\n\n");
    
    int tests_passed = 0;
    int total_tests = 0;
    
    // Test 1: Multiple fork()
    printf("Test 1: Multiple fork() calls\n");
    total_tests++;
    
    int child1 = simulated_fork();
    int child2 = simulated_fork();
    
    if (child1 > 0 && child2 > 0) {
        printf("✓ Multiple fork() calls successful\n");
        tests_passed++;
    } else {
        printf("✗ Multiple fork() calls failed\n");
    }
    
    // Test 2: Multiple pipes
    printf("\nTest 2: Multiple pipe() calls\n");
    total_tests++;
    
    int pipe1[2], pipe2[2];
    if (simulated_pipe(pipe1) == 0 && simulated_pipe(pipe2) == 0) {
        printf("✓ Multiple pipe() calls successful\n");
        tests_passed++;
        
        // Clean up
        simulated_close(pipe1[0]);
        simulated_close(pipe1[1]);
        simulated_close(pipe2[0]);
        simulated_close(pipe2[1]);
    } else {
        printf("✗ Multiple pipe() calls failed\n");
    }
    
    // Test 3: Large data transfer
    printf("\nTest 3: Large data transfer through pipe\n");
    total_tests++;
    
    int pipefd[2];
    if (simulated_pipe(pipefd) == 0) {
        // Write large data
        char large_data[512];
        memset(large_data, 'A', sizeof(large_data) - 1);
        large_data[sizeof(large_data) - 1] = '\0';
        
        ssize_t written = simulated_write(pipefd[1], large_data, sizeof(large_data) - 1);
        if (written > 0) {
            // Read data back
            char buffer[512];
            ssize_t read_bytes = simulated_read(pipefd[0], buffer, sizeof(buffer));
            if (read_bytes == written) {
                printf("✓ Large data transfer successful (%zd bytes)\n", written);
                tests_passed++;
            } else {
                printf("✗ Data corruption detected\n");
            }
        } else {
            printf("✗ Failed to write large data\n");
        }
        
        simulated_close(pipefd[0]);
        simulated_close(pipefd[1]);
    } else {
        printf("✗ Failed to create pipe for large data test\n");
    }
    
    // Print results
    printf("\n=== TEST RESULTS ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, total_tests);
    printf("Success rate: %.1f%%\n", (float)tests_passed / total_tests * 100);
    
    if (tests_passed == total_tests) {
        printf("🎉 All simulation tests passed!\n");
    } else {
        printf("⚠️  Some tests failed. Please check implementation.\n");
    }
    
    wait_for_enter();
}
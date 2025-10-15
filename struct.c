#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Định nghĩa struct SensorData
struct SensorData {
    char timestamp[30];  // Giảm kích thước từ 50 xuống 30 để tiết kiệm
    float temperature;
    float humidity;
    int errorCode;
};

// Hàm khởi tạo mảng động với tối ưu
struct SensorData* initDynamicArray(int initialSize) {
    struct SensorData* arr = (struct SensorData*)calloc(initialSize, sizeof(struct SensorData));
    if (arr == NULL) {
        printf("LỖI: Không đủ bộ nhớ!\n");
        return NULL;
    }

    // Khởi tạo giá trị mặc định
    time_t now = time(NULL);
    char* timeStr = ctime(&now);
    timeStr[strlen(timeStr) - 1] = '\0'; // Xóa \n
    for (int i = 0; i < initialSize; i++) {
        strncpy(arr[i].timestamp, timeStr, sizeof(arr[i].timestamp) - 1);
        arr[i].timestamp[sizeof(arr[i].timestamp) - 1] = '\0';
        arr[i].temperature = 0.0;
        arr[i].humidity = 0.0;
        arr[i].errorCode = 0;
    }

    return arr;
}

// Hàm mở rộng mảng động
struct SensorData* expandArray(struct SensorData* arr, int* currentSize, int newSize) {
    struct SensorData* temp = (struct SensorData*)realloc(arr, newSize * sizeof(struct SensorData));
    if (temp == NULL) {
        printf("LỖI: Không mở rộng được mảng!\n");
        free(arr);
        return NULL;
    }
    arr = temp;

    // Khởi tạo phần mới
    time_t now = time(NULL);
    char* timeStr = ctime(&now);
    timeStr[strlen(timeStr) - 1] = '\0';
    for (int i = *currentSize; i < newSize; i++) {
        strncpy(arr[i].timestamp, timeStr, sizeof(arr[i].timestamp) - 1);
        arr[i].timestamp[sizeof(arr[i].timestamp) - 1] = '\0';
        arr[i].temperature = 0.0;
        arr[i].humidity = 0.0;
        arr[i].errorCode = 0;
    }
    *currentSize = newSize;
    return arr;
}

int main() {
    int size = 2;
    int currentSize = size;
    struct SensorData* sensorArray = initDynamicArray(size);

    if (sensorArray != NULL) {
        // Thêm dữ liệu mẫu
        sensorArray[0].temperature = 25.5;
        sensorArray[0].humidity = 60.0;

        // Mở rộng mảng nếu cần
        size = 4;
        sensorArray = expandArray(sensorArray, &currentSize, size);
        if (sensorArray != NULL) {
            sensorArray[2].temperature = 26.0;
            sensorArray[2].humidity = 61.0;

            // In để kiểm tra
            for (int i = 0; i < currentSize; i++) {
                printf("[%s] T: %.1f, H: %.1f\n", sensorArray[i].timestamp,
                       sensorArray[i].temperature, sensorArray[i].humidity);
            }

            free(sensorArray);
        }
    }

    return 0;
}

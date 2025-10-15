/* ui_report.c — Triển khai module UI & Report
 * Phạm Hữu Minh – HE201180
 * 
 * Cung cấp giao diện dạng ASCII hiển thị dữ liệu cảm biến, 
 * cảnh báo, biểu đồ và xuất báo cáo ra file report.txt
 */

#include "ui_report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int UI_USE_COLOR = 1;   // mặc định có màu
static int CHART_W = 50;       // chiều rộng biểu đồ ASCII

// Mã màu ANSI (nếu terminal hỗ trợ)
#define C_RESET  "\x1b[0m"
#define C_RED    "\x1b[31m"
#define C_YEL    "\x1b[33m"
#define C_CYAN   "\x1b[36m"
#define C_BOLD   "\x1b[1m"

// Hàm nội bộ: chọn dùng màu hay không
static const char* c(const char *code){ return UI_USE_COLOR ? code : ""; }

void ui_init(int use_color){ UI_USE_COLOR = use_color ? 1 : 0; }
void ui_set_chart_width(int w){ if(w >= 20 && w <= 120) CHART_W = w; }

/* In menu chính */
void ui_print_menu(FILE *out){
    fprintf(out,
        "%s========= MENU =========%s\n"
        "1) AUTO   - Thu thập & cảnh báo\n"
        "2) USER   - Xem & phân tích dữ liệu\n"
        "3) ADMIN  - Cấu hình ngưỡng cảnh báo\n"
        "4) REPORT - Xuất báo cáo report.txt\n"
        "0) EXIT   - Thoát chương trình\n",
        c(C_BOLD), c(C_RESET));
}

/* In bảng dữ liệu gần nhất */
void ui_print_table(FILE *out, const SensorData *data, size_t n){
    if(n == 0){ fprintf(out, "(No data)\n"); return; }

    fprintf(out, "%s%-19s  %7s  %8s  %7s%s\n",
        c(C_BOLD), "Timestamp", "Temp(C)", "Hum(%)", "Gas(ppm)", c(C_RESET));
    fprintf(out, "-------------------  -------  --------  -------\n");

    size_t start = (n > 20) ? n - 20 : 0;  // chỉ in 20 dòng cuối
    char buf[32];
    for(size_t i = start; i < n; i++){
        struct tm *tmv = localtime(&data[i].ts);
        strftime(buf, sizeof(buf), "%H:%M:%S", tmv);
        fprintf(out, "%-19s  %7.2f  %8.2f  %7d\n",
            buf, data[i].temperature, data[i].humidity, data[i].gas_ppm);
    }
}

/* Kiểm tra và in cảnh báo vượt ngưỡng */
int ui_print_alert(FILE *out, const SensorData *last, Thresholds th){
    if(!last) return 0;
    int alert = 0;

    if(last->temperature > th.temp_hi){
        fprintf(out, "%s[ALERT]%s Nhiệt độ cao: %.2f > %.2f°C\n",
                c(C_RED), c(C_RESET), last->temperature, th.temp_hi);
        alert = 1;
    }
    if(last->humidity > th.humid_hi){
        fprintf(out, "%s[ALERT]%s Độ ẩm cao: %.2f > %.2f%%\n",
                c(C_RED), c(C_RESET), last->humidity, th.humid_hi);
        alert = 1;
    }
    if(last->gas_ppm > th.gas_hi){
        fprintf(out, "%s[ALERT]%s Nồng độ khí gas cao: %d > %d ppm\n",
                c(C_RED), c(C_RESET), last->gas_ppm, th.gas_hi);
        alert = 1;
    }

    if(!alert)
        fprintf(out, "%s[OK]%s Dữ liệu trong ngưỡng an toàn.\n",
                c(C_CYAN), c(C_RESET));
    return alert;
}

/* Vẽ biểu đồ ASCII cho nhiệt độ và độ ẩm */
void ui_chart_temp_humid(FILE *out, const SensorData *data, size_t n){
    if(n == 0){ fprintf(out, "(No data to chart)\n"); return; }

    // tìm giá trị lớn nhất để scale
    float maxv = 1.0f;
    for(size_t i=0;i<n;i++){
        if(data[i].temperature > maxv) maxv = data[i].temperature;
        if(data[i].humidity > maxv)    maxv = data[i].humidity;
    }

    fprintf(out, "%s[Biểu đồ ASCII]%s (max=%.2f)\n",
            c(C_BOLD), c(C_RESET), maxv);

    size_t start = (n > 20) ? n - 20 : 0;
    for(size_t i = start; i < n; i++){
        int tlen = (int)((data[i].temperature / maxv) * CHART_W);
        int hlen = (int)((data[i].humidity / maxv) * CHART_W);

        fprintf(out, "T%02zu ", i);
        for(int k=0;k<tlen;k++) fputc('#', out);
        fprintf(out, " (%.1f°C)\n", data[i].temperature);

        fprintf(out, "H%02zu ", i);
        for(int k=0;k<hlen;k++) fputc('*', out);
        fprintf(out, " (%.1f%%)\n", data[i].humidity);
    }
}

/* Xuất báo cáo ra file report.txt */
int ui_write_report(FILE *out, const SensorData *data, size_t n,
                    const Stats *st, Thresholds th){
    if(!out || !st) return -1;

    fprintf(out, "=== Sensor Report ===\n");
    fprintf(out, "Samples: %zu\n\n", n);

    fprintf(out, "Temperature (°C): min %.2f | avg %.2f | max %.2f\n",
            st->t_min, st->t_avg, st->t_max);
    fprintf(out, "Humidity (%%):     min %.2f | avg %.2f | max %.2f\n",
            st->h_min, st->h_avg, st->h_max);
    fprintf(out, "Gas (ppm):         min %d   | avg %.1f | max %d\n",
            st->g_min, st->g_avg, st->g_max);

    fprintf(out, "\nThresholds: T>%.2f, H>%.2f, G>%d\n",
            th.temp_hi, th.humid_hi, th.gas_hi);

    if(n > 0){
        const SensorData *last = &data[n-1];
        fprintf(out, "Last sample: T=%.2f°C, H=%.2f%%, G=%d ppm\n",
                last->temperature, last->humidity, last->gas_ppm);
    }
    fprintf(out, "\n(End of report)\n");
    return 0;
}

/* ui_report.h — Header cho UI & Report (an toàn, chống phụ thuộc)
 * Phạm Hữu Minh – HE201180
 */
#ifndef UI_REPORT_H
#define UI_REPORT_H

#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include "ui_adapt.h"   /* dùng UIItem & UIItemExtractor */

/* Thống kê (có thể truyền NULL để UI tự tính từ buffer) */
typedef struct {
    float t_min, t_max, t_avg;
    float h_min, h_max, h_avg;
    int   g_min, g_max;
    double g_avg;
} UIStats;

typedef struct {
    float temp_hi;   /* ví dụ 35.0 */
    float humid_hi;  /* ví dụ 85.0 */
    int   gas_hi;    /* ví dụ 300 */
} UIThresholds;

/* Cấu hình UI */
void ui_init(int use_color);            /* bật/tắt màu ANSI */
void ui_set_chart_width(int width);     /* 20..120 */

/* In menu chính */
void ui_print_menu(FILE *out);

/* In bảng N dòng cuối cùng từ buffer generic */
void ui_print_table_g(FILE *out,
                      const void *buf, size_t n, UIItemExtractor ex);

/* Kiểm tra & in ALERT cho phần tử cuối cùng trong buffer generic.
 * Trả về 1 nếu có cảnh báo, 0 nếu an toàn. */
int ui_print_alert_g(FILE *out,
                     const void *buf, size_t n, UIItemExtractor ex,
                     UIThresholds th);

/* Vẽ biểu đồ ASCII (Temp/Humid) từ buffer generic */
void ui_chart_temp_humid_g(FILE *out,
                           const void *buf, size_t n, UIItemExtractor ex);

/* Ghi báo cáo. Nếu st==NULL → UI tự tính thống kê từ buffer.
 * Trả về 0 nếu OK, âm nếu lỗi. */
int ui_write_report_g(FILE *out,
                      const void *buf, size_t n, UIItemExtractor ex,
                      const UIStats *st_or_null, UIThresholds th);

/* ====== Tùy chọn tiện lợi (nếu nhóm dùng đúng struct SensorData) ====== */
/* Định nghĩa macro này trước khi include nếu bạn có system.h chuẩn:
   #define UI_HAVE_SYSTEM_SENSOR
*/
#ifdef UI_HAVE_SYSTEM_SENSOR
#include "system.h"
void ui_print_table(FILE *out, const SensorData *a, size_t n);
int  ui_print_alert(FILE *out, const SensorData *a, size_t n, UIThresholds th);
void ui_chart_temp_humid(FILE *out, const SensorData *a, size_t n);
int  ui_write_report(FILE *out, const SensorData *a, size_t n,
                     const UIStats *st_or_null, UIThresholds th);
#endif

#endif

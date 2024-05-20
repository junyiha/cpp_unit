#include <stdio.h>
#include <stdlib.h>
#include <sensors/sensors.h>

int main() {
    // 初始化 libsensors 库
    sensors_init(NULL);
    
    // 获取传感器列表
    const sensors_chip_name *chip_name;
    int chip_nr = 0;
    while ((chip_name = sensors_get_detected_chips(NULL, &chip_nr)) != 0) {
        printf("Chip: %s\n", chip_name->prefix);

        // 获取该芯片下的所有特性
        const sensors_feature *feature;
        int feature_nr = 0;
        while ((feature = sensors_get_features(chip_name, &feature_nr)) != 0) {
            printf("  Feature: %s\n", feature->name);

            // 如果是温度特性，获取其值
            if (feature->type == SENSORS_FEATURE_TEMP) {
                double value;
                if (sensors_get_value(chip_name, feature->number, &value) == 0) {
                    printf("    Temperature: %.2f °C\n", value);
                } else {
                    printf("    Failed to read temperature\n");
                }
            }
        }
    }
    
    // 清理并关闭 libsensors 库
    sensors_cleanup();

    return 0;
}

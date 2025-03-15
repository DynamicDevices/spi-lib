#ifndef IFX_TEST_HELPER_H_
#define IFX_TEST_HELPER_H_

#include "arm_math_types.h"

uint32_t count_lines(const char* filename);
uint32_t read_float32_from_file(const char* filename, float32_t* array, uint32_t length);
void write_float32_to_file(const char* filename, float32_t* array, uint32_t length, uint8_t append);
void write_uint32_to_file(const char* filename, uint32_t* array, uint32_t length, uint8_t append);

#endif

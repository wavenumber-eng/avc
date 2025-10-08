#include "stdint.h"
#include "stdbool.h"

#ifndef AVC__LINE_PROCESSOR_H_
#define AVC__LINE_PROCESSOR_H_


// HSL structure for convenience
typedef struct {
    float h;  // Hue: 0-360 degrees
    float s;  // Saturation: 0-1
    float l;  // Lightness: 0-1
} hsl_t;


// Convert single RGB565 pixel to normalized Y (0.0 to 1.0)
static inline float rgb565_to_y_float(uint16_t rgb565)
{
    // Extract RGB components (5-6-5 bits)
    uint8_t r5 = (rgb565 >> 11) & 0x1F;
    uint8_t g6 = (rgb565 >> 5) & 0x3F;
    uint8_t b5 = rgb565 & 0x1F;

    // Convert to 8-bit values
    uint8_t r8 = (r5 << 3) | (r5 >> 2);  // Expand 5-bit to 8-bit
    uint8_t g8 = (g6 << 2) | (g6 >> 4);  // Expand 6-bit to 8-bit
    uint8_t b8 = (b5 << 3) | (b5 >> 2);  // Expand 5-bit to 8-bit

    // Convert to normalized float (0.0 to 1.0)
    float r = r8 / 255.0f;
    float g = g8 / 255.0f;
    float b = b8 / 255.0f;

    // ITU-R BT.601 luminance coefficients
    // Y = 0.299*R + 0.587*G + 0.114*B
    return 0.299f * r + 0.587f * g + 0.114f * b;
}

// Fast approximation version (using integer coefficients similar to original)
static inline float rgb565_to_y_float_fast(uint16_t rgb565)
{
    // Extract and convert to 8-bit
    uint8_t r8 = ((rgb565 >> 11) & 0x1F) << 3;
    uint8_t g8 = ((rgb565 >> 5) & 0x3F) << 2;
    uint8_t b8 = (rgb565 & 0x1F) << 3;

    // Fast approximation: 77/256 ≈ 0.301, 150/256 ≈ 0.586, 29/256 ≈ 0.113
    uint16_t y256 = (r8 * 77 + g8 * 150 + b8 * 29) >> 8;

    return y256 / 255.0f;
}


// Convert RGB565 to HSL with struct array output
void avc__convert_rgb565_to_hsl_struct(uint16_t *input,
                                       hsl_t *output,
                                       uint32_t length);


void avc__convert_rgb565_to_y8(uint16_t  * input,
							   uint8_t * output,
							   uint32_t  length);

// Structure to hold edge detection results
typedef struct {
    uint16_t position;  // Pixel position of edge
    uint8_t type;       // 0 = falling edge (1->0), 1 = rising edge (0->1)
} edge_t;

// Structure to hold line segment information
typedef struct {
    uint16_t start;     // Start position of line segment
    uint16_t end;       // End position of line segment
    uint16_t center;    // Center of this segment
    uint16_t width;     // Width of segment
} line_segment_t;


/**
 * Detect edges in binary filter array
 * Returns number of edges found
 */
uint32_t avc__detect_edges(uint8_t *filter, uint32_t length, edge_t *edges, uint32_t max_edges);

/**
 * Find line segments (continuous black regions)
 * Returns number of segments found
 */
uint32_t avc__find_line_segments(uint8_t *filter, uint32_t length,
                                  line_segment_t *segments, uint32_t max_segments);

uint32_t avc__find_segments_from_hsl_adaptive(hsl_t *hsl_array,
                                              uint32_t length,
                                              float luminance_threshold,
                                              line_segment_t *segments,
                                              uint32_t max_segments,
                                              bool find_dark);

uint32_t avc__find_segments_from_hsl_hybrid(hsl_t *hsl_array,
                                            uint32_t length,
                                            float luminance_threshold,
                                            float gradient_threshold,
                                            line_segment_t *segments,
                                            uint32_t max_segments,
                                            bool find_dark);

#endif /* AVC__LINE_PROCESSOR_H_ */

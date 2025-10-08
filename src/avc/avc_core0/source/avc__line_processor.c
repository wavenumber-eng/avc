#include "avc__line_processor.h"
#include "stdint.h"
#include "eGFX.h"
#include <stdint.h>
#include <math.h>

// Helper macros for min/max
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif


#define RGB888_TO_RGB565(R,G,B)     ( ((R>>3)<<11)   | ((G>>2)<<5)   | (B>>3)   )
#define RGB_TO_RGBX888(R,G,B)       ( ((R&0xFF)<<16) | ((G&0xFF)<<8) | (B&0xff) )

#define RGB565_TO_R5(rgb565)		((((uint16_t)(rgb565))>>11)&0x1F)
#define RGB565_TO_G6(rgb565)		((((uint16_t)(rgb565))>>5)&0x3F)
#define RGB565_TO_B5(rgb565)		(((uint16_t)(rgb565))&0x1F)

#define RGB565_TO_R8(rgb565)		 (RGB565_TO_R5(rgb565)<<3)
#define RGB565_TO_G8(rgb565)		 (RGB565_TO_G6(rgb565)<<2)
#define RGB565_TO_B8(rgb565)		 (RGB565_TO_B5(rgb565)<<3)



//Fast Approx of Luminance from rgb565

#define RGB565_TO_Y8(rgb565)	    (\
											( \
											   (((uint16_t)RGB565_TO_R8(rgb565))*77) + \
											   (((uint16_t)RGB565_TO_G8(rgb565))*150) + \
											   (((uint16_t)RGB565_TO_B8(rgb565))*29) \
											) >> 8 \
										)


//Fast Approx of Luminance to rgb565

#define Y8_TO_RGB565(Y8)		(((((uint16_t)(Y8))>>3)<<11) |  \
									((((uint16_t)(Y8))>>2)<<5)   |  \
									((((uint16_t)(Y8))>>3)))


/*
 * Convert RGB565 Color to grayscale
 *
 */
void avc__convert_rgb565_to_y8(uint16_t  * input,
							   uint8_t * output,
							   uint32_t  length)
{
	for(int i=0;i<length;i++)
	{
		output[i] = RGB565_TO_Y8(input[i]);
	}

}

void avc__convert_rgb565_to_r5(uint16_t  * input,
							   uint8_t * output,
							   uint32_t  length)
{
	for(int i=0;i<length;i++)
	{
		output[i] = RGB565_TO_R5(input[i]);
	}
}


uint8_t avc__find_uint8_max(uint8_t  * input,
				      uint32_t  length)
{
	uint8_t max =0;

	for(int i=0;i<length;i++)
	{
		if(input[i] > max)
		{
			max = input[i];
		}
	}

	return max;
}

uint8_t avc__find_uint8_min(uint8_t  * input,
				      uint32_t  length)
{
	uint8_t min = 0xFF;

	for(int i=0;i<length;i++)
	{
		if(input[i] < min)
		{
			min = input[i];
		}
	}

	return min;
}


// Convert single RGB565 pixel to HSL
static inline hsl_t rgb565_to_hsl(uint16_t rgb565)
{
    hsl_t hsl;

    // Extract RGB components (5-6-5 bits)
    uint8_t r5 = (rgb565 >> 11) & 0x1F;
    uint8_t g6 = (rgb565 >> 5) & 0x3F;
    uint8_t b5 = rgb565 & 0x1F;

    // Convert to 8-bit values with proper bit replication
    uint8_t r8 = (r5 << 3) | (r5 >> 2);  // Expand 5-bit to 8-bit
    uint8_t g8 = (g6 << 2) | (g6 >> 4);  // Expand 6-bit to 8-bit
    uint8_t b8 = (b5 << 3) | (b5 >> 2);  // Expand 5-bit to 8-bit

    // Convert to normalized float (0.0 to 1.0)
    float r = r8 / 255.0f;
    float g = g8 / 255.0f;
    float b = b8 / 255.0f;

    // Find min and max values
    float max = MAX(MAX(r, g), b);
    float min = MIN(MIN(r, g), b);
    float delta = max - min;

    // Calculate Lightness
    hsl.l = (max + min) / 2.0f;

    if (delta < 0.00001f) {
        // Achromatic (gray)
        hsl.h = 0.0f;
        hsl.s = 0.0f;
    } else {
        // Calculate Saturation
        if (hsl.l < 0.5f) {
            hsl.s = delta / (max + min);
        } else {
            hsl.s = delta / (2.0f - max - min);
        }

        // Calculate Hue
        if (max == r) {
            hsl.h = (g - b) / delta;
            if (g < b) {
                hsl.h += 6.0f;
            }
        } else if (max == g) {
            hsl.h = 2.0f + (b - r) / delta;
        } else {
            hsl.h = 4.0f + (r - g) / delta;
        }

        hsl.h *= 60.0f;  // Convert to degrees
    }

    return hsl;
}

// Convert RGB565 to HSL with struct array output
void avc__convert_rgb565_to_hsl_struct(uint16_t *input,
                                       hsl_t *output,
                                       uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        output[i] = rgb565_to_hsl(input[i]);
    }
}


/**
 * Detect edges in binary filter array
 * Returns number of edges found
 */
uint32_t avc__detect_edges(uint8_t *filter, uint32_t length, edge_t *edges, uint32_t max_edges)
{
    uint32_t edge_count = 0;

    if (length < 2 || max_edges == 0)
    {
        return 0;
    }

    for (uint32_t i = 1; i < length && edge_count < max_edges; i++)
    {
        // Rising edge: 0 -> 1
        if (filter[i-1] == 0 && filter[i] == 1)
        {
            edges[edge_count].position = i;
            edges[edge_count].type = 1;
            edge_count++;
        }
        // Falling edge: 1 -> 0
        else if (filter[i-1] == 1 && filter[i] == 0)
        {
            edges[edge_count].position = i;
            edges[edge_count].type = 0;
            edge_count++;
        }
    }

    return edge_count;
}

/**
 * Find line segments (continuous black regions)
 * Returns number of segments found
 */
uint32_t avc__find_line_segments(uint8_t *filter, uint32_t length,
                                  line_segment_t *segments, uint32_t max_segments)
{
    uint32_t segment_count = 0;
    uint32_t segment_start = 0;
    bool in_segment = false;

    for (uint32_t i = 0; i < length && segment_count < max_segments; i++)
    {
        if (filter[i] && !in_segment)
        {
            // Start of new segment
            segment_start = i;
            in_segment = true;
        }
        else if (!filter[i] && in_segment)
        {
            // End of segment
            segments[segment_count].start = segment_start;
            segments[segment_count].end = i - 1;
            segments[segment_count].width = i - segment_start;
            segments[segment_count].center = (segment_start + i - 1) / 2;
            segment_count++;
            in_segment = false;
        }
    }

    // Handle case where segment extends to end of array
    if (in_segment && segment_count < max_segments)
    {
        segments[segment_count].start = segment_start;
        segments[segment_count].end = length - 1;
        segments[segment_count].width = length - segment_start;
        segments[segment_count].center = (segment_start + length - 1) / 2;
        segment_count++;
    }

    return segment_count;
}

/**
 * Calculate luminance gradient with smoothing
 * Internal helper function
 */
static void calculate_gradient_smooth(hsl_t *hsl_array,
                                      float *gradient_out,
                                      uint32_t length)
{
    if (length < 3) {
        if (length == 2) {
            gradient_out[0] = hsl_array[1].l - hsl_array[0].l;
            gradient_out[1] = gradient_out[0];
        }
        return;
    }

    // First element - forward difference
    gradient_out[0] = hsl_array[1].l - hsl_array[0].l;

    // Middle elements - central difference
    for (uint32_t i = 1; i < length - 1; i++)
    {
        gradient_out[i] = (hsl_array[i + 1].l - hsl_array[i - 1].l) / 2.0f;
    }

    // Last element - backward difference
    gradient_out[length - 1] = hsl_array[length - 1].l - hsl_array[length - 2].l;
}

/**
 * Main function: Find line segments from HSL data using gradient-based edge detection
 *
 * @param hsl_array Input HSL data array
 * @param length Length of input array
 * @param luminance_threshold Threshold for dark/light classification (0-1)
 * @param gradient_threshold Minimum gradient to consider as edge (0-1, typically 0.01-0.1)
 * @param segments Output array of line segments
 * @param max_segments Maximum number of segments to find
 * @param find_dark If true, finds dark segments; if false, finds light segments
 * @return Number of segments found
 */
uint32_t avc__find_segments_from_hsl_gradient(hsl_t *hsl_array,
                                              uint32_t length,
                                              float luminance_threshold,
                                              float gradient_threshold,
                                              line_segment_t *segments,
                                              uint32_t max_segments,
                                              bool find_dark)
{
    if (length < 2 || max_segments == 0) return 0;

    // Allocate gradient array
    float gradient[length];
    calculate_gradient_smooth(hsl_array, gradient, length);

    // Find edges based on gradient magnitude
    uint16_t edge_positions[100];  // Adjust size as needed
    uint32_t edge_count = 0;
    const uint32_t max_edges = 100;

    // Detect significant gradient changes (edges)
    for (uint32_t i = 1; i < length - 1 && edge_count < max_edges; i++)
    {
        float grad_mag = fabsf(gradient[i]);

        // Check for local maximum in gradient magnitude
        if (grad_mag > gradient_threshold &&
            grad_mag >= fabsf(gradient[i - 1]) &&
            grad_mag >= fabsf(gradient[i + 1]))
        {
            edge_positions[edge_count++] = i;
            // Skip nearby points to avoid duplicate edges
            i += 1;
        }
    }

    // Now create segments between edges (or from start/end to edges)
    uint32_t segment_count = 0;
    uint32_t current_pos = 0;

    // Add implicit edges at start and end if needed
    for (uint32_t i = 0; i <= edge_count && segment_count < max_segments; i++)
    {
        uint32_t next_edge = (i < edge_count) ? edge_positions[i] : length;

        if (next_edge > current_pos)
        {
            // Calculate average luminance for this region
            float sum_luminance = 0;
            for (uint32_t j = current_pos; j < next_edge; j++)
            {
                sum_luminance += hsl_array[j].l;
            }
            float avg_luminance = sum_luminance / (next_edge - current_pos);

            // Check if this segment matches what we're looking for
            bool is_dark = (avg_luminance < luminance_threshold);
            if ((find_dark && is_dark) || (!find_dark && !is_dark))
            {
                segments[segment_count].start = current_pos;
                segments[segment_count].end = next_edge - 1;
                segments[segment_count].width = next_edge - current_pos;
                segments[segment_count].center = (current_pos + next_edge - 1) / 2;
                segment_count++;
            }
        }

        current_pos = next_edge;
    }

    return segment_count;
}

/**
 * Simplified function using adaptive thresholding
 * Automatically adjusts gradient threshold based on image contrast
 */
uint32_t avc__find_segments_from_hsl_adaptive(hsl_t *hsl_array,
                                              uint32_t length,
                                              float luminance_threshold,
                                              line_segment_t *segments,
                                              uint32_t max_segments,
                                              bool find_dark)
{
    if (length < 2 || max_segments == 0) return 0;

    // Calculate gradient
    float gradient[length];
    calculate_gradient_smooth(hsl_array, gradient, length);

    // Calculate adaptive threshold based on gradient statistics
    float sum_grad = 0;
    float max_grad = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        float abs_grad = fabsf(gradient[i]);
        sum_grad += abs_grad;
        if (abs_grad > max_grad) max_grad = abs_grad;
    }
    float avg_grad = sum_grad / length;

    // Use average gradient as baseline, require 2x average for edge
    float adaptive_threshold = avg_grad * 2.0f;
    if (adaptive_threshold < 0.01f) adaptive_threshold = 0.01f;  // Minimum threshold

    // Use the main function with adaptive threshold
    return avc__find_segments_from_hsl_gradient(hsl_array, length,
                                                luminance_threshold,
                                                adaptive_threshold,
                                                segments, max_segments,
                                                find_dark);
}

/**
 * Hybrid approach: Combines gradient edges with luminance thresholding
 * Better for noisy images or when edges aren't well defined
 */
uint32_t avc__find_segments_from_hsl_hybrid(hsl_t *hsl_array,
                                            uint32_t length,
                                            float luminance_threshold,
                                            float gradient_threshold,
                                            line_segment_t *segments,
                                            uint32_t max_segments,
                                            bool find_dark)
{
    if (length < 2 || max_segments == 0) return 0;

    // First pass: Create binary filter based on luminance threshold
    uint8_t binary_filter[length];
    for (uint32_t i = 0; i < length; i++)
    {
        if (find_dark) {
            binary_filter[i] = (hsl_array[i].l < luminance_threshold) ? 1 : 0;
        } else {
            binary_filter[i] = (hsl_array[i].l >= luminance_threshold) ? 1 : 0;
        }
    }

    // Second pass: Refine edges using gradient information
    float gradient[length];
    calculate_gradient_smooth(hsl_array, gradient, length);

    // Find strong gradient positions
    for (uint32_t i = 1; i < length - 1; i++)
    {
        float grad_mag = fabsf(gradient[i]);

        // If we have a strong gradient, ensure the edge is marked correctly
        if (grad_mag > gradient_threshold)
        {
            // For rising edge (dark to light)
            if (gradient[i] > gradient_threshold)
            {
                if (i > 0) binary_filter[i - 1] = find_dark ? 1 : 0;
                binary_filter[i] = find_dark ? 0 : 1;
            }
            // For falling edge (light to dark)
            else if (gradient[i] < -gradient_threshold)
            {
                binary_filter[i] = find_dark ? 1 : 0;
                if (i < length - 1) binary_filter[i + 1] = find_dark ? 0 : 1;
            }
        }
    }

    // Now find segments from refined binary filter
    uint32_t segment_count = 0;
    uint32_t segment_start = 0;
    bool in_segment = false;

    for (uint32_t i = 0; i < length && segment_count < max_segments; i++)
    {
        if (binary_filter[i] && !in_segment)
        {
            segment_start = i;
            in_segment = true;
        }
        else if (!binary_filter[i] && in_segment)
        {
            segments[segment_count].start = segment_start;
            segments[segment_count].end = i - 1;
            segments[segment_count].width = i - segment_start;
            segments[segment_count].center = (segment_start + i - 1) / 2;
            segment_count++;
            in_segment = false;
        }
    }

    // Handle segment extending to end
    if (in_segment && segment_count < max_segments)
    {
        segments[segment_count].start = segment_start;
        segments[segment_count].end = length - 1;
        segments[segment_count].width = length - segment_start;
        segments[segment_count].center = (segment_start + length - 1) / 2;
        segment_count++;
    }

    // Filter out very small segments (noise)
    uint32_t filtered_count = 0;
    for (uint32_t i = 0; i < segment_count; i++)
    {
        if (segments[i].width >= 3)  // Minimum width of 3 pixels
        {
            if (i != filtered_count)
            {
                segments[filtered_count] = segments[i];
            }
            filtered_count++;
        }
    }

    return filtered_count;
}


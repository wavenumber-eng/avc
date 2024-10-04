#ifndef ST7789_H_
#define ST7789_H_

#include "stdint.h"

#define PORTRAIT 1
#define LANDSCAPE 2
#define CONFIG_DISPLAY_ORIENTATION	LANDSCAPE	

#define LITTLE_ENDIAN   1
#define BIG_ENDIAN      2
#define CONFIG_DISPLAY_ENDIANESS    LITTLE_ENDIAN

#define LCD_RS__SET		GPIO_PinWrite(GPIO0,26,1)
#define LCD_RS__CLR		GPIO_PinWrite(GPIO0,26,0)
#define LCD_RES__SET	GPIO_PinWrite(GPIO0,28,1)
#define LCD_RES__CLR	GPIO_PinWrite(GPIO0,28,0)


extern void ST7789_Initial(void);
extern void Write_Cmd_Data(unsigned char x);
extern void Write_Cmd(unsigned char DL);
extern void Write_Data(unsigned char DH,unsigned char DL);
extern void delayms(unsigned int tt);
extern void Write_Data_U16(uint16_t y);
extern inline void  Write_Data_U32(uint32_t y);
extern void LCD_SetPos(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1);
extern void ClearScreen(unsigned int bColor);
extern void LCD_FULL(unsigned int i);
extern void ST7789__display_img(uint8_t *image_buff);
extern void ST7789__display_row(uint8_t * image_buff, uint32_t row_size);

#endif

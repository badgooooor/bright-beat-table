#ifndef WS2812B_H
#define WS2812B_H

#include "main.h"
#include <string.h>

extern TIM_HandleTypeDef htim1;

#define LED_CFG_USE_RGBW                0       /*!< Set to 1 to use RGBW leds.
                                                    Set to 0 to use WS2812B leds */

#define LED_CFG_LEDS_CNT                90       /*!< Number of leds in a strip row */

#if LED_CFG_USE_RGBW
#define LED_CFG_BYTES_PER_LED           4
#else /* LED_CFG_USE_RGBW */
#define LED_CFG_BYTES_PER_LED           3
#endif /* !LED_CFG_USE_RGBW */

#define LED_CFG_RAW_BYTES_PER_LED       (LED_CFG_BYTES_PER_LED * 8)

class WSLED {

/**
 * \brief           Array of 4x (or 3x) number of leds (R, G, B[, W] colors)
 */
	private:
	uint8_t leds_colors[LED_CFG_BYTES_PER_LED * LED_CFG_LEDS_CNT];
	uint16_t leds_raw_pwm[(LED_CFG_BYTES_PER_LED * LED_CFG_LEDS_CNT * 8) + (6 * LED_CFG_RAW_BYTES_PER_LED)];

/**
 * \brief           Temporary array for dual LED with extracted PWM duty cycles
 * 
 * We need LED_CFG_RAW_BYTES_PER_LED bytes for PWM setup to send all bits.
 * Before we can send data for first led, we have to send reset pulse, which must be 50us long.
 * PWM frequency is 800kHz, to achieve 50us, we need to send 40 pulses with 0 duty cycle = make array size MAX(2 * LED_CFG_RAW_BYTES_PER_LED, 40)
 */
	uint32_t tmp_led_data[2 * LED_CFG_RAW_BYTES_PER_LED];

	uint8_t          is_reset_pulse;     /*!< Status if we are sending reset pulse or led data */
	volatile uint8_t is_updating;        /*!< Is updating in progress? */
	uint32_t         current_led;        /*!< Current LED number we are sending */

	public:
	WSLED();
	void        led_init(void);
	uint8_t     led_update(uint8_t block);

	#if LED_CFG_USE_RGBW
	uint8_t     led_set_color(size_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
	uint8_t     led_set_color_all(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
	uint8_t     led_set_color_rgbw(size_t index, uint32_t rgbw);
	uint8_t     led_set_color_all_rgbw(uint32_t rgbw);
	#else /* LED_CFG_USE_RGBW */
	uint8_t     led_set_color(size_t index, uint8_t r, uint8_t g, uint8_t b);
	uint8_t     led_set_color_all(uint8_t r, uint8_t g, uint8_t b);
	uint8_t     led_set_color_rgb(size_t index, uint32_t rgb);
	uint8_t     led_set_color_all_rgb(uint32_t rgb);
	#endif /* !LED_CFG_USE_RGBW */

	uint8_t     led_is_update_finished(void);
	uint8_t     led_start_reset_pulse(uint8_t num);
	uint8_t     led_fill_led_pwm_data(size_t ledx);
	void led_display(void);

};


#endif




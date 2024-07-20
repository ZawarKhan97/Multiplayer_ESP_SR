#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stddef.h> // Include for NULL definition
#include <audio_data.h>
#include <esp_afe_sr_iface.h>
#include <esp_afe_sr_models.h>
#include <esp_agc.h>

/* for my dev Kit */
#define I2S_WS  41 
#define I2S_SD  39
#define I2S_SCK 40
// I2S port
#define I2S_PORT I2S_NUM_0

// Buffer length
#define BUFFER_LENGTH 160

// AGC enable flag
#define AGC_ENABLE 1
/* COnfiguration Parameters*/
#define SAMPLE_RATE 16000
#define AGC_MODE 3
#define Gain_dB 100
#define LIMITER_ENABLE 0
const int TARGET_LEVEL=5;
/* AFE Handle Configuration*/
static esp_afe_sr_iface_t *afe_handle= NULL;
static esp_afe_sr_data_t *afe_data=NULL;
void* agc_handle;
/* AGC attributes config*/

/*Function Prototypes*/
void i2s_init();
void play_audio();
int16_t* convert_audio_data(const uint8_t *audio_data, size_t audio_data_len);


#endif // MAIN_H
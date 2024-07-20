/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <driver/i2s.h>
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include "main.h"

void app_main(void)
{
    printf("Multiplayer Started!\n");
    i2s_init();
    agc_handle = esp_agc_open(AGC_MODE, SAMPLE_RATE);
    if (agc_handle == NULL) {
        printf("Failed To open AGC Handle");
        return;
    }
    set_agc_config(agc_handle,Gain_dB,LIMITER_ENABLE,TARGET_LEVEL);
    play_audio();
}


void play_audio()
{
  size_t bytes_written;
  size_t offset=0;
  int16_t AGC_processed_Audio_Data[BUFFER_LENGTH];

  
  while(true)
  {
    if (offset >= audio_data_len* sizeof(int8_t)-BUFFER_LENGTH) 
        {
            offset = 0;
        }

    size_t bytes_to_write=audio_data_len* sizeof(int16_t)-offset;
    if (AGC_ENABLE)
    {
      if (bytes_to_write>BUFFER_LENGTH)
      {
       bytes_to_write=BUFFER_LENGTH;
      }
    }
    else
        {
          if (bytes_to_write>512)
          {
            bytes_to_write=512;
          }
        } 
    
    
    // Convert audio data from uint8_t to int16_t
    int16_t* converted_Data=convert_audio_data(&audio_data[offset],bytes_to_write*sizeof(int8_t));
    if(AGC_ENABLE)
      {
        /*Apply AGC to Chunk of Audio Data*/
        int ret = esp_agc_process(agc_handle, converted_Data, AGC_processed_Audio_Data, BUFFER_LENGTH, SAMPLE_RATE);
        if (ret < 0) 
          {
            printf("AGC Error Occured");
          }
          i2s_write(I2S_PORT, AGC_processed_Audio_Data, bytes_to_write, &bytes_written, portMAX_DELAY);
          offset += bytes_written;
      }
    else
      { 
       i2s_write(I2S_PORT, converted_Data, bytes_to_write, &bytes_written, portMAX_DELAY);
       offset += bytes_written;
      }
    // afe_handle->feed(afe_data,&converted_Data[offset]);
    // afe_fetch_result_t* res=afe_handle->fetch(afe_data);
    // // Serial.println(res->data_size);
    // i2s_write(I2S_PORT,res->data,res->data_size,&bytes_written,portMAX_DELAY);
    // offset+=bytes_written;

    free(converted_Data);
    vTaskDelay(pdMS_TO_TICKS(8));
  }

}


void i2s_init()
{
  i2s_config_t i2s_config ={
    .mode= (I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate=SAMPLE_RATE,
    .bits_per_sample= 16,
    .channel_format=I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format=( I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags=ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count=8,
    .dma_buf_len= BUFFER_LENGTH,
    .use_apll=false
    // .tx_desc_auto_clear=true,
    // .fixed_mclk=0
  };
  i2s_pin_config_t pin_config={
    .bck_io_num=I2S_SCK,
    .ws_io_num=I2S_WS,
    .data_out_num=I2S_SD,
    .data_in_num=I2S_PIN_NO_CHANGE
  };
  i2s_driver_install(I2S_PORT,&i2s_config,0,NULL);
  i2s_set_pin(I2S_PORT,&pin_config);
  i2s_set_sample_rates(I2S_PORT,SAMPLE_RATE );
}

int16_t* convert_audio_data(const uint8_t *audio_data, size_t audio_data_len) {

  // Step 1: Convert from uint8_t to uint16_t
  int16_t *audio_data_uint16 = (int16_t *)malloc(audio_data_len / 2 * sizeof(int16_t));
  for (size_t i = 0; i < audio_data_len / 2; i++) 
  {
    audio_data_uint16[i] = (audio_data[2 * i + 1] << 8) | audio_data[2 * i];
  }

  return audio_data_uint16;
}

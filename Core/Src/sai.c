/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : SAI.c
  * Description        : This file provides code for the configuration
  *                      of the SAI instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "sai.h"
#include "usbd_cdc_if.h"
#include "main.h"
#include "usart.h"
#include "string.h"
/* USER CODE BEGIN 0 */
extern float32_t data_mic_left[WAV_WRITE_SAMPLE_COUNT];
extern int32_t data_sai[WAV_WRITE_SAMPLE_COUNT];
extern int8_t audio_mode;

arm_rfft_fast_instance_f32 fftInstance;
uint8_t audio_message[WAV_WRITE_SAMPLE_COUNT/2 * sizeof(float32_t)];
uint16_t audio_pointer = 0;
uint8_t audio_header[12+4+1]; // 0 - FFT, 1 - USB




/* USER CODE END 0 */

SAI_HandleTypeDef hsai_BlockB1;
DMA_HandleTypeDef hdma_sai1_b;

/* SAI1 init function */
void MX_SAI1_Init(void)
{

  /* USER CODE BEGIN SAI1_Init 0 */
  uint32_t signature[3];
  signature[0] = HAL_GetUIDw0();
  signature[1] = HAL_GetUIDw1();
  signature[2] = HAL_GetUIDw2();

  memcpy(audio_header, signature, sizeof(signature));
  /* USER CODE END SAI1_Init 0 */

  /* USER CODE BEGIN SAI1_Init 1 */

  /* USER CODE END SAI1_Init 1 */

  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.AudioMode = SAI_MODEMASTER_RX;
  hsai_BlockB1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockB1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_MONOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SAI1_Init 2 */

  /* USER CODE END SAI1_Init 2 */

}
static uint32_t SAI1_client =0;

void HAL_SAI_MspInit(SAI_HandleTypeDef* saiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
/* SAI1 */
    if(saiHandle->Instance==SAI1_Block_B)
    {
      /* SAI1 clock enable */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

      if (SAI1_client == 0)
      {
       __HAL_RCC_SAI1_CLK_ENABLE();
      }
    SAI1_client ++;

    /**SAI1_B_Block_B GPIO Configuration
    PB3 (JTDO/TRACESWO)     ------> SAI1_SCK_B
    PB5     ------> SAI1_SD_B
    PB6     ------> SAI1_FS_B
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral DMA init*/

    hdma_sai1_b.Instance = DMA2_Channel7;
    hdma_sai1_b.Init.Request = DMA_REQUEST_1;
    hdma_sai1_b.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sai1_b.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_b.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_b.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sai1_b.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sai1_b.Init.Mode = DMA_CIRCULAR;
    hdma_sai1_b.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_sai1_b) != HAL_OK)
    {
      Error_Handler();
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(saiHandle,hdmarx,hdma_sai1_b);
    __HAL_LINKDMA(saiHandle,hdmatx,hdma_sai1_b);
    }
}

void HAL_SAI_MspDeInit(SAI_HandleTypeDef* saiHandle)
{

/* SAI1 */
    if(saiHandle->Instance==SAI1_Block_B)
    {
    SAI1_client --;
      if (SAI1_client == 0)
      {
      /* Peripheral clock disable */
      __HAL_RCC_SAI1_CLK_DISABLE();
      }

    /**SAI1_B_Block_B GPIO Configuration
    PB3 (JTDO/TRACESWO)     ------> SAI1_SCK_B
    PB5     ------> SAI1_SD_B
    PB6     ------> SAI1_FS_B
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6);

    HAL_DMA_DeInit(saiHandle->hdmarx);
    HAL_DMA_DeInit(saiHandle->hdmatx);
    }
}

void makeAudioMassage(uint8_t *data, uint32_t len) {

    // Prepare the data for transmission
    memcpy(audio_message+audio_pointer, data, len); 
}


void StartAudioProcessing(uint8_t mode, uint32_t num_samples) {
    // Initialize FFT instance
    audio_header[12] = mode;
    if (mode == 0) {
        // FFT mode
    	audio_mode = 0;
      arm_rfft_fast_init_f32(&fftInstance, WAV_WRITE_SAMPLE_COUNT);
        audio_header[13] = (uint8_t*)2048;
    } else if (mode == 1) {
        // USB mode
        audio_mode = 1;
        audio_header[13] = (uint8_t*)num_samples;
    } else if (mode == 2) {
        // complex mode
        audio_mode = 2;
        arm_rfft_fast_init_f32(&fftInstance, WAV_WRITE_SAMPLE_COUNT);
        audio_header[13] = (uint8_t*)4096;
    } else {
        // Invalid mode
        return;
    }
    // Start DMA-based SAI reception
    HAL_SAI_Receive_DMA(&hsai_BlockB1, (uint8_t *)data_sai, WAV_WRITE_SAMPLE_COUNT);
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) {

  HAL_SAI_DMAStop(&hsai_BlockB1);
  // Process buffer with 8-bit shift
  for (int i = 0; i < WAV_WRITE_SAMPLE_COUNT; i++) {
    data_mic_left[i] = (float32_t)(data_sai[i]>>8);
  }

  if (audio_mode == 0) { // Process FFT
    ProcessFFT(&data_mic_left, &data_mic_left);
  }
  else if (audio_mode == 1) { // Send data over USB

    CDC_Transmit_FS((uint8_t *)data_mic_left, WAV_WRITE_SAMPLE_COUNT * sizeof(float32_t));
  }
}

void ProcessFFT(float32_t *input, float32_t *output) {
    // Perform FFT
    if (arm_rfft_fast_init_f32(&fftInstance, WAV_WRITE_SAMPLE_COUNT) != ARM_MATH_SUCCESS) {
            Error_Handler();
    }
    arm_rfft_fast_f32(&fftInstance, input, output, 0);
    float32_t mag[WAV_WRITE_SAMPLE_COUNT / 2];
    // Compute magnitude
    for (int i = 0; i < WAV_WRITE_SAMPLE_COUNT / 2; i++) {
        mag[i] = sqrtf(output[2 * i] * output[2 * i] + output[2 * i + 1] * output[2 * i + 1]);
    }
    makeAudioMassage(mag, sizeof(mag));
    HAL_UART_Transmit(&huart4, (uint8_t*)audio_header, sizeof(audio_header), 1000);
    HAL_UART_Transmit(&huart4, (uint8_t*)audio_header, sizeof(audio_header), 1000);

    //sendUSBMasssage(audio_message, sizeof(audio_message));
}

/**
  * @}
  */

/**
  * @}
  */

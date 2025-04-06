/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sai.h
  * @brief   This file contains all the function prototypes for
  *          the sai.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SAI_H__
#define __SAI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include "arm_math.h"
/* USER CODE END Includes */

extern SAI_HandleTypeDef hsai_BlockB1;

/* USER CODE BEGIN Private defines */
/*FFT variables*/
extern int32_t data_sai[WAV_WRITE_SAMPLE_COUNT];
extern volatile int16_t sample_sai;
extern volatile uint8_t  half_sai, full_sai;

/* USER CODE END Private defines */

void MX_SAI1_Init(void);

/* USER CODE BEGIN Prototypes */

/**
 * @brief Implementace reakce na přijetí dat
 * @param hsai ukazatel na strukturu SAI_HandleTypeDef
 * @retval None
 * @note Tato funkce je volána, když je přijatý buffer plný
 */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai);

/**
 * @brief Implementace reakce na přijetí poloviny dat
 * @param hsai ukazatel na strukturu SAI_HandleTypeDef
 * @retval None
 * @note Tato funkce je volána, když je přijatá polovina bufferu
 */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai);

/**
 * @brief Implementace měření zvuku
 * @retval None
 */
void Meas_Audio(uint8_t mode);


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SAI_H__ */


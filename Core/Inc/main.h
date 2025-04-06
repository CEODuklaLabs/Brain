/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/**
 * @brief CUx_State_
 * @note CUx_State_ is a state machine for the CUx device
 *  reset for not connected or not initialized
 *  ready for prepared CUx.
 */
#define CUx_State_RESET 0x0FU /*!< CUx not connected or not initialized */
#define CUx_State_READY 0x10U /*!< CUx ready state */


// Příkazy pro USB komunikace
#define USB_COMMAND_RESET         0x01U    /* USB reset command                              */
#define USB_COMMAND_PING          0x02U    /* USB ping command                               */
#define USB_COMMAND_CONF          0x03U    /* USB status command                             */
#define USB_COMMAND_GET_CU_CALIB  0x04U    /* USB CU calibration command                     */
#define USB_COMMAND_SET_CU_CALIB  0x05U    /* USB CU calibration command                     */
#define USB_COMMAND_CU_STATUS     0x06U    /* USB CU status command                          */
#define USB_COMMAND_GET_MEAS      0x07U    /* USB get measurements command                   */
#define USB_COMMAND_GET_AUDIO     0x08U    /* USB get FFT data command                       */
#define USB_COMMAND_CLIMA         0x09U    /* USB get temperature command                    */
#define USB_COMMAND_SCD_CALIB     0x0AU    /* USB SCD calibration command                    */
#define USB_COMMAND_SCD_STATUS    0x0BU    /* USB SCD status command                         */
#define USB_COMMAND_GET_CU_TEMP   0x0CU    /* USB get CU temperature command                 */


//Descriptory dat do USB buffer
#define SCD_DATA                  0x30U    /*!< SCD data command                              */
#define SCD_SNO                   0x31U    /*!< SCD serial number command                     */
#define SCD_CAL_TEMP              0x32U    /*!< SCD calibration data command                  */
#define SCD_CAL_ALT               0x33U    /*!< SCD calibration data command                  */
#define SCD_LOG_TEMP              0x34U    /*!< SCD log temperature command                   */
#define SCD_LOG_ALT               0x35U    /*!< SCD log altitude command                      */
#define SCD_LOG_SNO               0x36U    /*!< SCD log serial number command                 */
//Descriptory pro CUx data do USB buffer
#define CUx_TEMP                  0x37U    /* CU data                           */
#define CUx_CAL                   0x38U    /*!< CU calibration data command                  */
//Audio data do USB buffer
#define AUDIO_DATA                0x39U    /*!< MIC data command                              */
#define AUDIO_FFT                 0x3AU    /*!< MIC FFT data command                          */

#define BEEBRAIN_STAT             0x3BU    /*!< BeeBrain status command                      */
#define END_MESSAGE               0x3FU    /*!< End of message command                        */


//LOGY pro USB buffer
#define SCD_ERR                   0x43U    /*!< SCD error command                             */
#define SCD_OK                    0x45U    /*!< SCD OK command                               */


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define NMBR_CU 10
#define NMBR_CU_TEMP 10
#define NMBR_Audio 100
#define WAV_WRITE_SAMPLE_COUNT 8192

#define STEREO 1
#define MONO 2
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/**
 * @brief Resets the device.
 */
void resetDevice(void);




/**
 * @brief Retrieves data from the specified CU (Control Unit).
 * 
 * @param CU_number The number of the Control Unit.
 * @param command The command to be sent to the Control Unit.
 * @return uint8_t The data retrieved from the Control Unit.
 */
uint8_t getCUdata(uint8_t CU_number, uint8_t command);



/**
 * @brief Selects the specified device.
 * 
 * @param deviceNumber The number of the device to be selected.
 * @param select The selection state of the device. 0 for deselect, 1 for select.
 */
void selectDeselectDevice(uint8_t deviceNumber, uint8_t select);


/**
 * @brief Callback function called when SAI (Serial Audio Interface) receive is complete.
 * 
 * @param hsai Pointer to the SAI handle.
 */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai);

/**
 * @brief Callback function called when SAI (Serial Audio Interface) receive is half complete.
 * 
 * @param hsai Pointer to the SAI handle.
 */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai);

/**
 * @brief Sends and receives an SPI command.
 *
 * This function sends a command to a specified control unit (CU) over SPI
 * and receives the response.
 *
 * @param CU_number The number of the control unit to communicate with.
 * @param command The command to send to the control unit.
 * @return 0 if the command was successful or error code if an error occurred.
 */
uint8_t sendRecieveSPICommand(uint8_t CU_number, uint8_t command);




/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TEST_Pin GPIO_PIN_0
#define TEST_GPIO_Port GPIOC
#define CS_CU1_Pin GPIO_PIN_4
#define CS_CU1_GPIO_Port GPIOA
#define CS_CU3_Pin GPIO_PIN_5
#define CS_CU3_GPIO_Port GPIOA
#define CS_CU5_Pin GPIO_PIN_6
#define CS_CU5_GPIO_Port GPIOA
#define CS_CU7_Pin GPIO_PIN_7
#define CS_CU7_GPIO_Port GPIOA
#define CS_CU9_Pin GPIO_PIN_4
#define CS_CU9_GPIO_Port GPIOC
#define CS_CU8_Pin GPIO_PIN_0
#define CS_CU8_GPIO_Port GPIOB
#define CS_CU6_Pin GPIO_PIN_1
#define CS_CU6_GPIO_Port GPIOB
#define CS_CU4_Pin GPIO_PIN_2
#define CS_CU4_GPIO_Port GPIOB
#define CS_CU2_Pin GPIO_PIN_10
#define CS_CU2_GPIO_Port GPIOB
#define CS_CU0_Pin GPIO_PIN_11
#define CS_CU0_GPIO_Port GPIOB
#define STAT_Pin GPIO_PIN_12
#define STAT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

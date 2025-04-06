/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "sai.h"
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "usbd_cdc_if.h"
#include "scd4x_i2c.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t USB_Flag;
uint8_t USB_Command;

/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];
uint16_t UserTxBufferFS_pointer;

/*Response arrays variables*/
int8_t SCD_data[16];
int8_t BRAIN_response[4];
uint8_t spi_response;

int32_t data_sai[WAV_WRITE_SAMPLE_COUNT];
int32_t data_mic_left[WAV_WRITE_SAMPLE_COUNT/2];
int32_t data_mic_right[WAV_WRITE_SAMPLE_COUNT/2];

volatile int16_t sample_sai;
arm_rfft_fast_instance_f32 fft_audio_instance;
volatile uint8_t  half_sai, full_sai;

 CUPREXIT_Command active_command;
CUPREXIT_Device *active_device;

CUPREXIT_Device CU_devices[NMBR_CU];

int8_t SPIRxBuffer[64];
int8_t SPITxBuffer[64];

uint8_t timestamp[8];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SAI1_Init();
  MX_SPI3_Init();
  MX_USB_DEVICE_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */
  int i = 0;
  CUPREXIT_InitDevice(&CU_devices[0], &hspi3, i++, CS_CU0_Pin, CS_CU0_GPIO_Port);
  CUPREXIT_InitDevice(&CU_devices[1], &hspi3, i++, CS_CU1_Pin, CS_CU1_GPIO_Port);
  CUPREXIT_InitDevice(&CU_devices[2], &hspi3, i++, CS_CU2_Pin, CS_CU2_GPIO_Port);
  CUPREXIT_InitDevice(&CU_devices[3], &hspi3, i++, CS_CU3_Pin, CS_CU3_GPIO_Port);
  CUPREXIT_InitDevice(&CU_devices[4], &hspi3, i++, CS_CU4_Pin, CS_CU4_GPIO_Port);
  CUPREXIT_InitDevice(&CU_devices[5], &hspi3, i++, CS_CU5_Pin, CS_CU5_GPIO_Port);
  CUPREXIT_InitDevice(&CU_devices[6], &hspi3, i++, CS_CU6_Pin, CS_CU6_GPIO_Port);
  CUPREXIT_InitDevice(&CU_devices[7], &hspi3, i++, CS_CU7_Pin, CS_CU7_GPIO_Port);
  CUPREXIT_InitDevice(&CU_devices[8], &hspi3, i++, CS_CU8_Pin, CS_CU8_GPIO_Port);
  CUPREXIT_InitDevice(&CU_devices[9], &hspi3, i++, CS_CU9_Pin, CS_CU9_GPIO_Port);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (USB_Flag == 1){
      	USB_Flag = 0;
        USB_Command = UserRxBufferFS[0];
        uint8_t* USB_DATA = &UserRxBufferFS[1];
        uint8_t tmp_off[4], alt[2];
        uint8_t data[10];
        uint8_t scd_serial[6];
        addSignatureUSBBuffer();      
      	switch(USB_Command){
          case USB_COMMAND_RESET:
            sendUSBMasssage();
            resetDevice();
            break;  
      	  case USB_COMMAND_PING:
            addDataToUSBBuffer((uint8_t*)USB_COMMAND_PING, 1, 0);
            break;
          case USB_COMMAND_GET_AUDIO:
            addDataToUSBBuffer((uint8_t*)USB_COMMAND_GET_AUDIO, 1, 0);
            Meas_Audio(USB_DATA[0]);
            continue;
            break;
          case USB_COMMAND_CLIMA:
            addDataToUSBBuffer((uint8_t*)USB_COMMAND_CLIMA, 1,0);
            addDataToUSBBuffer(handleSCD4xGetting(data, 4, scd4x_read_measurement_block, SCD_OK, SCD_ERR), 2, 0);
            addDataToUSBBuffer(data, sizeof(data), SCD_DATA);
            break;   
          case USB_COMMAND_CONF:
            scd4x_wake_up();
            addDataToUSBBuffer(handleSCD4xGetting(scd_serial, 4, scd4x_get_serial_number_block, SCD_OK, SCD_ERR), 2, SCD_LOG_SNO);
            addDataToUSBBuffer(scd_serial, sizeof(scd_serial), SCD_SNO);
            addDataToUSBBuffer(handleSCD4xGetting(tmp_off, 4, scd4x_get_temperature_offset_int8, SCD_OK, SCD_ERR), 4, SCD_LOG_TEMP);
            addDataToUSBBuffer(tmp_off, sizeof(tmp_off), SCD_CAL_TEMP);
            addDataToUSBBuffer(handleSCD4xGetting(alt, 4, scd4x_get_sensor_altitude_int8, SCD_OK, SCD_ERR), 2, SCD_LOG_ALT);
            addDataToUSBBuffer(alt, sizeof(alt), SCD_CAL_ALT);
            scd4x_power_down();
            addDataToUSBBuffer(NULL, 1, CUx_TEMP);
            for (int i = 0; i < NMBR_CU; i++) {
            	handleCUPCommand(CUPREXIT_COMMAND_GET_CALIB, CU_devices);
              addDataToUSBBuffer((uint8_t*)&CU_devices[i].User_ID, sizeof(CU_devices[i].User_ID), 0);
              addDataToUSBBuffer((uint8_t*)(CU_devices[i].Device_UID), sizeof(CU_devices[i].Device_UID), 0);
            }	
            break;

          case USB_COMMAND_GET_CU_CALIB:
            handleCUPCommand(CUPREXIT_COMMAND_GET_CALIB, CU_devices);
            break;
          case USB_COMMAND_SET_CU_CALIB:
            handleCUPCommand(CUPREXIT_COMMAND_SET_CALIB, CU_devices);
            break;
          case USB_COMMAND_GET_CU_TEMP:
            handleCUPCommand(CUPREXIT_COMMAND_MEAS, CU_devices);
          
        
            
            break;

          case USB_COMMAND_SCD_STATUS:
            addDataToUSBBuffer((uint8_t *) USB_COMMAND_SCD_STATUS, 1, 0);
            scd4x_wake_up();
            addDataToUSBBuffer(handleSCD4xGetting(tmp_off, 4, scd4x_get_temperature_offset_int8, SCD_OK, SCD_ERR), 4, SCD_LOG_TEMP);
            addDataToUSBBuffer(tmp_off, sizeof(tmp_off), SCD_CAL_TEMP);
            addDataToUSBBuffer(handleSCD4xGetting(alt, 4, scd4x_get_sensor_altitude_int8, SCD_OK, SCD_ERR), 2, SCD_LOG_ALT);
            addDataToUSBBuffer(alt, sizeof(alt), SCD_CAL_ALT);
            scd4x_power_down();
            break;
          case USB_COMMAND_SCD_CALIB:
            addDataToUSBBuffer((uint8_t*)USB_COMMAND_SCD_CALIB, 1,0);
            scd4x_wake_up(  );
            addDataToUSBBuffer(handleSCD4xSetting(&USB_DATA[0], 4, scd4x_set_temperature_offset_int8, SCD_OK, SCD_ERR), 2, 0);
            addDataToUSBBuffer(handleSCD4xSetting(&USB_DATA[4], 4, scd4x_set_sensor_altitude_int8, SCD_OK, SCD_ERR), 2, 0);
            scd4x_power_down();
            break;
          default:
            break;    		
      }
      addDataToUSBBuffer((uint8_t*)END_MESSAGE, 1, 0);
      sendUSBMasssage();
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  	  
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI1|RCC_PERIPHCLK_USB;
  PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK|RCC_PLLSAI1_48M2CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */



void processReceivedData(uint8_t *data) {
    int8_t size=2;
    switch (data[0]) {
      case USB_COMMAND_PING:
          size++;
          break;
      case USB_COMMAND_GET_CU_CALIB: /*recieve actual calib data*/
      case USB_COMMAND_SET_CU_CALIB: /*recieve old calib data*/
      case USB_COMMAND_GET_CU_TEMP: /*recieve actual temp data*/
          size = size + NMBR_CU_TEMP*4;
          break;
      case USB_COMMAND_CU_STATUS:
          size = 64;
    }
    // Přidání dat do bufferu
    addDataToUSBBuffer(data, size, CUx_TEMP);
}





void resetDevice() {
  /*todo reset cuprexits*/
  HAL_NVIC_SystemReset();
}





/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

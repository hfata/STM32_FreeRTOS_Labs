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
#include "cmsis_os.h"
#include "task.h"
#include "semphr.h"

#define DELAY_MS  1000

void SystemClock_Config(void);
//static void MX_GPIO_Init(void);
void Task01(void const * argument);
void Task02(void const * argument);
extern void vSetVarulMaxPRIGROUPValue();
void LED_Init(void);

osThreadId myTask01Handle;
osThreadId myTask02Handle;

// taskLoopCounter created for count the number of times the task loop is executed. Also for breakpoint the code.
static uint32_t taskLoopCounter = 0;

// Binary semaphore tanımı
SemaphoreHandle_t xLedSemaphore;

void funcLedGreenToggle(void *pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;) {
        if (xSemaphoreTake(xLedSemaphore, portMAX_DELAY) == pdTRUE) {
            HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);  // LED1 (Yeşil LED) toggle
            while (xLastWakeTime + DELAY_MS < xTaskGetTickCount())
            {
                ; // loop for delay
            }
            xLastWakeTime = xTaskGetTickCount();
            xSemaphoreGive(xLedSemaphore);
        }
    }
}

void funcLedRedToggle(void *pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;) {
        if (xSemaphoreTake(xLedSemaphore, portMAX_DELAY) == pdTRUE) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);  // LED2 (Turuncu LED) toggle
            while ((xLastWakeTime + DELAY_MS) < xTaskGetTickCount())
            {
                ; // loop for delay
            }
            xLastWakeTime = xTaskGetTickCount();
            xSemaphoreGive(xLedSemaphore);
        }
    }
}
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /*	System Init */
  HAL_Init();
  SystemClock_Config();
  //MX_GPIO_Init();
  LED_Init();
  /*	SysView Init	*/
  traceSTART();
  vSetVarulMaxPRIGROUPValue();
  SEGGER_SYSVIEW_Start();

  /* Binary semaphore oluşturma */
  xLedSemaphore = xSemaphoreCreateBinary();
  if (xLedSemaphore == NULL) {
      // Semaphore oluşturulamadı, hata işleme
      Error_Handler();
  }

  /* Led Toggle Task Init */
  xTaskCreate(funcLedRedToggle, "LED1 Task", configMINIMAL_STACK_SIZE, NULL, osPriorityNormal, NULL);
  xTaskCreate(funcLedGreenToggle, "LED2 Task", configMINIMAL_STACK_SIZE, NULL, osPriorityNormal, NULL);

  /* Semaphore'u başlangıçta serbest bırak */
  xSemaphoreGive(xLedSemaphore);

  /*	osKernelStart	*/
  osKernelStart();

  while (1)
  {
	  ;
  }
}

void LED_Init(void) {
    // GPIO Port I ve B için saat sinyallerini açıyoruz
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // LED1 (Yeşil LED) için GPIO yapılandırması (PI1)
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);  // PI1 pini çıkış olarak ayarlanıyor

    // LED2 (Turuncu LED) için GPIO yapılandırması (PB7)
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  // PB7 pini çıkış olarak ayarlanıyor
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 210;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
}

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

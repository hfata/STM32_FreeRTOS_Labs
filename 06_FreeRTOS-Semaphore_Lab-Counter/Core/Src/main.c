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

#define DELAY_FOR_TASK01    (uint32_t)500  // 500 milli
#define DELAY_FOR_TASK02    (uint32_t)550  // 550 milli
#define DELAY_FOR_TASK03    (uint32_t)600  // 600 milli

void SystemClock_Config(void);
void genericTask(void * argument);
void sharedFunc(void);
static void MX_GPIO_Init(void);
extern void vSetVarulMaxPRIGROUPValue();

SemaphoreHandle_t ledSemaphore;

typedef enum {
	SEMAPHORE_TAKEN,
	SEMAPHORE_NOT_TAKEN
} te_semaphoreStatus;

typedef struct {
    te_semaphoreStatus semaphoreStatus;
	uint32_t delay;
	TickType_t xLastWakeTime;
} TaskParams_t;

TaskParams_t taskParams[3] = {
		{SEMAPHORE_NOT_TAKEN, DELAY_FOR_TASK01, 0},
		{SEMAPHORE_NOT_TAKEN, DELAY_FOR_TASK02, 0},
		{SEMAPHORE_NOT_TAKEN, DELAY_FOR_TASK03, 0},
};
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* System Init */
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* SysView Init */
  traceSTART();
  vSetVarulMaxPRIGROUPValue();
  SEGGER_SYSVIEW_Start();

  /*	Semaphore Init	*/
  ledSemaphore = xSemaphoreCreateCounting(1, 1);
  if (ledSemaphore == NULL)
  {
    Error_Handler(); // Handle error
  }

  /* Task01 Init */
  xTaskCreate(genericTask, "Task 01", 128, (void *)&taskParams[0], 1, NULL);

  /* Task02 Init */
  xTaskCreate(genericTask, "Task 02", 128, (void *)&taskParams[1], 1, NULL);

  /* Task03 Init */
  xTaskCreate(genericTask, "Task 03", 128, (void *)&taskParams[2], 1, NULL);

  /* osKernelStart */
  osKernelStart();

  while (1)
  {
    ;
  }
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PI11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/**
  * @brief  Function implementing the genericTask thread.
  * @param  argument: Delay time for the task
  * @retval None
  */
void genericTask(void *argument) {
    TaskParams_t *params = (TaskParams_t *)argument;
    params->xLastWakeTime = xTaskGetTickCount();  // Get the initial timestamp
    for (;;) {
        // If the semaphore has not been taken, try to take it
        if (xSemaphoreTake(ledSemaphore, portMAX_DELAY) == pdTRUE) {
            // Semaphore is taken, performing the task
            params->semaphoreStatus = SEMAPHORE_TAKEN;
            // Turn on the LED
            HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_11);
            // Wait for the specified time (100 ms or 200 ms)
            vTaskDelayUntil(&params->xLastWakeTime, pdMS_TO_TICKS(params->delay));
            // Turn off the LED
            HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_11);
            // Release the semaphore
            xSemaphoreGive(ledSemaphore);
            // Set the semaphore status back to NOT_TAKEN
            params->semaphoreStatus = SEMAPHORE_NOT_TAKEN;
        }
        // Delay the task periodically
        // Also you can try to use "vTaskDelay" function instead of vTaskDelayUntil function
        vTaskDelayUntil(&params->xLastWakeTime, pdMS_TO_TICKS(params->delay));
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

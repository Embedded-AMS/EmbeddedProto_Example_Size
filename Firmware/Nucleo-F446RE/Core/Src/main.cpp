/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <cstdint>
#include "core_cm4.h"  // For CoreDebug and DWT register definitions

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
// Linker symbols for memory boundaries (declared as arrays to avoid size warnings)
extern "C" {
    extern uint32_t _end[];          // End of BSS section, start of free memory
    extern uint32_t _estack[];       // Top of stack (end of RAM)
    extern uint32_t _watermark_start[]; // Start of watermark section
    extern uint32_t _watermark_end[];   // End of watermark section
    extern uint32_t _heap_watermark_start[]; // Start of heap watermark section
    extern uint32_t _heap_watermark_end[];   // End of heap watermark section
}

// Global variable to store stack usage result (accessible via SWD)
volatile uint32_t g_stack_usage = 0;

// Global variable to store heap usage result (accessible via SWD)
volatile uint32_t g_heap_usage = 0;

// Global variable to store CPU cycle count for run_scenario() (accessible via SWD)
volatile uint32_t g_cpu_cycles_used = 0;

// Global variable to store the number of bytes used in the serialization buffer. (accessible via SWD)
volatile uint32_t g_n_bytes_in_buffer = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
// Function prototypes for memory measurement
uint32_t measure_memory_usage();
uint32_t measure_heap_usage();

// Function prototype for DWT cycle counter
void enable_dwt_cycle_counter();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Forward declaration for scenario function
uint32_t run_scenario();

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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
/* USER CODE BEGIN 2 */
  // Enable DWT cycle counter for CPU usage measurement
  enable_dwt_cycle_counter();

  // Memory watermark is initialized in startup code (startup_stm32f446retx.s)

/* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // Reset cycle counter and capture start value
    DWT->CYCCNT = 0;

    // Run the selected scenario
    g_n_bytes_in_buffer = run_scenario();

    // Capture cycle count after scenario execution
    g_cpu_cycles_used = DWT->CYCCNT;

    // Measure stack usage after scenario and store result
    g_stack_usage = measure_memory_usage();

    // Measure heap usage after scenario and store result
    g_heap_usage = measure_heap_usage();

    // Infinite loop
    while(true)
    {
      asm("nop");
    }
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief Measure how much of the free memory was used (stack high water mark)
  *        Scans from end of watermark section downward to find first overwritten location
  * @retval uint32_t: Total peak stack usage from _estack (accurate measurement)
  */
uint32_t measure_memory_usage()
{
    // Get watermark boundaries from linker symbols
    uint32_t* start = _watermark_start;
    uint32_t* end = _watermark_end;

    // Watermark pattern is 0xA5A5A5A5 as defined in linker script
    const uint32_t WATERMARK_PATTERN = 0xA5A5A5A5;

    // Scan from end of watermark region downward to find first overwritten location
    for(uint32_t* ptr = end - 1; ptr >= start; ptr--)
    {
        if(*ptr != WATERMARK_PATTERN)
        {
            // Found overwritten memory, calculate total stack usage from _estack
            return (uint32_t)_estack - (uint32_t)ptr;
        }
    }

    // No overwritten memory found - stack usage is minimal
    return (uint32_t)_estack - (uint32_t)start;
}

/**
  * @brief Measure heap usage by scanning heap watermark section
  *        Scans from start of heap watermark section upward to find first overwritten location
  * @retval uint32_t: Total peak heap usage from heap start
  */
uint32_t measure_heap_usage()
{
    // Get heap watermark boundaries from linker symbols
    uint32_t* start = _heap_watermark_start;
    uint32_t* end = _heap_watermark_end;

    // Heap watermark pattern is 0x5A5A5A5A (different from stack to avoid confusion)
    const uint32_t HEAP_WATERMARK_PATTERN = 0x5A5A5A5A;

    // Scan from start of heap watermark region upward to find first overwritten location
    for(uint32_t* ptr = start; ptr < end; ptr++)
    {
        if(*ptr != HEAP_WATERMARK_PATTERN)
        {
            // Found overwritten memory, calculate total heap usage from start of heap
            return (uint32_t)ptr - (uint32_t)start;
        }
    }

    // No overwritten memory found - heap usage is minimal
    return 0;
}

/**
  * @brief Enable DWT cycle counter for CPU cycle measurement
  *        Must be called once during initialization
  * @retval None
  */
void enable_dwt_cycle_counter()
{
    // Enable trace and debug block (required for DWT)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // Reset the cycle counter
    DWT->CYCCNT = 0;
    // Enable the cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
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

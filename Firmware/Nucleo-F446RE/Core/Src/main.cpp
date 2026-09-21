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
#include <cstddef>
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
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
// Global variable to store CPU cycle count for run_scenario() (accessible via SWD)
volatile uint32_t g_cpu_cycles_used = 0;

// Global variable to store the number of bytes used in the serialization buffer. (accessible via SWD)
volatile uint32_t g_n_bytes_in_buffer = 0;
// Global variable to store the RAM taken by the scenario's message object. (accessible via SWD)
volatile uint32_t g_message_size_bytes = 0;
// Global variable to store the RAM the scenario took at its worst moment: the stack high-water
// mark below main() plus the heap it made newlib grow. (accessible via SWD)
volatile uint32_t g_peak_ram_bytes = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
// Function prototype for DWT cycle counter
void enable_dwt_cycle_counter();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Forward declaration for scenario function
uint32_t run_scenario();

// The newlib heap, see sysmem.c. With an increment of zero it tells where the heap ends.
extern "C" void* _sbrk(ptrdiff_t increment);

// The free RAM between the heap and the stack is painted with this value before a scenario
// runs. Whatever the scenario touches loses the pattern, which tells how deep it went.
static constexpr uint32_t RAM_PAINT = 0xC5C5C5C5U;
// Bytes left unpainted right under the caller's stack pointer: room for the painter's own frame.
static constexpr uint32_t PAINT_MARGIN = 64U;

//! Paint the free RAM from the heap end up to the margin below the given stack pointer.
static void __attribute__((noinline)) paint_free_ram(const uint32_t stack_pointer)
{
  const uint32_t heap_end = reinterpret_cast<uint32_t>(_sbrk(0));
  uint32_t* word = reinterpret_cast<uint32_t*>((heap_end + 3U) & ~3U);
  uint32_t* const top = reinterpret_cast<uint32_t*>(stack_pointer - PAINT_MARGIN);
  while(word < top)
  {
    *word = RAM_PAINT;
    ++word;
  }
}

//! Peak RAM: how far below the stack pointer the pattern is gone, plus how far the heap grew.
static uint32_t __attribute__((noinline)) measure_peak_ram(const uint32_t stack_pointer,
                                                            const uint32_t heap_before)
{
  const uint32_t heap_after = reinterpret_cast<uint32_t>(_sbrk(0));
  const uint32_t* word = reinterpret_cast<const uint32_t*>((heap_after + 3U) & ~3U);
  const uint32_t* const top = reinterpret_cast<const uint32_t*>(stack_pointer - PAINT_MARGIN);
  while((word < top) && (RAM_PAINT == *word))
  {
    ++word;
  }
  const uint32_t stack_bytes = (word < top) ? (stack_pointer - reinterpret_cast<uint32_t>(word)) : 0U;
  return stack_bytes + (heap_after - heap_before);
}

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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  // Blink LD2 from TIM2 in hardware while the board is in use. No code runs for it, so the
  // cycle count of the scenario is unaffected. The tool stops it with a reset after the read.
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

  // Enable DWT cycle counter for CPU usage measurement
  enable_dwt_cycle_counter();

/* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // No SysTick interrupt while measuring: its frames would show up in the stack high-water
    // mark and its cycles in the count.
    HAL_SuspendTick();
    const uint32_t stack_pointer = __get_MSP();
    const uint32_t heap_before = reinterpret_cast<uint32_t>(_sbrk(0));
    paint_free_ram(stack_pointer);

    // Reset cycle counter and capture start value
    DWT->CYCCNT = 0;

    // Run the selected scenario
    g_n_bytes_in_buffer = run_scenario();

    // Capture cycle count after scenario execution
    g_cpu_cycles_used = DWT->CYCCNT;

    g_peak_ram_bytes = measure_peak_ram(stack_pointer, heap_before);
    HAL_ResumeTick();

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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */
  // TIM2 runs from the APB1 timer clock, 84 MHz (APB1 at 42 MHz, doubled because the APB1
  // prescaler is 2). Prescaler 84 gives a 1 MHz count, period 2 s, LD2 on for 100 ms.
  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 84-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2000000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 100000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

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

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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

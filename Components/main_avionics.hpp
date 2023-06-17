/**
 ******************************************************************************
 * File Name          : Main.hpp
 * Description        : Header file for Main.cpp, acts as an interface between
 *  STM32CubeIDE and our application.
 ******************************************************************************
*/
#ifndef AVIONICS_INCLUDE_SOAR_MAIN_H
#define AVIONICS_INCLUDE_SOAR_MAIN_H
#include "Mutex.hpp"
#include "stm32f4xx_hal.h"

/* Interface Functions ------------------------------------------------------------------*/
/* These functions act as our program's 'main' and any functions inside CubeIDE's main --*/
void run_main();
void run_StartDefaultTask();

/* Global Functions ------------------------------------------------------------------*/
void print(const char* format, ...);
void soar_assert_debug(bool condition, const char* file, uint16_t line, const char* str = nullptr, ...);

/* Global Variable Interfaces ------------------------------------------------------------------*/
/* All must be extern from main_avionics.cpp -------------------------------------------------*/
namespace Global
{
	extern Mutex vaListMutex;
}


/* System Handles ------------------------------------------------------------------*/
/* This should be the only place externs are allowed -------------------------------*/
//UART Handles
extern UART_HandleTypeDef huart1;   // UART1 - RS485 - HALF DUPLEX FOR NOW!! SCHEMATIC WRONG NAME
extern UART_HandleTypeDef huart5;   // UART5 - Debug


//I2C Handles
extern I2C_HandleTypeDef hi2c1;      // I2C1 -- Infrared Temperature Sensor

//CRC Handles
extern CRC_HandleTypeDef hcrc;       // CRC - Hardware CRC System Handle

//DMA Handles
extern DMA_HandleTypeDef hdma_uart5_rx; // DMA UART 5 RX -
extern DMA_HandleTypeDef hdma_uart5_tx; // DMA UART 5 TX -

namespace SystemHandles {
	// Aliases
	constexpr UART_HandleTypeDef* UART_Protocol = &huart1;
	constexpr UART_HandleTypeDef* UART_Debug = &huart5;

	constexpr I2C_HandleTypeDef* I2C_IR = &hi2c1;
	constexpr CRC_HandleTypeDef* CRC_Handle = &hcrc;

	// DMA Aliases
}

#endif /* AVIONICS_INCLUDE_SOAR_MAIN_H */

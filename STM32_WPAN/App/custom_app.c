/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    custom_app.c
  * @author  MCD Application Team
  * @brief   Custom Example Application (Server)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
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
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "custom_app.h"
#include "custom_stm.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  /* P2P_Server */
  uint8_t               Switch_c_Notification_Status;
  /* USER CODE BEGIN CUSTOM_APP_Context_t */
  uint8_t               SW1_Status;
  /* USER CODE END CUSTOM_APP_Context_t */

  uint16_t              ConnectionHandle;
} Custom_App_Context_t;

/* USER CODE BEGIN PTD */
typedef struct
{
  uint16_t              index;
  uint8_t               received_write[509];
} Write_ExtValue_t;
/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TOGGLE_ON                       1
#define TOGGLE_OFF                      0
/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

static Custom_App_Context_t Custom_App_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */

uint8_t UpdateCharData[247];
uint8_t NotifyCharData[247];

/* USER CODE BEGIN PV */
Write_ExtValue_t write_ext;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* P2P_Server */
static void Custom_Switch_c_Update_Char(void);
static void Custom_Switch_c_Send_Notification(void);

/* USER CODE BEGIN PFP */
static void Custom_Switch_c_Send_Notification_Ext(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void Custom_STM_App_Notification(Custom_STM_App_Notification_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_1 */
  
  /* USER CODE END CUSTOM_STM_App_Notification_1 */
  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* USER CODE END CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* P2P_Server */
    case CUSTOM_STM_LED_C_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_LED_C_READ_EVT */
       PRINT_MESG_DBG("ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE My_Led_Char Read\n");

      /* USER CODE END CUSTOM_STM_LED_C_READ_EVT */
      break;

    case CUSTOM_STM_LED_C_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN CUSTOM_STM_LED_C_WRITE_NO_RESP_EVT */
      if (pNotification->DataTransfered.pPayload[1] == 0x01 || pNotification->DataTransfered.pPayload[1] == 0x00) /* Write */
      {
        APP_DBG_MSG("-- P2P APPLICATION SERVER : WRITE RECEIVED\r\n");
        APP_DBG_MSG("** Write Data: 0x%02X %02X\r\n\r\n", pNotification->DataTransfered.pPayload[0], pNotification->DataTransfered.pPayload[1]);
      
        if (pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          HAL_GPIO_WritePin(Blue_Led_GPIO_Port, Blue_Led_Pin, GPIO_PIN_SET); 
        }
        else if (pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          HAL_GPIO_WritePin(Blue_Led_GPIO_Port, Blue_Led_Pin, GPIO_PIN_RESET); 
        }
      }
      else /* Extended Write */
      {
        APP_DBG_MSG("-- P2P APPLICATION SERVER : EXTENDED WRITE RECEIVED\r\n");
        APP_DBG_MSG("** Extended Write Data (Length: %d | Offset: %x):", pNotification->DataTransfered.Length, pNotification->Offset);
        APP_DBG_MSG("\r\n0x");
        for (uint16_t i = 0; i < pNotification->DataTransfered.Length; i++)
        {
          APP_DBG_MSG("%02X ", pNotification->DataTransfered.pPayload[i]);
          write_ext.received_write[write_ext.index + i] = pNotification->DataTransfered.pPayload[i];
        }
        APP_DBG_MSG("\r\n\r\n");
        if(pNotification->Offset & 0x8000)
        {
          write_ext.index = write_ext.index + pNotification->DataTransfered.Length;
        }
        else
        {
          write_ext.index = 0;
        }
        
        // Delay to Give Time for Log Message to Print (10 ms)
        HAL_Delay(10);
      }
      /* USER CODE END CUSTOM_STM_LED_C_WRITE_NO_RESP_EVT */
      break;

    case CUSTOM_STM_SWITCH_C_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_SWITCH_C_NOTIFY_ENABLED_EVT */
      APP_DBG_MSG("\r\n\r** CUSTOM_STM_BUTTON_C_NOTIFY_ENABLED_EVT \n");

      Custom_App_Context.Switch_c_Notification_Status = TOGGLE_ON;        /* My_Switch_Char notification status has been enabled */
      /* USER CODE END CUSTOM_STM_SWITCH_C_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_SWITCH_C_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_SWITCH_C_NOTIFY_DISABLED_EVT */
      APP_DBG_MSG("\r\n\r** CUSTOM_STM_BUTTON_C_NOTIFY_DISABLED_EVT \n");

      Custom_App_Context.Switch_c_Notification_Status = TOGGLE_OFF;        /* My_Switch_Char notification status has been disabled */
      /* USER CODE END CUSTOM_STM_SWITCH_C_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_NOTIFICATION_COMPLETE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */

      /* USER CODE END CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_STM_App_Notification_default */

      /* USER CODE END CUSTOM_STM_App_Notification_default */
      break;
  }
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_2 */

  /* USER CODE END CUSTOM_STM_App_Notification_2 */
  return;
}

void Custom_APP_Notification(Custom_App_ConnHandle_Not_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_APP_Notification_1 */

  /* USER CODE END CUSTOM_APP_Notification_1 */

  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_APP_Notification_Custom_Evt_Opcode */

    /* USER CODE END P2PS_CUSTOM_Notification_Custom_Evt_Opcode */
    case CUSTOM_CONN_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_CONN_HANDLE_EVT */
          
      /* USER CODE END CUSTOM_CONN_HANDLE_EVT */
      break;

    case CUSTOM_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_DISCON_HANDLE_EVT */
      
      /* USER CODE END CUSTOM_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_APP_Notification_default */

      /* USER CODE END CUSTOM_APP_Notification_default */
      break;
  }

  /* USER CODE BEGIN CUSTOM_APP_Notification_2 */

  /* USER CODE END CUSTOM_APP_Notification_2 */

  return;
}

void Custom_APP_Init(void)
{
  /* USER CODE BEGIN CUSTOM_APP_Init */  
  Custom_Switch_c_Update_Char();

  UTIL_SEQ_RegTask(1<< CFG_TASK_SW1_BUTTON_PUSHED_ID, UTIL_SEQ_RFU, Custom_Switch_c_Send_Notification);
  UTIL_SEQ_RegTask(1<< CFG_TASK_SW2_BUTTON_PUSHED_ID, UTIL_SEQ_RFU, Custom_Switch_c_Send_Notification_Ext);

  Custom_App_Context.Switch_c_Notification_Status = TOGGLE_OFF;   
  Custom_App_Context.SW1_Status = 0;                 
  /* USER CODE END CUSTOM_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* P2P_Server */
void Custom_Switch_c_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Switch_c_UC_1*/

  /* USER CODE END Switch_c_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_SWITCH_C, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Switch_c_UC_Last*/

  /* USER CODE END Switch_c_UC_Last*/
  return;
}

void Custom_Switch_c_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Switch_c_NS_1*/
  if (Custom_App_Context.Switch_c_Notification_Status == TOGGLE_ON)
  {
    updateflag = 1;
    
    if (Custom_App_Context.SW1_Status == 0)
    {
      Custom_App_Context.SW1_Status = 1;
      NotifyCharData[0] = 0x01;
      NotifyCharData[1] = 0x01;
    }
    else
    {
      Custom_App_Context.SW1_Status = 0;
      NotifyCharData[0] = 0x01;
      NotifyCharData[1] = 0x00;
    }

    APP_DBG_MSG("-- P2P APPLICATION SERVER : BUTTON 1 PUSHED - NOTIFICATION TO CLIENT\n");
  }
  else
  {
    APP_DBG_MSG("-- P2P APPLICATION SERVER : BUTTON 1 PUSHED - NOTIFICATION DISABLED\n");
  }     
  /* USER CODE END Switch_c_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_SWITCH_C, (uint8_t *)NotifyCharData);
  }

  /* USER CODE BEGIN Switch_c_NS_Last*/

  /* USER CODE END Switch_c_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
void Custom_Switch_c_Send_Notification_Ext(void)
{  
  uint8_t payload[509];
  uint16_t length = 509;
  
  if(Custom_App_Context.Switch_c_Notification_Status){ 
    APP_DBG_MSG("-- P2P APPLICATION SERVER : BUTTON 2 PUSHED - EXTENDED NOTIFICATION TO CLIENT\n");

    for (uint16_t i = 0; i < length; i++)
    {
      if (i < 255)
      {
        payload[i] = i;
      }
      else
      {
        payload[i] = 0xA;
      }
    }

    Custom_STM_App_Update_Char_Ext(CUSTOM_STM_SWITCH_C, Custom_App_Context.ConnectionHandle, length, (uint8_t*)&payload);
  } else {
    APP_DBG_MSG("-- P2P APPLICATION SERVER : BUTTON 2 PUSHED - NOTIFICATION DISABLED\n"); 
  }

  return;
}

void SW1_Button_Action(void)
{
  UTIL_SEQ_SetTask(1<<CFG_TASK_SW1_BUTTON_PUSHED_ID, CFG_SCH_PRIO_0);

  return;
}

void SW2_Button_Action(void)
{
  UTIL_SEQ_SetTask(1<<CFG_TASK_SW2_BUTTON_PUSHED_ID, CFG_SCH_PRIO_0);

  return;
}
/* USER CODE END FD_LOCAL_FUNCTIONS*/

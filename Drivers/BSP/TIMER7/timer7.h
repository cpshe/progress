#ifndef __BTIM_H
#define __BTIM_H

#include "./SYSTEM/sys/sys.h"
extern TIM_HandleTypeDef htim7;  /* ��ʱ����� */

/******************************************************************************************/
/* ������ʱ�� ���� */

/* TIMX �ж϶��� 
 * Ĭ�������TIM6/TIM7
 * ע��: ͨ���޸���4���궨��,����֧��TIM1~TIM8����һ����ʱ��.
 */
 
#define TIM7_INT                       TIM7
#define TIM7_INT_IRQn                  TIM7_IRQn
#define TIM7_INT_IRQHandler            TIM7_IRQHandler
#define TIM7_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM7_CLK_ENABLE(); }while(0)   /* TIM7 ʱ��ʹ�� */


/******************************************************************************************/

void tim7_int_init(uint16_t arr, uint16_t psc);    /* ������ʱ�� ��ʱ�жϳ�ʼ������ */
void Error_Handler(void);

#endif

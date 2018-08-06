/**
 * Author:   SH<sh_def@163.com>
 * Date:     2018-08-05
 * Describe: stack check for SMC-RTOS Thread
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "smc_thread.h"
#include "smc_core.h"
#include "smc_timer.h"
#include <stdio.h>
#include <memory.h>


#define STACK_PADDING_DATA 0xA5
#define STACK_CHECK_MODE SMC_STACK_CALC

#define SMC_STACK_CHECK

#ifdef STACK_CHECK_MODE
#ifndef SMC_STACK_CHECK
	#error "you have defined STACK_CHECK_MODE but not define SMC_STACK_CHECK"
#endif
#endif

enum smc_stack_check_flag_e {
	SMC_STACK_CALC,
	SMC_STACK_CHECK_POINTER,
	SMC_STACK_CHECK_DATA,
};

enum smc_stack_check_error_e {
	SMC_STACK_CHEACK_OK,
	SMC_STACK_CHECK_OVERFLOW,
};

#ifdef SMC_STACK_CHECK
smc_stack_t *smc_stack_check_init(void)
{
	smc_stack_t *ret;
	switch(STACK_CHECK_MODE){
	case SMC_STACK_CALC:
		memset(smc_thread_current->stack_top, STACK_PADDING_DATA, stack_size);
		ret = smc_thread_current->stack_top + smc_thread_current->stack_size;
		break;
	case SMC_STACK_CHECK_DATA:
		memset(stack_top + sizeof(smc_int32_t), STACK_PADDING_DATA, sizeof(smc_int32_t));
		ret = smc_thread_current->stack_top + sizeof(smc_int32_t);
		break;
	case SMC_STACK_CHECK_POINTER:

	default:
		break;
	}
	return ret;
}


smc_stack_t *smc_stack_check(void)
{
	smc_int32_t i = 0;
	char *pstack;
	switch(STACK_CHECK_MODE){
	case SMC_STACK_CALC:
		pstack = (char *)(smc_thread_current->stack_top);
		while(*pstack++ == STACK_PADDING_DATA);
		smc_thread_current->stack_usage = 100 - 100 * (pstack - (char *)(smc_thread_current->stack_top)) / smc_thread_current->stack_size;
		if(smc_thread_current->stack_usage < 99)
			break;
		else
			return -SMC_STACK_CHECK_OVERFLOW;
	case SMC_STACK_CHECK_DATA:
		pstack = (char *)(smc_thread_current->stack_top);
		for(i = 0; i < sizeof(smc_int32_t); i++){
			if(*pstack++ != STACK_PADDING_DATA){
				printf("The priority of %d stack is overflowing", smc_thread_current->priority);
				return -SMC_STACK_CHECK_OVERFLOW;
			}
		}
		break;
	case SMC_STACK_CHECK_POINTER:
		if(smc_thread_current->sp <= smc_thread_current->stack_top){
			printf("The priority of %d stack is overflowing", smc_thread_current->priority);
			return -SMC_STACK_CHECK_OVERFLOW;
		}
	default:
		break;
	}
	return SMC_STACK_CHEACK_OK;
}

#endif


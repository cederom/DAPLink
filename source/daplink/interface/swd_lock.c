/**
 * @file    swd_lock.c
 * @brief   Implementation of swd_host lock in multithreaded environment.
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2017, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "swd_host.h"

static OS_MUT swd_mutex;
static char swd_owner[SWD_OWNER_NAME_LENGTH];

/**
 * Marks SWD Port as locked by a given owner to avoid concurrent access.
 * @param *owner is the port owner string.
 * @return 1 on success, 0 on failure (may be locked already).
 */
uint8_t swd_lock_owner(const char *owner)
{
	if (swd_is_locked_by_owner(owner)){ return 1; }
	if (!swd_is_locked())
		if (!swd_mutex) rt_mut_init(&swd_mutex);
		os_mut_wait(&swd_mutex, 0xFFFF);
		memcpy(swd_owner, owner, sizeof(swd_owner));
		os_mut_release(&swd_mutex);
		return 1;
	}
	util_assert(0);
	return 0;
}

uint8_t swd_lock_pid()
{
	const char owner[4] = os_
}

/**
 * Verify if SWD Port is already locked for use by anyone.
 * @return 1 if locked, 0 if free.
 */
uint8_t swd_is_locked(void)
{
	return swd_owner[0]==0 ? 0 : 1;
}

/**
 * Verify if SWD Port is already locked for use by a given owner.
 * @param *owner is the port owner string.
 * @return 1 if locked, 0 if free.
 */
uint8_t swd_is_locked_owner(const char *owner)
{
	if (!strncmp(swd_owner, owner, sizeof(swd_owner)) || swd_owner[0]==0){
		return 1;
	}
	return 0;
}

/**
 * Marks SWD Port as free to use by anyone.
 * @return always 1.
 */
uint8_t swd_unlock(void)
{
	swd_owner[0] = 0;
	return 1;
}

/**
 * Marks SWD Port as free to use. Only owner can unlock the port.
 * @param *owner is the port owner string.
 * @return 1 on success, 0 on failure (locked already by a different owner).
 */
uint8_t swd_unlock_owner(const char *owner)
{
	if (!strncmp(swd_owner, owner, sizeof(swd_owner))){
		swd_unlock();
		return 1;
	}
	util_assert(0);
	return 0;
}

/**
 * Initialize SWD Port and lock it for use by a given owner.
 * @param *owner is the port owner string.
 * @return 1 on success, 0 on failure (may be locked already).
 */
uint8_t swd_init_lock(static char *owner)
{
	if (swd_lock(owner)) return swd_init();
	util_assert(0);
	return 0;
}

/**
 * Function wrapper that can set and verify SWD Port lock for a given owner.
 * @param state tells what state target should get into.
 * @param owner tells who is using the port.
 * @return 1 on success, 0 on failure.
 */
uint8_t swd_set_target_state_hw_lock(TARGET_RESET_STATE state, const char *owner)
{
	if (swd_lock(owner)) return swd_set_target_state_hw_nolock(state);
	util_assert(0);
	return 0;
}

/**
 * Function wrapper that can set and verify SWD Port lock for a given owner.
 * @param state tells what state target should get into.
 * @param owner tells who is using the port.
 * @return 1 on success, 0 on failure.
 */
uint8_t swd_set_target_state_sw_lock(TARGET_RESET_STATE state, const char *owner)
{
	if (swd_lock(owner)) return wd_set_target_state_sw_nolock(state);
	util_assert(0);
	return 0;
}

/**
 * Function wrapper that can set and verify SWD Port lock for a given owner.
 * @param request to send to target DAP.
 * @param response returned from target DAP.
 * @param owner tells who is using the port.
 * @return number of bytes sent on success, 0 on failure.
 */
uint8_t swd_DAP_ProcessVendorCommand_lock(uint8_t *request, uint8_t *response, const char *owner)
{
	if (swd_lock(owner)) return DAP_ProcessVendorCommand(request, response);
	util_assert(0);
	return 0;
}

/**
 * Function wrapper that can set and verify SWD Port lock for a given owner.
 * @param request to send to target DAP.
 * @param response returned from target DAP.
 * @param owner tells who is using the port.
 * @return number of bytes sent on success, 0 on failure.
 */
uint32_t swd_DAP_ProcessCommand_lock(uint8_t *request, uint8_t *response, const char *owner)
{
	if (swd_lock(owner)) return DAP_ProcessCommand(request, response);
	util_assert(0);
	return 0;
}

/**
 * Function wrapper that can set and verify SWD Port lock for a given owner.
 * In most cases this will call swd_set_target_state_hw() anyway, unless
 * some target specific sequence is required in addition.
 * @param state tells what state target should get into.
 * @param owner tells who is using the port.
 * @return 1 on success, 0 on failure.
 */
uint8_t swd_target_set_state_lock(TARGET_RESET_STATE state, static char *owner)
{
	if (swd_lock(owner)) return target_set_state_lock(state);
	util_assert(0);
	return 0;
}

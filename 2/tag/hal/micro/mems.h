/** @file mems.h
 * @brief Header for MEMS APIS 
 *
 *
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */

#ifndef _MEMS_H_
#define _MEMS_H_

#include "hal/micro/mems_regs.h"
/** @addtogroup MEMS
 *@{
 */

/** @brief Mems data type: three acceleration values each related to a specific direction
   Watch out: only lower data values (e.g. those terminated by the _l) are 
   currently used by the device */

typedef struct {
  int8u outx_l;
  int8u outx_h;
  int8u outy_l;
  int8u outy_h;
  int8u outz_l;
  int8u outz_h;
} t_mems_data;


/** @brief Get mems state
    @return MEMS status register
 */
int8u halMemsGetStatus(void);

/** @brief Get mems acceleration values
    @param mems_data Output parameters with mems axis acceleration
    @return 1 for success, 0 on failure
 */
int8u halMemsGetValue(t_mems_data *mems_data);

/** @brief Mems Initialization function
    @return 1 for success, 0 on failure
 */
int8u halMemsInit(void);

/** @brief Puts the Mems in power down
 */
void halMemsPowerDown(void);

/** @} // END addtogroup
 */


#endif /* _MEMS_H_ */


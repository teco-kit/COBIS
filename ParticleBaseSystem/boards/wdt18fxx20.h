/**
  * Copyright (c) 2004, Telecooperation Office (TecO),
  * Universitaet Karlsruhe (TH), Germany.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions
  * are met:
  *
  *   * Redistributions of source code must retain the above copyright
  *     notice, this list of conditions and the following disclaimer.
  *   * Redistributions in binary form must reproduce the above
  *     copyright notice, this list of conditions and the following
  *     disclaimer in the documentation and/or other materials provided
  *     with the distribution.
  *   * Neither the name of the Universitaet Karlsruhe (TH) nor the names
  *     of its contributors may be used to endorse or promote products
  *     derived from this software without specific prior written
  *     permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  */


/* Header-File for Software-WatchDogTimer-Driver
*
* You can use this driver to use the software-watchdog in your program giving it a rate
* Be careful: You have to disable the hardware-watchdog in the #fuses in the header-file
* of your program (#fuses nowdt)
* software watchdog is significant only if hardware watchdog is turned off!!
*
*
* author: Klaus Schmidt (schmidtk@teco.edu)
* date: 2004-08-03
* ver: 003
*/

#ifndef __WDT18FXX20_H__
#define __WDT18FXX20_H__	003

/**
* Configures the watchdog timer.
* To configure the software-watchdog specify the timeout:
* as 1, 2, 3, 4, 5, 6, 7, 8 where
* 0 = 18 ms,
* 1 = 36 ms
* 2 = 72 ms
* 3 = 144 ms
* 4 = 288 ms
* 5 = 576 ms
* 6 = 1.152 s
* 7 = 2.304 s
* @param timeout watchdog-timeout as described above
*
*/
void WDTConfig(unsigned int timeout);

/**
* Enables the software watchdog timer.
*/
void WDTEnable();

/**
* Disables the software watchdog timer.
*/
void WDTDisable();

/**
* Tests, whether software watchdog timer is enabled (returns 1) or disabled (returns 0)
*/
unsigned int WDTTest();

/**
* Reads the actual value of the software-watchdog-timer
* returns value as described in WDTConfig
*/
unsigned int WDTReadValue();

#endif
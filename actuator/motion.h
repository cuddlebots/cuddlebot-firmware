/*

Cuddlebot actuator firmware - Copyright (C) 2014 Michael Phan-Ba

Property of SPIN Research Group
ICICS/CS Building X508-2366 Main Mall
Vancouver, B.C. V6T 1Z4 Canada
(604) 822 8169 - maclean@cs.ubc.ca

*/

#ifndef _MOTION_H_
#define _MOTION_H_

#include <ch.h>
#include <hal.h>

#include "comm.h"
#include "msgtype.h"
#include "pid.h"

/* Motion driver states. */
typedef enum {
  MOTION_UNINIT = 0,
  MOTION_STOP = 1,
  MOTION_READY = 2
} motionstate_t;

/* Motion driver configuration. */
typedef struct {

  /* Setpoint buffer messaging. */
  MemoryPool *pool;
  Mailbox *mbox;

  /* Driver thread configuration. */
  void *thread_wa;                      // Thread working area.
  size_t thread_wa_size;                // Thread working area size.
  tprio_t thread_prio;                  // Thread priority.

} MotionConfig;

/* Motion driver structure. */
typedef struct {
  /* Driver state. */
  motionstate_t state;                  // driver state
  BinarySemaphore ready;                // signal from GPT

  /* Configuration. */
  MotionConfig *config;                 // motion driver configuration

  /* Setpoint buffers. */
  msgtype_setpoint_t *sp;               // current setpoints
  msgtype_setpoint_t *nextsp;           // next setpoints

  uint16_t delay;                       // delay until next setpoints
  uint16_t loop;                        // loops for current setpoints
  uint16_t duration;                    // duration for current setpoint
  size_t spindex;                       // setpoint offset
  float pos;                            // last read position

  /* Driver handles. */
  GPTDriver *gptp;                      // GPT driver
  PIDDriver pid;                        // PID driver

  /* Thread pointer. */
  Thread *thread_tp;                    // motion driver thread

} MotionDriver;

extern MotionDriver MOTION2;

/* Initialize the motion driver. */
void motionInit(void);

/*

Initialize a motion driver object.

@param mdp The motion driver

*/
void motionObjectInit(MotionDriver *mdp);

/*

Start the motion driver.

@param mdp The motion driver
@param mdcfg The motion driver configuration

*/
void motionStart(MotionDriver *mdp, MotionConfig *mdcfg);

/*

Stop the motion driver.

@param mdp The motion driver

*/
void motionStop(MotionDriver *mdp);

/*

Set PID coefficients.

@param mdp The motion driver
@param coeff The PID coefficients

*/
void motionSetCoeff(MotionDriver *mdp, const PIDConfig *coeff);

/*

Queue next setpoint buffer.

@param mdp The motion driver
@param setpoint The setpoint buffer
@return RDY_OK if ok or RDY_TIMEOUT if full

*/
msg_t motionSetpoint(MotionDriver *mdp, msgtype_setpoint_t *setpoint);

/*

Get most recent motor position.

@param mdp The motion driver
@return The position of the sensor in radians

*/
float motionGetPosition(MotionDriver *mdp);

#endif // _MOTION_H_

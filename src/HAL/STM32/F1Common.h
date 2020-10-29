// Common macros and functions for STM32 F103 and F303 microcontrollers

// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define a1STEP_H WRITE_REG(Axis1_StpPORT->BSRR, Axis1_StpBIT)
#define a1STEP_L WRITE_REG(Axis1_StpPORT->BRR,  Axis1_StpBIT)
#define a1DIR_H  WRITE_REG(Axis1_DirPORT->BSRR, Axis1_DirBIT)
#define a1DIR_L  WRITE_REG(Axis1_DirPORT->BRR,  Axis1_DirBIT)

#define a2STEP_H WRITE_REG(Axis2_StpPORT->BSRR, Axis2_StpBIT)
#define a2STEP_L WRITE_REG(Axis2_StpPORT->BRR,  Axis2_StpBIT)
#define a2DIR_H  WRITE_REG(Axis2_DirPORT->BSRR, Axis2_DirBIT)
#define a2DIR_L  WRITE_REG(Axis2_DirPORT->BRR,  Axis2_DirBIT)

#define a1CS_H WRITE_REG(Axis1_M2PORT->BSRR, Axis1_M2BIT)
#define a1CS_L WRITE_REG(Axis1_M2PORT->BRR, Axis1_M2BIT)
#define a1CLK_H WRITE_REG(Axis1_M1PORT->BSRR, Axis1_M1BIT)
#define a1CLK_L WRITE_REG(Axis1_M1PORT->BRR, Axis1_M1BIT)
#define a1SDO_H WRITE_REG(Axis1_M0PORT->BSRR, Axis1_M0BIT)
#define a1SDO_L WRITE_REG(Axis1_M0PORT->BRR, Axis1_M0BIT)
#define a1M0(P) if (P) WRITE_REG(Axis1_M0PORT->BSRR, Axis1_M0BIT); else WRITE_REG(Axis1_M0PORT->BRR, Axis1_M0BIT)
#define a1M1(P) if (P) WRITE_REG(Axis1_M1PORT->BSRR, Axis1_M1BIT); else WRITE_REG(Axis1_M1PORT->BRR, Axis1_M1BIT)
#define a1M2(P) if (P) WRITE_REG(Axis1_M2PORT->BSRR, Axis1_M2BIT); else WRITE_REG(Axis1_M2PORT->BRR, Axis1_M2BIT)

#define a2CS_H WRITE_REG(Axis2_M2PORT->BSRR, Axis2_M2BIT)
#define a2CS_L WRITE_REG(Axis2_M2PORT->BRR, Axis2_M2BIT)
#define a2CLK_H WRITE_REG(Axis2_M1PORT->BSRR, Axis2_M1BIT)
#define a2CLK_L WRITE_REG(Axis2_M1PORT->BRR, Axis2_M1BIT)
#define a2SDO_H WRITE_REG(Axis2_M0PORT->BSRR, Axis2_M0BIT)
#define a2SDO_L WRITE_REG(Axis2_M0PORT->BRR, Axis2_M0BIT)
#define a2M0(P) if (P) WRITE_REG(Axis2_M0PORT->BSRR, Axis2_M0BIT); else WRITE_REG(Axis2_M0PORT->BRR, Axis2_M0BIT)
#define a2M1(P) if (P) WRITE_REG(Axis2_M1PORT->BSRR, Axis2_M1BIT); else WRITE_REG(Axis2_M1PORT->BRR, Axis2_M1BIT)
#define a2M2(P) if (P) WRITE_REG(Axis2_M2PORT->BSRR, Axis2_M2BIT); else WRITE_REG(Axis2_M2PORT->BRR, Axis2_M2BIT)


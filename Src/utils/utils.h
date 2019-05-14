#ifndef UTILS_H
#define UTILS_H

//------------------- Some bit macros -------------------------
#define UT_SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define UT_CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define UT_READ_BIT(REG, BIT)    ((REG) & (BIT))

#define UT_CLEAR_REG(REG)        ((REG) = (0x0))

#define UT_WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define UT_READ_REG(REG)         ((REG))

#define UT_MODIFY_REG(REG, CLEARMASK, SETMASK)  UT_WRITE_REG((REG), (((UT_READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
//------------------- Some bit macros -------------------------

#endif // UTILS_H

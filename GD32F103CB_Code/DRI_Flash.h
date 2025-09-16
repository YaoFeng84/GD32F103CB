#ifndef DRI_Flash_h
#define DRI_Flash_h

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"

/***************************************************************************
* 函 数 名: DRI_Flash_ReadData
* 功能描述：Flash 阻塞型 读数据函数
*          支持任意地址任意大小的读取
* 入口参数：
            u32 radd:读地址
            u8 *rd:数据存放首地址
            u16 rl:读字节数
* 出口参数：
       其他值:实际读取字节数
        小于0:操作失败
* 注意事项:  
            无
* 例     如:
* 修改记录:
*           2025-09-01 BY:YJX
***************************************************************************/
extern s32 DRI_Flash_ReadData(u32 radd,u8 *rd,u16 rl);

/***************************************************************************
* 函 数 名: DRI_Flash_DirectWriteData
* 功能描述：Flash 阻塞型直写数据函数(不擦除直接写入)
*          支持任意地址任意大小的写入
* 入口参数：
            u32 wadd:写地址
            u8 *wd:数据源首地址
            u16 wl:写字节数
* 出口参数：
       其他值:实际写字节数
        小于0:操作失败
* 注意事项:  
            不擦除直接写入!!!
* 例   如:
* 修改记录:
*           2025-09-01 BY:YJX
***************************************************************************/
extern s32 DRI_Flash_DirectWriteData(u32 wadd,u8 *wd,u16 wl);

/// @brief 判断地址是否扇区头对齐 或 尾对齐
/// @param addr 待判断的地址
/// @return 小于0:表示不对齐 0:表示头对齐 大于0:表示尾对齐
extern s8 DRI_Flash_IsSectorAligned(u32 addr);

/// @brief 擦除eaddr所在的整个扇区的函数
/// @param eaddr ：要擦除的地址
/// @return ：小于0:擦除失败 其他值：实际擦除字节数 
extern s32 DRI_Flash_EraseSector(u32 eaddr);


/***************************************************************************
* 函 数 名: DRI_Flash_Erase
* 功能描述：Flash 阻塞型 空间擦除函数
* 入口参数：
            u32 eaddr：擦除起始地址
            u32 esize：擦除字节数
* 出口参数：
       其他值:实际擦除字节数
        小于0:擦除失败
* 注意事项:
        支持任意地址任意大小的擦除
        如果地址和大小都是1024字节对齐，则栈空间不会被占用1024字节，否则栈空间会占用1024字节
* 例    如:
* 修改记录:
*           2025-09-01 BY:YJX
***************************************************************************/
extern s32 DRI_Flash_Erase(u32 eaddr,u32 esize);

/**
 * @brief DRI_Flash_EraseWriteData
 * Flash 阻塞型擦写数据函数(先擦除再写入)
 * 支持任意地址任意大小的写入
 * 如果地址和大小都是1024字节对齐，则栈空间不会被占用1024字节，否则栈空间会占用1024字节
 * @param wadd 写入地址
 * @param wd 待写入数据首地址
 * @param wl 待写入数据字节数
 * @return s32 小于0:写失败，其他值:实际写入字节数
 */
extern s32 DRI_Flash_EraseWriteData(u32 wadd,u8 *wd,u32 wl);

/**
 * @brief DRI_Flash_OutCapacity
 * 输出最大空间大小
 * @return u32 最大空间大小，单位字节数
 */
extern u32 DRI_Flash_OutCapacity(void);


#ifdef __cplusplus 
}
#endif 


#endif


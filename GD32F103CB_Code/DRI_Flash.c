/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2025-09-01
 * 作者：YJX
 * 版本号：V1.0（初版）
 ***********************************************
 * 修改内容：
 * 修改日期：
 * 修改作者：
 * 版本号：
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块的说明区-----------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*	
特点：
     
*/
#include "DRI_Flash.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块的对接函数区-----------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
     
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的变量申明和宏定义区--------------------------------                      *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define Flash_SpaceStartAddr  0x08000000//flash空间起始地址
#define Flash_SpaceEndAddr    0x0801FFFF//flash空间结束地址
//
#define Flash_MaxSpaceSize    (Flash_SpaceEndAddr - Flash_SpaceStartAddr + 1)//flash大小，单位字节
#define Flash_MinSectorSize   1024//最小扇区大小，单位字节
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s8 Flash_IsSectorAligned(u32 addr);
static u16 Flash_WriteData(u32 wadd,u8 *wd,u16 wl);
static s32 Flash_SectorErase(u32 secaddr);
static void Flash_SectorProcess(u32 addr,u16 ldata,u16 mfl,u16 rdata);
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/




/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
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
*           2025-09-01 BY:
***************************************************************************/
s32 DRI_Flash_ReadData(u32 radd,u8 *rd,u16 rl)
{
     u8 *rp = (u8 *)radd;
     u16 u16temp;

     if(radd < Flash_SpaceStartAddr || radd > Flash_SpaceEndAddr || (radd + rl) > (Flash_SpaceEndAddr + 1))
     {//越界了
          return -1;
     }

     if(rl)
     {
          for(u16temp = 0;u16temp < rl;u16temp++)
          {
               rd[u16temp] = rp[u16temp];
          }
     }
     return rl;
}

/***************************************************************************
* 函 数 名: DRI_Flash_DirectWriteData
* 功能描述：Flash 阻塞型直写数据函数(不擦除直接写入)
* 入口参数：
            u32 wadd:写地址(需要4字节对齐)
            u8 *wd:数据源首地址
            u16 wl:写字节数(需要4字节对齐)
* 出口参数：
       其他值:实际写字节数
        小于0:操作失败
* 注意事项:  
            不擦除直接写入!!!
* 例   如:
* 修改记录:
*           2025-09-19 BY:
***************************************************************************/
s32 DRI_Flash_DirectWriteData(u32 wadd,u8 *wd,u16 wl)
{//
     if(wadd < Flash_SpaceStartAddr || wadd > Flash_SpaceEndAddr || (wadd + wl) > (Flash_SpaceEndAddr + 1))
     {//越界了
          return -1;
     }

     if((wadd % 4) || (wl % 4))
     {//写入起始地址不是4字节对齐 或 写入字节数不是4字节对齐
     //【1】、对于非4字节地址对齐的，由于芯片在进行字编程时，如果写入的字空间不是0xffffffff，则会导致触发FMC_FLAG_BANK0_PGERR标志，且无法正常写入。
     //即使写入的那个字节空间是0xff，但是由于进行字编程，整体非0xffffffff，也会触发FMC_FLAG_BANK0_PGERR标志，且无法正常写入。
     //【2】、写入长度需要4字节对齐，也是基于此考虑的，因为如果长度不是4字节，势必造成最后一个字编程时，是非整字编程，在不擦除时造成最后的字空间将在后续无法写入！！
     //【3】、虽然提供半字编程，但如果半字刚好处于整字的中间，，也会造成最后一个字节在不擦除时，无法正常写入。
     //所以，此处直接按4字节对齐进行判断拦截。
          return -2;
     }

     return Flash_WriteData(wadd,wd,wl);

//  //以下代码是支持字节级操作，符合整字操作要求，不做改变
//     u8 u8temp,*rp,u8i,u8data[4];
//     u16 u16temp,u16wl;
//      u16wl = wl;//暂存wl
//      if(wl)
//      {         
//           //------------处理首部非4字节对齐
//           u8temp = (u8)(wadd % 4);
//           if(u8temp)
//           {//首部非4字节对齐
//                rp = (u8 *)(wadd - u8temp);//往前u8temp个字节，读u8temp个数据
//                for(u8i = 0;u8i < u8temp;u8i++)
//                {
//                     u8data[u8i] = rp[u8i];
//                }
//                for(;(u8i < 4);u8i++)
//                {
//                     if(wl)
//                     {
//                          wl--;
//                          u8data[u8i] = wd[u8i - u8temp];
//                     }
//                     else
//                     {
//                          break;
//                     }
//                }
//                for(;(u8i < 4);u8i++)
//                {
//                     u8data[u8i] = rp[u8i];
//                }
//                //printf("首部：写地址:0x%08x,写数据:%d,%d,%d,%d\r\n",(u32)rp,u8data[0],u8data[1],u8data[2],u8data[3]);
//                //printf("首部wl = %d\r\n",wl);
//                //对一个完整且对齐的4字节进行写操作
//                Flash_WriteData((u32)rp,u8data,4);
//                //更新写入地址、源数据指针、写入字节数
//                u8temp = 4 - u8temp;
//                wadd += u8temp;
//                wd += u8temp;
//           }
//           //------------写入中间部分
//           if(wl >= 4) 
//           {//
//                //printf("中间:wl = %d\r\n",wl);
//                u16temp = Flash_WriteData(wadd,wd,wl);
//                wadd += u16temp;
//                wd += u16temp;
//                wl -= u16temp;
//           }          
//           //------------处理尾部非4字节对齐
//           u8temp = (u8)(wl % 4);
//           if(u8temp)
//           {
//                //printf("尾部wl = %d\r\n",wl);
//                for(u8i = 0;u8i < u8temp;u8i++)
//                {
//                     u8data[u8i] = wd[u8i];
//                }
//                rp = (u8 *)(wadd + u8temp);
//                for(u8i = u8temp;u8i < 4;u8i++)
//                {
//                     u8data[u8i] = rp[u8i - u8temp];
//                }
//                //对一个完整且对齐的4字节进行写操作
//                //printf("尾部：写地址:0x%08x,写数据:%d,%d,%d,%d\r\n",(u32)wadd,u8data[0],u8data[1],u8data[2],u8data[3]);
//                Flash_WriteData((u32)wadd,u8data,4);
//           }
//      }
//      return u16wl;     
}

/// @brief 判断地址是否扇区头对齐 或 尾对齐
/// @param addr 待判断的地址
/// @return 小于0:表示不对齐 0:表示头对齐 大于0:表示尾对齐
s8 DRI_Flash_IsSectorAligned(u32 addr)
{
     return Flash_IsSectorAligned(addr);
}

/// @brief 擦除eaddr所在的整个扇区的函数
/// @param eaddr ：要擦除的扇区起始地址（需要扇区对齐！！！）
/// @return ：小于0:擦除失败 其他值：实际擦除字节数 
s32 DRI_Flash_EraseSector(u32 eaddr)
{
     return Flash_SectorErase(eaddr);
}


/***************************************************************************
* 函 数 名: DRI_Flash_Erase
* 功能描述：Flash 阻塞型 空间擦除函数
* 入口参数：
            u32 eaddr：擦除起始地址(需要4字节对齐)
            u32 esize：擦除字节数(需要4字节对齐)
* 出口参数：
       其他值:实际擦除字节数
        小于0:擦除失败
* 注意事项:
        如果地址和大小都是1024字节对齐，则栈空间不会被占用1024字节，否则栈空间会占用1024字节
* 例    如:
* 修改记录:
*           2025-09-01 BY:
***************************************************************************/
s32 DRI_Flash_Erase(u32 eaddr,u32 esize)
{
     u16 u16temp,u16l,u16m,u16r,u16len;
     u32 secaddr;

     if(eaddr + esize > Flash_MaxSpaceSize)
     {//越界了
          //printf("越界了，eaddr = 0x%08x,esize = %d",eaddr,esize);
          return -1;
     }

     if((eaddr % 4) || (esize % 4))
     {//写入起始地址不是4字节对齐 或 写入字节数不是4字节对齐
     //此处需要考虑，由于写接口提供了字对齐的操作，所以擦除时，也限制在字对齐，否则，非字对齐的残留空间将无法写入     
          return -2;
     }

     u16len = esize;

     if(esize)
     {
     //-----处理前部
          u16temp = (eaddr % Flash_MinSectorSize);
          if(u16temp)
          {//地址不对齐---判断是否只删除中间扇区
               secaddr = eaddr - u16temp;//使得地址为本扇区对齐
               u16l = u16temp;//扇区前面原数据字节数
               if((u16temp + esize) < Flash_MinSectorSize)
               {//仅扇区中间
                    u16m = esize;//扇区中间删除字节数
                    u16r = (Flash_MinSectorSize - u16temp - esize);//扇区后面原数据字节数
               }
               else
               {//跨扇区
                    u16m = (Flash_MinSectorSize - u16temp);//扇区中间删除字节数
                    u16r = 0;//扇区后面原数据字节数
               }
               //扇区擦除
               Flash_SectorProcess(secaddr,u16l,u16m,u16r);
               //更新参数
               eaddr += u16m;
               esize -= u16m;
          }          
     
     //-----处理中部
          while(esize >= Flash_MinSectorSize)
          {
               Flash_SectorErase(eaddr);
               eaddr += Flash_MinSectorSize;
               esize -= Flash_MinSectorSize;
          }

     //-----处理后部
          if(esize)
          {
               secaddr = eaddr;
               u16l = 0;//扇区前面原数据字节数
               u16m = esize;//扇区中间删除字节数
               u16r = (Flash_MinSectorSize - esize);//扇区后面原数据字节数
               //扇区擦除
               Flash_SectorProcess(secaddr,u16l,u16m,u16r);
               //更新参数
               eaddr += u16m;
               esize -= u16m;
          }
     }

     return u16len;
}

/**
 * @brief DRI_Flash_EraseWriteData
 * Flash 阻塞型擦写数据函数(先擦除再写入)
 * 如果地址和大小都是1024字节对齐，则栈空间不会被占用1024字节，否则栈空间会占用1024字节
 * @param wadd 写入地址(需要4字节对齐)
 * @param wd 待写入数据首地址
 * @param wl 待写入数据字节数(需要4字节对齐)
 * @return s32 小于0:写失败，其他值:实际写入字节数
 */
s32 DRI_Flash_EraseWriteData(u32 wadd,u8 *wd,u32 wl)
{
     s32 s32temp;

     if(wadd + wl > Flash_MaxSpaceSize)
     {//越界了
          return -1;
     }

     if((wadd % 4) || (wl % 4))
     {//写入起始地址不是4字节对齐 或 写入字节数不是4字节对齐
     //此处需要考虑，由于写接口提供了字对齐的操作，所以擦除时，也限制在字对齐，否则，非字对齐的残留空间将无法写入     
          return -2;
     }

     if(wl)
     {
     //先擦除
          s32temp = DRI_Flash_Erase(wadd,wl);
          if(s32temp != wl)
          {
               return s32temp;
          }
     //再写入
          return DRI_Flash_DirectWriteData(wadd,wd,wl);
     }
     return wl;
}

/**
 * @brief DRI_Flash_OutCapacity
 * 输出最大空间大小
 * @return u32 最大空间大小，单位字节数
 */
u32 DRI_Flash_OutCapacity(void)
{
     return Flash_MaxSpaceSize;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
/// @brief 判断地址是否扇区头对齐 或 尾对齐
/// @param addr 待判断的地址
/// @return 小于0:表示不对齐 0:表示头对齐 大于0:表示尾对齐
static s8 Flash_IsSectorAligned(u32 addr)
{
     if(addr < Flash_SpaceStartAddr || addr > Flash_SpaceEndAddr)
     {
          return -1;
     }

     if(addr % Flash_MinSectorSize == 0)
     {//头对齐
          return 0;
     }

     if((addr + 1) % Flash_MinSectorSize == 0)
     {//尾对齐
          return 1;
     }

     return -2;
}

//待写入的地址必须是4字节对齐!!!
//返回实际写入的字节数
static u16 Flash_WriteData(u32 wadd,u8 *wd,u16 wl)
{
     u16 u16temp,u16num;

     //判断地址 和 长度是否4字节对齐
     if((wadd % 4) || (wl % 4) || (wl == 0))
     {
          return 0;
     }

     fmc_unlock();

     fmc_flag_clear(FMC_FLAG_BANK0_END);
     fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
     fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

     u16num = (wl >> 2);
     for(u16temp = 0;u16temp < u16num;u16temp++)
     {
          //
          fmc_word_program(wadd,*(u32 *)wd);
          while (fmc_flag_get(FMC_FLAG_BANK0_BUSY));//等待空闲
          if(fmc_flag_get(FMC_FLAG_BANK0_END))
          {//正常写入
               wadd += 4;
               wd += 4;
          }
          else
          {//异常写入
               if(fmc_flag_get(FMC_FLAG_BANK0_PGERR))
               {
               }
               else if(fmc_flag_get(FMC_FLAG_BANK0_WPERR))
               {
               }
               break;
          } 

          fmc_flag_clear(FMC_FLAG_BANK0_END);
          fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
          fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

     }

     fmc_flag_clear(FMC_FLAG_BANK0_END);
     fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
     fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
     
     fmc_lock();

     return (u16temp << 2);
}

//纯扇区擦除
//secaddr为扇区地址，必须是扇区对齐
//返回小于0表示失败 其他值表示实际擦除字节数
static s32 Flash_SectorErase(u32 secaddr)
{
     s32 s32temp = 0;

     if(secaddr < Flash_SpaceStartAddr || secaddr > Flash_SpaceEndAddr)
     {
          return -1;
     }

     if(Flash_IsSectorAligned(secaddr))
     {//非扇区头对齐
          return -2;
     }

     // unlock the flash program/erase controller 
     fmc_unlock();

     // clear all pending flags 
     fmc_flag_clear(FMC_FLAG_BANK0_END);
     fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
     fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

     fmc_page_erase(secaddr);//(((secaddr >> 10) << 10));
     while (fmc_flag_get(FMC_FLAG_BANK0_BUSY));// 等待操作完成     
     if(fmc_flag_get(FMC_FLAG_BANK0_END))
     {
          s32temp = (s32)Flash_MinSectorSize;
     }
     else
     {
          s32temp = -3;
          // if(fmc_flag_get(FMC_FLAG_BANK0_WPERR))
          // {
          //      s32temp = -3;
          // }
          // else if(fmc_flag_get(FMC_FLAG_BANK0_PGERR))
          // {                    
          //      s32temp = -4;
          // }
     }   
     fmc_flag_clear(FMC_FLAG_BANK0_END);
     fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
     fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

     // lock the main FMC after the erase operation 
     fmc_lock();

     return s32temp;
}

/**
 * @brief 
 * 扇区处理，主要用于半扇区擦除时，还有半扇区数据需要在擦除后再写入
 * @param addr 扇区起始地址(要求扇区对齐)
 * @param ldata 扇区前部读取字节数(可以为0)
 * @param mfl 扇区中部填充0xff字节数(不能为0)
 * @param rdata 扇区后部读取字节数(可以为0)
 */
static void Flash_SectorProcess(u32 addr,u16 ldata,u16 mfl,u16 rdata)
{
     u8 buff[Flash_MinSectorSize];
     u8 *rp = (u8*)addr;
     u16 u16i = 0;

     //整理数据
     for(;u16i < ldata;)
     {
          buff[u16i++] = *(u8 *)rp;
          rp++;
     }
     for(;u16i < (ldata + mfl);)
     {
          buff[u16i++] = 0xff;
          rp++;
     }
     for(;u16i < (ldata + mfl + rdata);)
     {
          buff[u16i++] = *(u8 *)rp;
          rp++;
     }
     //擦除扇区
     Flash_SectorErase(addr);//擦除扇区
     //写扇区数据
     Flash_WriteData(addr,buff,Flash_MinSectorSize);
}



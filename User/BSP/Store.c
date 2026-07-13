#include "Store.h"
#include "MyFLASH.h"

// 存储的起始地址
#define STORE_START_ADDRESS		0x0800FC00
// 存储数据的个数
#define STORE_COUNT				512

// 定义SRAM数组
uint16_t Store_Data[STORE_COUNT];

/**
 * 函数：参数存储模块初始化
 * 参数：无
 * 返回值：无
 */
void Store_Init(void)
{
	uint16_t i;
	/* 判断是不是第一次使用 */
	// 读取第一个半字的标志位，if成立，则执行第一次使用的初始化
	if (MyFLASH_ReadHalfWord(STORE_START_ADDRESS) != 0xA5A5)
	{
		// 擦除指定页
		MyFLASH_ErasePage(STORE_START_ADDRESS);
		// 在第一个半字中写入自己规定的标志位，用于判断是不是第一次使用
		MyFLASH_ProgramHalfWord(STORE_START_ADDRESS, 0xA5A5);
		// 循环STORE_COUNT次，除了第一个标志位
		for (i = 1; i < STORE_COUNT; i++)
		{
			// 除了标志位的有效数据全部清0
			MyFLASH_ProgramHalfWord(STORE_START_ADDRESS + i * 2, 0x0000);
		}
	}
	
	/* 上电时，将闪存数据加载回SRAM数组，实现SRAM数组的掉电不丢失 */
	// 循环STORE_COUNT次，包括第一个标志位
	for (i = 0; i < STORE_COUNT; i++)
	{
		// 将闪存的数据加载回SRAM数组
		Store_Data[i] = MyFLASH_ReadHalfWord(STORE_START_ADDRESS + i * 2);
	}
}

/**
 * 函数：参数存储模块保存数据到闪存
 * 参数：无
 * 返回值：无
 */
void Store_Save(void)
{
	uint16_t i;
	// 擦除指定页
	MyFLASH_ErasePage(STORE_START_ADDRESS);
	// 循环STORE_COUNT次，包括第一个标志位
	for (i = 0; i < STORE_COUNT; i++)
	{
		// 将SRAM数组的数据备份保存到闪存
		MyFLASH_ProgramHalfWord(STORE_START_ADDRESS + i * 2, Store_Data[i]);
	}
}

/**
 * 函数：参数存储模块将所有有效数据清0
 * 参数：无
 * 返回值：无
 */
void Store_Clear(void)
{
	uint16_t i;
	// 循环STORE_COUNT次，除了第一个标志位
	for (i = 1; i < STORE_COUNT; i++)
	{
		// SRAM数组有效数据清0
		Store_Data[i] = 0x0000;
	}
	// 保存数据到闪存
	Store_Save();
}

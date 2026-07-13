#include "DMA_ADC.h"

// 遥控器电压的100倍
// uint16_t Battery_RC;
// 存放ADC转换读到的16位原始数据
uint16_t ADC_Value[M];

/**
 * 函数：ADC的IO初始化
 * 参数：无
 * 返回值：无
 */
static void ADC1_GPIO_Config(void)
{
	/* 开启时钟 */
	// 开启ADC1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	// 开启GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	// 开启DMA1的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* 设置ADC时钟 */
	// 选择时钟6分频，ADCCLK = 72MHz / 6 = 12MHz，ADC时钟最大不能超过14MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	/* GPIO初始化 */
	// 将PA0~PA7引脚初始化为模拟输入
	GPIO_InitTypeDef GPIO_InitStructure;
	// 模拟输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	// PA0~PA7为ADC输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | 
	GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	// 输出速度为50MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO初始化
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * 函数：ADC及DMA配置
 * 参数：无
 * 返回值：无
 */
static void ADC1_Mode_Config(void)
{
	// 重置ADC1为默认值
	ADC_DeInit(ADC1);
	// 使能内部参考电压（1.2V）
	ADC_TempSensorVrefintCmd(ENABLE);
	
	/* 规则组通道配置 */
	// 规则组序列1~8的位置分别配置为通道0~7，序列9的位置配置为通道17，
	// 普通采样时间为71.5个周期
	// 自转微调电位器，AUX2
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_71Cycles5);
	// 前后微调电位器，AUX3
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_71Cycles5);
	// 左右微调电位器，AUX1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_71Cycles5);
	// 3.7V锂电池电压，AUX4
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5);
	// 左摇杆的Y轴（上下），油门（THROTTLE）
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_71Cycles5);
	// 左摇杆的X轴（左右），偏航角（YAW）
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_71Cycles5);
	// 右摇杆的Y轴（上下），俯仰角（PITCH）
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_71Cycles5);
	// 右摇杆的X轴（左右），横滚角（ROLL）
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_71Cycles5);
	// 内部参考电压（1.2V）
	ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 9, ADC_SampleTime_239Cycles5);
	
	/* ADC初始化 */
	// 定义结构体变量
	ADC_InitTypeDef ADC_InitStructure;
	// 模式，选择独立模式，即单独使用ADC1
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	// 数据对齐，选择右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	// 外部触发，使用软件触发，不需要外部触发
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// 连续转换，使能，每转换一次规则组序列后立刻开始下一次转换
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// 扫描模式，使能，扫描规则组的序列
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	// 通道数，即扫描数量为M：9
	ADC_InitStructure.ADC_NbrOfChannel = M;
	// 将结构体变量交给ADC_Init，配置ADC1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	// 重置DMA1的通道1为默认值
	DMA_DeInit(DMA1_Channel1);
	
	/* DMA初始化 */
	// 定义结构体变量
	DMA_InitTypeDef DMA_InitStructure;
	// 外设基地址，即 4001 244C
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	// 外设数据宽度，选择半字，对应16位的ADC数据寄存器
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	// 外设地址自增，选择失能，始终以ADC数据寄存器为源
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 存储器基地址，给定存放AD转换结果的全局数组ADC_Value
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Value;
	// 存储器数据宽度，选择半字，与源数据宽度对应
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	// 存储器地址自增，选择使能
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// 数据传输方向，选择由外设到存储器
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// 转运的数据大小（转运次数），与ADC通道数一致
	DMA_InitStructure.DMA_BufferSize = M;
	// 模式，选择循环模式，与ADC的连续转换一致
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	// 存储器到存储器，选择失能，数据由ADC外设触发转运到存储器
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	// 优先级，选择高优先级
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	// 将结构体变量交给DMA_Init，配置DMA1的通道1
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* DMA使能 */
	// DMA1的通道1使能
	DMA_Cmd(DMA1_Channel1, ENABLE);
	// ADC1触发DMA1的信号使能
	ADC_DMACmd(ADC1, ENABLE);
	
	/* ADC使能 */
	// 使能ADC1，ADC开始运行
	ADC_Cmd(ADC1, ENABLE);
	
	/* ADC校准 */
	// 固定流程，内部有电路会自动执行校准
	// 复位校准
	ADC_ResetCalibration(ADC1);
	// 判断复位校准状态，如果没有校准完成，则一直循环；校准完成，则退出循环
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	// 启动校准
	ADC_StartCalibration(ADC1);
	// 判断校准状态，如果没有校准完成，则一直循环；校准完成，则退出循环
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
	/* ADC触发 */
	// 软件触发ADC开始工作，由于ADC处于连续转换模式，
	// 故触发一次后ADC就可以一直连续不断地工作
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
 * 函数：ADC1初始化
 * 参数：无
 * 返回值：无
 */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}

/**
 * 函数：简单均值滤波
 * 参数：无
 * 返回值：无
 */
uint16_t Get_Filtered_ADC(uint16_t Value)
{
    uint32_t sum = 0;
	uint8_t i;
    for(i = 0; i < 10; i++) 
	{ 
		sum += Value;
	}
    return sum / 10;
}

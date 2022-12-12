/**
  ******************************************************************************
  * File Name          : DigtalSignal_Process.c
  * Description        : The file includes commonly used digital filtering, 
  *						 frequency domain transformation and motion control algorithms
  ******************************************************************************
 */
  
/* Includes ------------------------------------------------------------------*/
#include "DigtalSignal_Process.h"
#include "math.h"

/* Whether to use functions from the ARM-DSP library or use inefficient digital processing libraries */
#if(_ARM_DSP_USED == 1)
	/*
		Digital signal processing related library
	*/
	#include "arm_math.h"
	#include "arm_const_structs.h"
#endif

/* External function declaration----------------------------------------------*/


/* Private macro definitions--------------------------------------------------*/

/* Global variable------------------------------------------------------------*/

/* An input array for testing */
volatile static float32_t TempBuff[FFT_LENGTH*2] = {0};
/* A processed array for testing */
volatile static float32_t ProcessedTempBuff[FFT_LENGTH*2] = {0};
/* Number of function calls */
static uint8_t run_count = 0;

/* Static function definition-------------------------------------------------*/

static void conjugate_complex(int n,ComplexStruct in[],ComplexStruct out[]);
static void c_plus(ComplexStruct a,ComplexStruct b,ComplexStruct *c); 
static void c_mul(ComplexStruct a,ComplexStruct b,ComplexStruct *c) ; 
static void c_sub(ComplexStruct a,ComplexStruct b,ComplexStruct *c);	
static void c_div(ComplexStruct a,ComplexStruct b,ComplexStruct *c);	
static void fft(int N,ComplexStruct f[]); 
static void ifft(int N,ComplexStruct f[]);  
static void c_abs(ComplexStruct f[],float out[],int n); 
static void Wn_i(int n,int i,ComplexStruct *Wn,char flag);

/* Function definition--------------------------------------------------------*/

/** 
* @description: Mean filtering function
* @param  {Mean_Filter*} p_MeanFilterStruct : Filter structure pointer
* @param  {float*}    Temp_Data_Buf      : Cache array acquisition results
* @return {float}                        : Mean filtering result
* @author: leeqingshui 
*/
float Data_Mean_Filter_F(Mean_Filter_F* p_MeanFilterStruct,float Temp_Data_Buf[])
{
    p_MeanFilterStruct->sum = 0;
	 
	for(int count = 0;count < MEAN_FILTER_NUM;count++)
	{
		p_MeanFilterStruct->Data_Buf[count] = Temp_Data_Buf[count];
		p_MeanFilterStruct->sum = p_MeanFilterStruct->sum + p_MeanFilterStruct->Data_Buf[count];
	}
	
	p_MeanFilterStruct->result = (float)(p_MeanFilterStruct->sum / MEAN_FILTER_NUM);

	return p_MeanFilterStruct->result;
}
 
/** 
* @description: Mean filtering Reset function 
* @param  {Mean_Filter*} p_MeanFilterStruct : Filter structure pointer
* @return {void}                       
* @author: leeqingshui 
*/
void Mean_Filter_Rest_F(Mean_Filter_F* p_MeanFilterStruct)
{
	 for(int count = 0;count < MEAN_FILTER_NUM;count++)
	 {
		 p_MeanFilterStruct->Data_Buf[count] = 0;
	 }
	 
	 p_MeanFilterStruct->result = 0;
	 
	 p_MeanFilterStruct->sum = 0;
}

/** 
* @description: Mean filtering function
* @param  {Mean_Filter*} p_MeanFilterStruct : Filter structure pointer
* @param  {uint16_t*}    Temp_Data_Buf      : Cache array acquisition results
* @return {float}                           : Mean filtering result
* @author: leeqingshui 
*/
uint16_t Data_Mean_Filter_U16(Mean_Filter_U16* p_MeanFilterStruct,uint16_t Temp_Data_Buf[])
{
    p_MeanFilterStruct->sum = 0;
	 
	for(int count = 0;count < MEAN_FILTER_NUM;count++)
	{
		p_MeanFilterStruct->Data_Buf[count] = Temp_Data_Buf[count];
		p_MeanFilterStruct->sum = p_MeanFilterStruct->sum + p_MeanFilterStruct->Data_Buf[count];
	}
	
	p_MeanFilterStruct->result = (uint16_t)(p_MeanFilterStruct->sum / MEAN_FILTER_NUM);

	return p_MeanFilterStruct->result;
}

/** 
* @description: Mean filtering Reset function 
* @param  {Mean_Filter*} p_MeanFilterStruct : Filter structure pointer
* @return {void}                       
* @author: leeqingshui 
*/
void Mean_Filter_Rest_U16(Mean_Filter_U16* p_MeanFilterStruct)
{
	 for(int count = 0;count < MEAN_FILTER_NUM;count++)
	 {
		 p_MeanFilterStruct->Data_Buf[count] = 0;
	 }
	 
	 p_MeanFilterStruct->result = 0;
	 
	 p_MeanFilterStruct->sum = 0;
}

/** 
* @description: Initialize the Kalman filter
* @param  {Kalman_Filter*} p_Kalman_Filter : Filter structure pointer
* @return {void}                       
* @author: leeqingshui 
*/
void KalmanFilter_Init(Kalman_Filter* p_Kalman_Filter)
{
	p_Kalman_Filter->error     = 0.01;
	p_Kalman_Filter->last_data = 0;
	p_Kalman_Filter->now_data  = 0;
	p_Kalman_Filter->q         = 0.01;
	p_Kalman_Filter->r         = 0.01;
	p_Kalman_Filter->kGain     = 0;
}

/** 
* @description: Data were filtered by Kalman filter 
* @param  {float} InData                   : Kalman filter input value
* @param  {Kalman_Filter*} p_Kalman_Filter : Filter structure pointer
* @return {float}                          : Kalman filter prediction value
* @author: leeqingshui 
*/
float KalmanFilter_Calculate(Kalman_Filter* p_Kalman_Filter , float InData)
{
	p_Kalman_Filter->now_data = InData;
	p_Kalman_Filter->error = p_Kalman_Filter->error + p_Kalman_Filter->q;
	
	/* Compute the Kalman gain */
	p_Kalman_Filter->kGain = (p_Kalman_Filter->error)/(p_Kalman_Filter->error + p_Kalman_Filter->r);
	// Calculate the filter estimate
	p_Kalman_Filter->now_data = p_Kalman_Filter->last_data + ((p_Kalman_Filter->kGain)*(p_Kalman_Filter->now_data - p_Kalman_Filter->last_data));
	// Update the measurement variance
	p_Kalman_Filter->error = (1-p_Kalman_Filter->kGain)*(p_Kalman_Filter->error);
	
	p_Kalman_Filter->last_data = p_Kalman_Filter->now_data;
	
	return (p_Kalman_Filter->now_data);
}


/** 
* @description: Get the absolute value of an array
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t*} p_DstpBuff : A pointer to the processed array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Abs(float32_t* p_SrcBuff,float32_t* p_DstpBuff,uint32_t Buff_Size)
{	
	#if(_DSP_ABS_USED == 1)
		arm_abs_f32(p_SrcBuff,p_DstpBuff,Buff_Size);
	#else
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			if((*(p_SrcBuff + i))<0)
			{
				*(p_DstpBuff + i) = -(*(p_SrcBuff + i));
			}
		}
	#endif
}

/* !! 
	This function causes Hardfault for an unknown reason and does not perform properly !!
	Notice here that if you use sizeof on an array to find its length, it will cause Hardfault
*/
/** 
* @description: Get the offset additioned value of an array
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t}  offset     : Data offset
* @param  {float32_t*} p_DstpBuff : A pointer to the processed array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Offeset(float32_t* p_SrcBuff, float32_t offset,float32_t* p_DstpBuff,uint32_t Buff_Size)
{
	#if(_DSP_OFFSET_USED == 1)
		arm_offset_f32(p_SrcBuff, offset, p_DstpBuff, Buff_Size);	

	#else
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			*(p_DstpBuff + i) = *(p_SrcBuff + i) + offset;
		}
	
	#endif
}

/* !! 
	This function causes Hardfault for an unknown reason and does not perform properly !!
	Notice here that if you use sizeof on an array to find its length, it will cause Hardfault
*/
/** 
* @description: Computes the maximum value of the data array. This function returns the maximum value and its position in the array
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @param  {float32_t*} p_Result   : the maximum value of the data array
* @param  {uint32_t*}  p_Index    : The maximum element is indexed in the array
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Max(float32_t* p_SrcBuff, uint32_t Buff_Size, float32_t* p_Result, uint32_t* p_Index)
{
	#if(_DSP_MAX_USED == 1)
		arm_max_f32 (p_SrcBuff, Buff_Size, p_Result, p_Index);
	
	#else
		// Assume that the first element in the array is the maximum value
		float32_t temp_max = *p_SrcBuff;
		uint32_t  temp_index = 0;
		
		// Go through the group, getting each element, ready to compare
		for(uint32_t i = 1; i < Buff_Size; i++)
		{
			// If something larger than max occurs during the comparison, let max record the larger value
			if((*(p_SrcBuff + i)) > temp_max)
			{
					temp_max = *(p_SrcBuff + i);
					temp_index = i;
			}
		}
		
		// The loop ends and the result is retrieved
		*p_Result = temp_max;
		*p_Index  = temp_index;
		
	#endif
}

/* !! 
	This function causes Hardfault for an unknown reason and does not perform properly !!
	Notice here that if you use sizeof on an array to find its length, it will cause Hardfault
*/
/** 
* @description: The target array is multiplied by the proportionality constant
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t}  ratio      : the proportionality constant
* @param  {float32_t*} p_DstpBuff : A pointer to the processed array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Scale(float32_t* p_SrcBuff, float32_t ratio,float32_t* p_DstpBuff,uint32_t Buff_Size)
{
	#if(_DSP_SCALE_USED == 1)
		arm_scale_f32(p_SrcBuff, ratio, p_DstpBuff, Buff_Size);	
	
	#else
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			*(p_DstpBuff + i) = *(p_SrcBuff + i)*ratio;
		}
	
	#endif
}

/** 
* @description: Get the array average
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t*} p_Result   : Average of array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Mean(float32_t* p_SrcBuff, uint32_t Buff_Size, float32_t* p_Result)
{
	#if(_DSP_MEAN_USED == 1)
		arm_mean_f32(p_SrcBuff, Buff_Size, p_Result);	
	
	#else
		float sum = 0;
		
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			sum = sum + *(p_SrcBuff+i);
		}
		
		*p_Result = sum/Buff_Size;
	
	#endif
}

/** 
* @description: Computes the minimum value of the data array. This function returns the minimum value and its position in the array
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @param  {float32_t*} p_Result   : the minimum value of the data array
* @param  {uint32_t*}  p_Index    : The minimum element is indexed in the array
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Min(float32_t* p_SrcBuff, uint32_t Buff_Size, float32_t* p_Result, uint32_t* p_Index)
{
	#if(_DSP_MIN_USED == 1)
		arm_min_f32 (p_SrcBuff, Buff_Size, p_Result, p_Index);
	
	#else
		// Assume that the first element in the array is the maximum value
		float32_t temp_min = *p_SrcBuff;
		uint32_t  temp_index = 0;
		
		// Go through the group, getting each element, ready to compare
		for(uint32_t i = 1; i < Buff_Size; i++)
		{
			// If something larger than max occurs during the comparison, let max record the larger value
			if((*(p_SrcBuff + i)) < temp_min)
			{
					temp_min = *(p_SrcBuff + i);
					temp_index = i;
			}
		}
		
		// The loop ends and the result is retrieved
		*p_Result = temp_min;
		*p_Index  = temp_index;
		
	#endif
}

/** 
* @description: Gets the sum of squares in the array
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t*} p_Result   : the sum of squares of array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Power(float32_t* p_SrcBuff, uint32_t Buff_Size, float32_t* p_Result)
{
	#if(_DSP_POWER_USED == 1)
		arm_power_f32(p_SrcBuff, Buff_Size, p_Result);	
	
	#else
		float power = 0;
		
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			power = (*(p_SrcBuff+i))*(*(p_SrcBuff+i)) + power;
		}
		
		*p_Result = power;
	
	#endif
}

/** 
* @description: Gets the Root Mean Sqaure of the array
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t*} p_Result   : the Root Mean Sqaure of squares of array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Rms(float32_t* p_SrcBuff, uint32_t Buff_Size, float32_t* p_Result)
{
	#if(_DSP_RMS_USED == 1)
		arm_rms_f32(p_SrcBuff, Buff_Size, p_Result);	
	
	#else
		float power = 0;
		
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			power = (*(p_SrcBuff+i))*(*(p_SrcBuff+i)) + power;
		}
		
		*p_Result = sqrt(power/Buff_Size);
	
	#endif
}

/** 
* @description: Gets the Standard deviation of the array
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t*} p_Result   : the Standard deviation of squares of array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Std(float32_t* p_SrcBuff, uint32_t Buff_Size, float32_t* p_Result)
{
	#if(_DSP_STD_USED == 1)
		arm_std_f32(p_SrcBuff, Buff_Size, p_Result);	
	
	#else
		float sumOfSquares = 0;
		float sum		   = 0;
		
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			sum = *(p_SrcBuff+i) + sum;
		}
		
		Get_DataBuff_Power(p_SrcBuff, Buff_Size, (float32_t*)&sumOfSquares);
		
		*p_Result = sqrt((sumOfSquares - (sum*sum/Buff_Size))/(Buff_Size - 1));
	
	#endif
}

/** 
* @description: Get the array variance
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t*} p_Result   : the variance of squares of array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Var(float32_t* p_SrcBuff, uint32_t Buff_Size, float32_t* p_Result)
{
	#if(_DSP_VAR_USED == 1)
		arm_var_f32(p_SrcBuff, Buff_Size, p_Result);	
	
	#else
		float meanOfElements = 0;
		float sum		   = 0;
		
		Get_DataBuff_Std(p_SrcBuff, Buff_Size, (float32_t*)&meanOfElements);
	
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			sum = (*(p_SrcBuff+i) - meanOfElements)*(*(p_SrcBuff+i) - meanOfElements) + sum;
		}
		
		*p_Result = sum/(Buff_Size);
	
	#endif
}

/** 
* @description: Compute complex array magnitude values 
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t*} p_DstpBuff : the variance of squares of array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_ComplexMag(float32_t* p_SrcBuff, uint32_t Buff_Size, float32_t* p_DstpBuff)
{
	#if(_DSP_COMPLEX_MAG_USED == 1)
		arm_cmplx_mag_f32(p_SrcBuff, p_Result, Buff_Size);
	
	#else
	
		for(int32_t i=0; i<Buff_Size; i++) 
		{
			*(p_DstpBuff+i) = sqrt((*(p_SrcBuff+(2*i+0)))*(*(p_SrcBuff+(2*i+0))) + (*(p_SrcBuff+(2*i+1)))*(*(p_SrcBuff+(2*i+1))));
		}
	
	#endif
}

/** 
* @description: Fast Fourier transform 
* @param  {FFT_LENGTH_STATUS} p_Result   : Number of Fourier transform points (62, 256, or 1024)
* @param  {float32_t*} 		  p_SrcBuff  : points to the complex data buffer of size 2*fftLen. Processing occurs in-place.
* @param  {float32_t*}   	  p_DstpBuff : Fourier amplitude value output array
* @param  {uint32_t}   		  Buff_Size  : Size of Dst Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_FFT(FFT_LENGTH_STATUS length, float32_t* p_SrcBuff, float32_t* p_DstpBuff, uint32_t Dst_Buff_Size)
{
	#if(_DSP_FFT_USED == 1)
		switch((FFT_LENGTH_STATUS)length)
		{
			case Length_64 	:
								arm_cfft_f32(&arm_cfft_sR_f32_len64,p_SrcBuff,0,1);
								arm_cmplx_mag_f32(p_SrcBuff, p_DstpBuff, Dst_Buff_Size); 
								break;
			case Length_256	:
								arm_cfft_f32(&arm_cfft_sR_f32_len256,p_SrcBuff,0,1);
								arm_cmplx_mag_f32(p_SrcBuff, p_DstpBuff, Dst_Buff_Size); 
								break;
			case Length_1024:
								arm_cfft_f32(&arm_cfft_sR_f32_len1024,p_SrcBuff,0,1);
								arm_cmplx_mag_f32(p_SrcBuff, p_DstpBuff, Dst_Buff_Size); 
								break;
			default: return;
		}
	
	#else
		switch((FFT_LENGTH_STATUS)length)
		{
			case Length_64 	: 
								break;
			case Length_256	: 
								break;
			case Length_1024:
								ComplexStruct In_DataBuf[1024];
								
								for(int32_t i=0; i<1024; i++)
								{
									In_DataBuf[i].real = *(p_SrcBuff + 2*i);
									In_DataBuf[i].imag = *(p_SrcBuff + 2*i+1);
								}
								
								fft(1024,In_DataBuf);
								
								for(int32_t i=0; i<Dst_Buff_Size; i++)
								{
									*(p_DstpBuff + 2*i)   = In_DataBuf[i].real;
									*(p_DstpBuff + 2*i+1) = In_DataBuf[i].imag;
								}			
	
								break;
			default: return;
		}
		
	#endif
}


/* Functions for testing digital signals */
void DigtalSignal_Process_Test(void)
{
	static float32_t Result = 0;
	static uint32_t  Index  = 0;

	
	/* Generate a sinusoidal signal */
	for(int i=0;i<FFT_LENGTH;i++)
	{
		/* Real part of the signal, DC component is 100,1HZ signal amplitude is 10, 50HZ signal amplitude is 20,300 HZ signal amplitude is 30. */
		TempBuff[2*i]=100+
		5*arm_sin_f32(2*PI*i*10/FFT_LENGTH)+
		10*arm_sin_f32(2*PI*i*50/FFT_LENGTH)+
		20*arm_cos_f32(2*PI*i*300/FFT_LENGTH);	
		
		/* Imaginary part of the signal, all of which are 0 */
		TempBuff[2*i+1]=0;
	}
	
	Get_DataBuff_FFT(Length_1024, (float32_t*)TempBuff, (float32_t*)ProcessedTempBuff,FFT_LENGTH);
}

/* Conjugate of complex array */
static void conjugate_complex(int n,ComplexStruct in[],ComplexStruct out[])
{
  int i = 0;
  for(i=0;i<n;i++)
  {
    out[i].imag = -in[i].imag;
    out[i].real = in[i].real;
  }
}

/* Complex array takes the magnitude value */
static void c_abs(ComplexStruct f[],float out[],int n)
{
  int i = 0;
  float t;
  for(i=0;i<n;i++)
  {
    t = f[i].real * f[i].real + f[i].imag * f[i].imag;
    out[i] = sqrt(t);
  }
}

/* Complex array addition */
static void c_plus(ComplexStruct a,ComplexStruct b,ComplexStruct *c)
{
  c->real = a.real + b.real;
  c->imag = a.imag + b.imag;
}

/* Complex array subtraction */
static void c_sub(ComplexStruct a,ComplexStruct b,ComplexStruct *c)
{
  c->real = a.real - b.real;
  c->imag = a.imag - b.imag;
}

/* Complex array multiplication */
static void c_mul(ComplexStruct a,ComplexStruct b,ComplexStruct *c)
{
  c->real = a.real * b.real - a.imag * b.imag;
  c->imag = a.real * b.imag + a.imag * b.real;
}

/* Complex array division */
static void c_div(ComplexStruct a,ComplexStruct b,ComplexStruct *c)
{
  c->real = (a.real * b.real + a.imag * b.imag)/(b.real * b.real +b.imag * b.imag);
  c->imag = (a.imag * b.real - a.real * b.imag)/(b.real * b.real +b.imag * b.imag);
}

/* FFT correlation operation */
void Wn_i(int n,int i,ComplexStruct *Wn,char flag)
{
    Wn->real = cos(2*PI*i/n);
    if(flag == 1)
		Wn->imag = -sin(2*PI*i/n);
    else if(flag == 0)
		Wn->imag = -sin(2*PI*i/n);
}

/* Fast Fourier transform */
void fft(int N,ComplexStruct f[])
{
  ComplexStruct t,wn;
  int i,j,k,m,n,l,r,M;
  int la,lb,lc;
	
  /*---- computes the decomposition series M=log2(N)----*/
  for(i=N,M=1;(i=i/2)!=1;M++);
	
  /*---- Rearranges the original signals in inverted order ----*/
  for(i=1,j=N/2;i<=N-2;i++)
  {
    if(i<j)
    {
      t=f[j];
      f[j]=f[i];
      f[i]=t;
    }
    k=N/2;
    while(k<=j)
    {
      j=j-k;
      k=k/2;
    }
    j=j+k;
  }

  /*----FFT algorithm----*/
  for(m=1;m<=M;m++)
  {
    la=pow(2,m);// la=2^m represents the number of nodes in each packet of level m
    lb=la/2;    // lb represents the number of disc elements contained in each block of level m
                // It also represents the distance between the upper and lower nodes of each dish element
	  
    /*---- Disk operation ----*/
    for(l=1;l<=lb;l++)
    {
      r=(l-1)*pow(2,M-m);
      for(n=l-1;n<N-1;n=n+la) // Each group is traversed. The total number of groups is N/la
      {
        lc=n+lb; //n,lc represents the upper and lower node numbers of a disk-shaped unit, respectively
        Wn_i(N,r,&wn,1);//wn=Wnr
        c_mul(f[lc],wn,&t);//t = f[lc] * wn 
        c_sub(f[n],t,&(f[lc]));//f[lc] = f[n] - f[lc] * Wnr
        c_plus(f[n],t,&(f[n]));//f[n] = f[n] + f[lc] * Wnr
      }
    }
  }
}






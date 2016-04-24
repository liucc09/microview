
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "MVCAPI.h"

#include "mex.h"


HANDLE*  m_phMVCs;
BYTE*   m_pRawDatas[5];
struct CapInfoStruct* m_pCapInfos;
int*	pplhs0;


void free_data(int camera_num);
void init_camera(int camera_num, int* pcamera_ids, int* pwidths, int* pheights, int* phOffs, int* pvOffs);

/***
*输入参数：五个行向量，分别是相机标号，每个相机获取图像的宽和高，图像的水平偏置和垂直偏置
*输出参数：n*3 矩阵
*       |相机标号1|m_hMVs指针|m_CapInfo指针|
*       |相机标号2|m_hMVs指针|m_CapInfo指针|
*       ...
***/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	int camera_num,i;
    int camera_ids[6];
    int widths[6];
    int heights[6];
    int hOffs[6];
    int vOffs[6];
    double *pInputs[5];
    
	//分别相机标号、宽、高、水平偏置、垂直偏置 行向量
	if(nrhs != 5) {
		mexErrMsgIdAndTxt("MyToolbox:mvc_init_camera:nrhs",
                      "Five inputs required.");
	}
	//输出[相机标号,m_hMVs指针,m_CapInfo指针]
	if(nlhs != 1) {
		mexErrMsgIdAndTxt("MyToolbox:mvc_init_camera:nlhs",
						  "One output required.");
	}

	camera_num = mxGetN(prhs[0]);
    
       
    for (i=0;i<5;++i){
        pInputs[i] = mxGetPr(prhs[i]);
    }
    for (i=0;i<camera_num;++i){
        camera_ids[i] = (int)(pInputs[0][i]);
        widths[i] = (int)(pInputs[1][i]);
        heights[i] = (int)(pInputs[2][i]);
        hOffs[i] = (int)(pInputs[3][i]);
        vOffs[i] = (int)(pInputs[4][i]);
        mexPrintf("camera: %d %d %d %d %d\n",camera_ids[i],widths[i],heights[i],hOffs[i],vOffs[i]);
    }


	m_phMVCs = (HANDLE*)malloc(camera_num*sizeof(HANDLE));
	memset(m_phMVCs,0,camera_num*sizeof(HANDLE));

	m_pCapInfos = (struct CapInfoStruct*)malloc(camera_num*sizeof(struct CapInfoStruct));
	memset(m_pCapInfos,0,camera_num*sizeof(struct CapInfoStruct));
	
	init_camera(camera_num,camera_ids,widths,heights,hOffs,vOffs);
	
	plhs[0] = mxCreateNumericMatrix(3,(mwSize)camera_num,mxINT32_CLASS,mxREAL);
	

	pplhs0 = (int*)mxGetData(plhs[0]);
    for (i=0;i<camera_num;++i){
        pplhs0[i*3+0] = camera_ids[i];
        pplhs0[i*3+1] = (int)(m_phMVCs+i);
        pplhs0[i*3+2] = (int)(m_pCapInfos+i);
    }
	
}

void init_camera(int camera_num, int* pcamera_ids, int* pwidths, int* pheights, int* phOffs, int* pvOffs){
	int nIndex=0;
	int rt;
	int RawDataSize;
		
	for (nIndex=0;nIndex<camera_num;++nIndex){
        RawDataSize = pwidths[nIndex]*pheights[nIndex];
		m_pRawDatas[nIndex]=(BYTE*)malloc(RawDataSize*sizeof(BYTE));
		memset(m_pRawDatas[nIndex],0,RawDataSize*sizeof(BYTE));

		struct CapInfoStruct *m_pCapInfo = m_pCapInfos+nIndex;
		memset(m_pCapInfo, 0, sizeof(struct CapInfoStruct));
		m_pCapInfo->Buffer = m_pRawDatas[nIndex];

		m_pCapInfo->Height = pheights[nIndex];
		m_pCapInfo->Width  = pwidths[nIndex];
		m_pCapInfo->HorizontalOffset = phOffs[nIndex];
		m_pCapInfo->VerticalOffset   = pvOffs[nIndex];
		m_pCapInfo->Exposure         = 1024;
		m_pCapInfo->Gain[0]          = 17;
		m_pCapInfo->Gain[1]          = 9;
		m_pCapInfo->Gain[2]          = 15;
		m_pCapInfo->Control          = 0;
		memset(m_pCapInfo->Reserved, 0, 8);
		m_pCapInfo->Reserved[0]	   = 3;
		
		m_phMVCs[nIndex] = 0;
        mexPrintf("init camera: %d %d %d\n",pcamera_ids[nIndex],m_pCapInfo,&m_phMVCs[nIndex]);
		rt = MV_Usb2Init("MVC1000", pcamera_ids+nIndex, m_pCapInfo, m_phMVCs+nIndex);
		if(ResSuccess != rt)
		{
            free_data(nIndex+1);
			mexPrintf("rt:%d\n",rt);
			mexErrMsgTxt("Can not open USB camera");
		}

	}
    
    for (nIndex=0;nIndex<camera_num;++nIndex){
        MV_Usb2StartCapture(m_phMVCs[nIndex],TRUE);
    }

}

void free_data(int camera_num){
    for (int i=0;i<camera_num;++i){
        MV_Usb2Uninit(&m_phMVCs[i]);
        free(m_pRawDatas[i]);
    }
	free(m_phMVCs);
    free(m_pCapInfos);

}
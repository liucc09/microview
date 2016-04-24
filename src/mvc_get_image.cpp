#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "MVCAPI.h"
#include "mex.h"

HANDLE*  m_phMVCs;
struct CapInfoStruct* m_pCapInfos;
double*  pprhs0;
int* pprhs1;
BYTE* pImage;

/***
*输入参数：相机Id(行向量)，m_phMVCs指针地址(行向量)
*输出参数：Matlab的Cell类型，每个单元存一副图像
***/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	int camera_num,image_num,i,j,h,w,num;
    int camera_ids[6];
    int image_ids[6];
    int im2ca[6] = {-1,-1,-1,-1,-1,-1 }; //图像序号转相机序号
    
    mxArray *pIms[6];
	mwSize dims[4];
	/*
     *[id   id   id
     * hMVC hMVC hMVC
     * Cap  Cap  Cap]
     */
	//相机ids、mvcMgr
	if(nrhs != 2) {
		mexErrMsgIdAndTxt("MyToolbox:mvc_get_image:nrhs",
                      "Two inputs required.");
	}
	//输出图像结构体
	if(nlhs != 1) {
		mexErrMsgIdAndTxt("MyToolbox:mvc_get_image:nlhs",
						  "One output required.");
	}
    image_num = mxGetN(prhs[0]);
    camera_num = mxGetN(prhs[1]);
    
	pprhs0 = mxGetPr(prhs[0]);
    pprhs1 = (int*)mxGetData(prhs[1]);

    for (i=0;i<camera_num;++i){
        camera_ids[i] = (int)(pprhs1[i*3]);
        //mexPrintf("camera: %d\n",camera_ids[i]);
    }
    
    for (i=0;i<image_num;++i){
        image_ids[i] = (int)(pprhs0[i]);
        //mexPrintf("image: %d\n",image_ids[i]);
    }
    
	m_phMVCs = (HANDLE*)pprhs1[1];
	m_pCapInfos = (struct CapInfoStruct*)pprhs1[2];

    for (i=0;i<image_num;++i){
        for (j=0;j<camera_num;++j){
            if (image_ids[i]==camera_ids[j]){
                MV_Usb2GetRawData(m_phMVCs[j],&m_pCapInfos[j]);
                im2ca[i] = j;
            }
        }
        if (im2ca[i]<0){
            mexErrMsgTxt("Camera id not exist");
        }
	}

	dims[0] = m_pCapInfos[0].Width;;
	dims[1] = m_pCapInfos[0].Height;;

    plhs[0] = mxCreateCellMatrix(1,image_num);
    for (i=0;i<image_num;++i){
        num = im2ca[i];
        w = m_pCapInfos[num].Width;
        h = m_pCapInfos[num].Height;
        pIms[i] = mxCreateNumericMatrix(w,h,mxUINT8_CLASS,mxREAL);
        pImage = (BYTE*)mxGetData(pIms[i]);
        memcpy(pImage,m_pCapInfos[num].Buffer,w*h);
        mxSetCell(plhs[0],i,pIms[i]);
    }
    
}
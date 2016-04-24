#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "MVCAPI.h"
#include "mex.h"

HANDLE*  m_phMVCs;
struct CapInfoStruct* m_pCapInfos;
int*	pplhs0;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	int camera_num,i;
    int camera_ids[6];
    /*
     *[id   id   id
     * hMVC hMVC hMVC
     * Cap  Cap  Cap]
     */
	//[相机id、m_phMVCs、m_pCapInfos] 列向量
	if(nrhs != 1) {
		mexErrMsgIdAndTxt("MyToolbox:mvc_uinit_camera:nrhs",
                      "One inputs required.");
	}
	//无输出
	if(nlhs != 0) {
		mexErrMsgIdAndTxt("MyToolbox:mvc_uinit_camera:nlhs",
						  "Zero output required.");
	}

	if(mxGetM(prhs[0]) != 3){
		mexErrMsgIdAndTxt("MyToolbox:mvc_uinit_camera:nlhs",
						  "Wrong Input.");
	}
    
    camera_num = mxGetN(prhs[0]);
	pplhs0 = (int*)mxGetData(prhs[0]);

	m_phMVCs = (HANDLE*)pplhs0[1];
	m_pCapInfos = (struct CapInfoStruct*)pplhs0[2];


	for (int i=0;i<camera_num;++i){
		free(m_pCapInfos[i].Buffer);
		MV_Usb2StartCapture(m_phMVCs[i],FALSE);
		MV_Usb2Uninit(&m_phMVCs[i]);
	}
	free(m_phMVCs);
	free(m_pCapInfos);
}
#pragma once
#ifndef __MS3D_XRAY_INFO_H__
#define __MS3D_XRAY_INFO_H__

#include <msPlugIn.h>

class ms3d_xray_info: public cMsPlugIn {
public:
	virtual			~ms3d_xray_info();
	virtual int		GetType();
	virtual const char*	GetTitle();
	virtual int		Execute(msModel* model);
};

#endif

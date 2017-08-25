#pragma once
#ifndef __MS3D_XRAY_EXPSKL_H__
#define __MS3D_XRAY_EXPSKL_H__

#include <msPlugIn.h>

class ms3d_xray_export_skl: public cMsPlugIn {
public:
	virtual			~ms3d_xray_export_skl();
	virtual int		GetType();
	virtual const char*	GetTitle();
	virtual int		Execute(msModel* model);
};

#endif

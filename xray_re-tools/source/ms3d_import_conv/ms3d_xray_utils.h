#pragma once
#ifndef __MS3D_XRAY_UTILS_H__
#define __MS3D_XRAY_UTILS_H__

#include <string>

struct msMaterial;

void set_texture(msMaterial* mtl, const std::string& texture);

#endif

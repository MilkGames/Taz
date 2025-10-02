#include <babel.h>

// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bdRGBToHSV
   Purpose : convert an RGB colour to HSV space
   Parameters : RGB inputs, HSV output ptrs
   Returns : 
   Info : 
*/

void bdRGBToHSV(int r, int g, int b, int *hp, int *sp, int *vp){
	// todo: real version of this

	hp = 0xaa;
	sp = 0x05;
	vp = 0x60;
}


/* --------------------------------------------------------------------------------
   Function : bdHSVToRGB
   Purpose : convert an HSV colour to RGB space
   Parameters : HSV inputs, RGB output ptrs
   Returns : 
   Info : 
*/

void bdHSVToRGB(int h, int s, int v, int *rp, int *gp, int *bp){
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int out_green;
  int out_red;
  int out_blue;
  
  if (s == 0) {
    out_blue = v;
    out_green = v;
    out_red = v;
  }
  else {
    uVar1 = (h * 360) / 60;
    iVar2 = uVar1 - (uVar1 & 0xff);
    iVar4 = uVar1 - iVar2;
    out_green = v * (0xff - s) >> 8;
    iVar3 = v * (0xff - (s * iVar4 >> 8)) >> 8;
    iVar4 = v * (0xff - (s * (0xff - iVar4) >> 8)) >> 8;
    iVar2 = iVar2 >> 8;
    out_red = out_green;
    out_blue = out_green;
    switch(iVar2) {
    case 0:
      out_red = v;
      out_green = iVar4;
      break;
    case 1:
      out_green = v;
      out_red = iVar3;
      break;
    case 2:
      out_green = v;
      out_blue = iVar4;
      break;
    case 3:
      out_blue = v;
      out_green = iVar3;
      break;
    case 4:
      out_blue = v;
      out_red = iVar4;
      break;
    case 5:
      out_red = v;
      out_blue = iVar3;
      break;
    default:
      bkAssert(62,0,$SG28396,iVar2);
      return;
    }
  }
  *rp = out_red;
  *gp = out_green;
  *bp = out_blue;
  return;
}

/* --------------------------------------------------------------------------------
   Function : bdRGBToYCbCr
   Purpose : convert an RGB colour to YCbCr space
   Parameters : RGB inputs, YCbCr output ptrs
   Returns : 
   Info : 
*/

void bdRGBToYCbCr(int r, int g, int b, int *y, int *cb, int *cr){
  *y = (r * 0x4c >> 8) + (g * 0x96 >> 8) + (b * 0x1d >> 8);
  *cb = r * 0x2b + g * -0x55 + b * 0x80 >> 8;
  *cr = r * 0x80 + g * -0x6b + b * -0x15 >> 8;
  if (*cb < 0) {
    *cb = 0;
  }
  if (0xff < *cb) {
    *cb = 0xff;
  }
  if (*cr < 0) {
    *cr = 0;
  }
  if (0xff < *cr) {
    *cr = 0xff;
  }
  return;
}


/* --------------------------------------------------------------------------------
   Function : bdYCbCrToRGB
   Purpose : convert an YCbCr colour to RGB space
   Parameters : YCbCr inputs, RGB output ptrs
   Returns : 
   Info : 
*/

void bdYCbCrToRGB(int y, int cb, int cr, int *rp, int *gp, int *bp){
  *rp = y * 0xff + cr * 0x166 >> 8;
  *gp = y * 0x11d + cb * -0x58 + cr * -0x8d >> 8;
  *bp = y * 0x66 + cb * 0x1c5 + cr * -0xd6 >> 8;
  if (0xff < *rp) {
    *rp = 0xff;
  }
  if (*gp < 0) {
    *gp = 0;
  }
  if (0xff < *gp) {
    *gp = 0xff;
  }
  if (*bp < 0) {
    *bp = 0;
  }
  if (0xff < *bp) {
    *bp = 0xff;
  }
  return;
}


/*-----------------------------------------------------------------------*/
 #include "header.h"
/*-----------------------------------------------------------------------*/
 S _zap zap;
/*-----------------------------------------------------------------------*/



 V aaMain                              (V)
 {
 _rect r1;

 while(aaZapYield(&zap))
  {
  switch(aa_stage)
   {
   case 0:
   aaStageSet(20);
   break;

   case 20:
   aaRectSet(&r1,0,0,320,240);
   aaSurfaceFill(zap.canvas.handle,&r1,&col_red[rand()%10]);
   aaZapUpdate(&zap,0);
   break;
   }
  }
 }






/*-----------------------------------------------------------------------*/
 #include "header.h"
/*-----------------------------------------------------------------------*/
 structure
 {
 H magic;
 B unq_str[65];
 B iam;
 H is_space;
 _bififile my_db;
 }
 _app;
/*-----------------------------------------------------------------------*/
 S _ezy ezy;
 S _app app;
/*-----------------------------------------------------------------------*/

/*
 structure
 {
 B magic[8];
 Q content_count;
 Q content_offset;
 Q hdr_entry_reserve;
 }
 _bifimoneyhdr;

 structure
 {
 B search_mode;
 B query[64];
 Q content_offset;
 Q content_bytes;
 }
 _bificontenthdr;




 B bifiMoneyHdrInit                    (_bifimoneyhdr*bifimoneyhdr)
 {
 aaMemoryFill(bifimoneyhdr,sizeof(_bifimoneyhdr),0);
 *(QP)&bifimoneyhdr->magic=0x21111972aa72;
 bifimoneyhdr->hdr_entry_reserve=1000LL+aaTrillion(100);
 bifimoneyhdr->content_offset=bifimoneyhdr->hdr_entry_reserve*sizeof(_bificontenthdr);
 return RET_YES;
 }


*/




 B mydbContentVarsGet                  (GP contentnum,GP hdroff,GP contentoff)
 {
 B ret;
 G num;

 if(contentoff) { *contentoff=F64; }
 if(contentnum) { *contentnum=F64; }
 if((ret=aaBifiVarNumGet(&app.my_db,"content_off",&num))==RET_NOTFOUND)
  {
  if((ret=aaBifiVarNumSet(&app.my_db,"content_off",(aaTrillion(1)*80LL)+1000LL))!=YES) { oops; }
  if((ret=aaBifiVarNumGet(&app.my_db,"content_off",&num))!=RET_YES) { oops; }
  }
 else
 if(ret!=RET_YES) { oops; }
 if(contentoff) { *contentoff=num; }


 if((ret=aaBifiVarNumGet(&app.my_db,"content_num",&num))==RET_NOTFOUND)
  {
  if((ret=aaBifiVarNumSet(&app.my_db,"content_num",0LL))!=YES) { oops; }
  if((ret=aaBifiVarNumGet(&app.my_db,"content_num",&num))!=RET_YES) { oops; }
  }
 else
 if(ret!=RET_YES) { oops; }
 if(contentnum) { *contentnum=num; }

 if((ret=aaBifiVarNumGet(&app.my_db,"hdr_off",&num))==RET_NOTFOUND)
  {
  if((ret=aaBifiVarNumSet(&app.my_db,"hdr_off",0LL))!=YES) { oops; }
  if((ret=aaBifiVarNumGet(&app.my_db,"hdr_off",&num))!=RET_YES) { oops; }
  }
 else
 if(ret!=RET_YES) { oops; }
 if(hdroff) { *hdroff=num; }
 return RET_YES;
 }

t


 B mydbContentVarsSet                  (G contentnum,G hdroff,G contentoff)
 {
 B ret;

 if((ret=aaBifiVarNumSet(&app.my_db,"content_off",contentoff))!=YES) { oops; }
 if((ret=aaBifiVarNumSet(&app.my_db,"content_num",contentnum))!=YES) { oops; }
 if((ret=aaBifiVarNumSet(&app.my_db,"hdr_off",hdroff))!=YES) { oops; }
 return RET_YES;
 }



 B mydbContentWrite                    (VP cmd,H bytes,VP data)
 {
 B ret;
 G num,hoff,off;
 B key[_8K];
 B val[_8K];
 B hdr[129];
 B qry[100];
 B mode;
 _textreader tr;
 H li,chars,pos;
 B buf[_8K];

 mode=0;
 qry[0]=NULL_CHAR;
 if((ret=aaTextReaderNew(&tr,0,cmd))!=YES) { oops; }
 for(li=0;li<tr.line_count;li++)
  {
  if((ret=aaTextReaderLineGet(&tr,li,&chars,buf))!=YES) { oops; }
  if((ret=aaStringSplitChar(buf,chars,'=',NO,YES,0,YES,key,val))!=YES) { oops; }
  if(aaStringICompare(key,"postdata",0)!=YES) { continue; }
  if(aaStringFindFirstIString(val,0,"<m:accountName>",0,&pos)==YES)
   {
   mode=1;
   }
  else
  if(aaStringFindFirstIString(val,0,"<m:otn>",0,&pos)==YES)
   {
   mode=2;
   }
  else
  if(aaStringFindFirstIString(val,0,"<m:detailId>",0,&pos)==YES)
   {
   mode=3;
   }
  if(mode==0) { oof; continue; }
  aaStringDeleteChars(val,0,0,pos);
  if(aaStringFindChar(val,0,&pos,'>',YES,0,YES)!=YES) { oof; continue; }
  aaStringDeleteChars(val,0,0,pos+1);
  if(aaStringFindChar(val,0,&pos,'<',YES,0,YES)!=YES) { oof; continue; }
  val[pos]=NULL_CHAR;
  aaStringCopy(qry,val);
  break;
  }
 aaTextReaderDelete(&tr);
 if(mode==0||qry[0]==NULL_CHAR) { oof; return RET_FAILED; }



 mydbContentVarsGet(&num,&hoff,&off);
 aaMemoryFill(hdr,sizeof(hdr),0);
 hdr[0]=mode;
 aaStringCopy(&hdr[1],qry);
 hdr[63]=0;
 *(QP)&hdr[64]=off;
 *(QP)&hdr[72]=(Q)bytes;
 if((ret=aaBifiWrite(&app.my_db,hoff,80LL,hdr))!=YES) { oops; }
 if(bytes>0LL)
  {
  if((ret=aaBifiWrite(&app.my_db,off,(Q)bytes,data))!=YES) { oops; }
  }

 num+=1L;
 hoff+=80LL;
 off+=(Q)bytes;
 mydbContentVarsSet(num,hoff,off);
 ///aaEzyLog(&ezy,2,"now %I64d %I64d",num,off);
 return RET_YES;
 }









 B appStart                            (V)
 {
 B ret;
 _size s1;
 G num;

 aaMemoryFill(&app,sizeof(_app),0);
 app.magic=aaHPP(appStart);
 aaSysUniqueGet(F32,app.unq_str);
 if(aaStringICompare(app.unq_str,"ae2f65ba8c13bf3d29e2acd4134d7f4cb7de01b69360cd61605568e5a4961b5d",0)==YES)  {  app.iam=1; }
 aaSizeSet(&s1,ezy.surface.status.max_size.w*0.5,ezy.surface.status.max_size.h*0.5);
 aaSurfaceSizeSet(ezy.surface.handle,&s1);
 if(app.iam==1)  {  aaSurfaceMonitorAlign(ezy.surface.handle,0,5,5,0,0);  }
 else            {  aaSurfaceMonitorAlign(ezy.surface.handle,0,5,5,0,0);  }
 if((ret=aaBifiNew(&app.my_db,-10000LL,2,_32MEG,5000,"mydb","%s",ezy.info.sys_path.current_dir))!=RET_YES) { oops; }


 if((ret=aaBifiVarNumGet(&app.my_db,"content_off",&num))==RET_NOTFOUND)
  {
  if((ret=aaBifiVarNumSet(&app.my_db,"content_off",(aaTrillion(1)*80LL)+1000LL))!=YES) { oops; }
  if((ret=aaBifiVarNumGet(&app.my_db,"content_off",&num))!=RET_YES) { oops; }
  }
 else
 if(ret!=RET_YES) { oops; }

 if((ret=aaBifiVarNumGet(&app.my_db,"content_num",&num))==RET_NOTFOUND)
  {
  if((ret=aaBifiVarNumSet(&app.my_db,"content_num",0))!=YES) { oops; }
  if((ret=aaBifiVarNumGet(&app.my_db,"content_num",&num))!=RET_YES) { oops; }
  }
 else
 if(ret!=RET_YES) { oops; }

 if((ret=aaBifiVarNumGet(&app.my_db,"hdr_off",&num))==RET_NOTFOUND)
  {
  if((ret=aaBifiVarNumSet(&app.my_db,"hdr_off",0))!=YES) { oops; }
  if((ret=aaBifiVarNumGet(&app.my_db,"hdr_off",&num))!=RET_YES) { oops; }
  }
 else
 if(ret!=RET_YES) { oops; }


 aaSurfaceMaximize(ezy.surface.handle,YES);
 ///aaEzyLog(&ezy,2,"money count=%I64d off=%I64d ",bimohd.content_count,bimohd.content_offset);
 return RET_YES;
 }



 B appStop                             (V)
 {
 if(app.magic!=aaHPP(appStart)) { return RET_NOTSTARTED; }
 if(app.my_db.magic) { aaBifiDelete(&app.my_db); }
 aaMemoryFill(&app,sizeof(_app),0);
 return RET_YES;
 }




 B appYield                            (V)
 {
 _str4k str;

 if(app.magic!=aaHPP(appStart)) { return RET_NOTSTARTED; }
 if(aaAboutEvery(150))
  {
  //aaEzyTitleSet(&ezy,"aa_stage=%i %I64d",aa_stage,aaMsRunning());
  }
 if(aaEzyTextRead(&ezy,&str)==YES)
  {
  aaEzyLog(&ezy,1,"%s",str.buf);
  }
 if(aaIeIsKeyDown(ezy.ie,VK_SPACE))
  {
  if(ezy.surface.handle!=0&&ezy.ie.curr->focus_handle==ezy.surface.handle)  { app.is_space++;  }
  else
  if(ezy.surface.handle==0)                                                 { app.is_space++;  }
  }

 return RET_YES;
 }


/*-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/

 structure
 {
 H magic;
 H stage;
 B is_fin;
 _threadunit thread;
 CURL*curl_handle;
 B cmd[_8K];
 VP curly_ptr;
 struct curl_slist*clist;
 _memoryunit mun;
 H retries;
 Q ms;
 Q el;
 }
 _curlycall;


 structure
 {
 H magic;
 CURLSH*share_handle;
 _sync sync;
 H max_calls;
 H cur_calls;
 H fin_calls;
 _curlycall call[40];
 H pf_call;
 }
 _curly;

 V curlyCallBackLock                   (CURL*handle,curl_lock_data data,curl_lock_access access,VP userptr);
 V curlyCallBackUnLock                 (CURL*handle,curl_lock_data data,VP userptr);
 Z curlyCallBackTrace                  (CURL*handle,curl_infotype type,CP data,Y size,VP ud);
 Y curlyCallBackWrite                  (CP contents,Y size,Y nmemb,VP ud);
 B curlyCallThread                     (H handle);

 B curlyNew                            (_curly*curly);
 B curlyDelete                         (_curly*curly);
 B curlyYield                          (_curly*curly);
 B curlyCallNew                        (_curly*curly,HP index,VP fmt,...);
 B curlyCallDelete                     (_curly*curly,H index);

/*-----------------------------------------------------------------------*/



 V curlyCallBackLock                   (CURL*handle,curl_lock_data data,curl_lock_access access,VP userptr)
 {
 _curly*curly;
 H ci;
 curly=userptr;

 if(curly->magic!=aaHPP(curlyNew)) { oof; }
 for(ci=0;ci<curly->max_calls;ci++)
  {
  if(curly->call[ci].magic==0) { continue; }
  if(curly->call[ci].curl_handle==handle) {  break;   }
  }
 if(ci==curly->max_calls) { return; }
 UNUSE(access);
 UNUSE(data);
 while(aaSyncLock(&curly->sync)!=RET_YES);
 }




 V curlyCallBackUnLock                 (CURL*handle,curl_lock_data data,VP userptr)
 {
 _curly*curly;
 H ci;
 curly=userptr;

 if(curly->magic!=aaHPP(curlyNew)) { oof; }
 for(ci=0;ci<curly->max_calls;ci++)
  {
  if(curly->call[ci].magic==0) { continue; }
  if(curly->call[ci].curl_handle==handle) {  break;   }
  }
 if(ci==curly->max_calls) { return; }
 UNUSE(data);
 aaSyncUnlock(&curly->sync,0,0);
 }



 Z curlyCallBackTrace                  (CURL*handle,curl_infotype type,CP data,Y size,VP ud)
 {
 H ci;
 _curly*curly;

 curly=ud;
 for(ci=0;ci<curly->max_calls;ci++)
  {
  if(curly->call[ci].magic==0) { continue; }
  if(curly->call[ci].curl_handle==handle) {  break;   }
  }
 if(ci==curly->max_calls) { return 0; }
 UNUSE(data);
 UNUSE(size);
 switch(type)
  {
  default:
  return 0;

  case CURLINFO_TEXT:
  case CURLINFO_SSL_DATA_OUT:
  case CURLINFO_SSL_DATA_IN:
  break;

  case CURLINFO_HEADER_IN:
  case CURLINFO_HEADER_OUT:
  case CURLINFO_DATA_OUT:
  case CURLINFO_DATA_IN:
  break;
  }
 return 0;
 }



 Y curlyCallBackWrite                  (CP contents,Y size,Y nmemb,VP ud)
 {
 B ret;
 H left,realsize,ci;
 _curlycall*ccall;
 _curly*curly;

 ccall=(_curlycall*)ud;
 curly=(_curly*)ccall->curly_ptr;
 for(ci=0;ci<curly->max_calls;ci++)
  {
  if(curly->call[ci].magic==0) { continue; }
  if(curly->call[ci].thread.handle==ccall->thread.handle) {  break;   }
  }
 if(ci==curly->max_calls) { oof; }
 realsize=size*nmemb;
 if(ccall->mun.bytes==0)
  {
  if((ret=aaMemoryUnitAllocate(&ccall->mun,realsize+_8K))!=YES) { oops; }
  }
 left=ccall->mun.bytes-ccall->mun.used;
 if((realsize+_4K)>=left)
  {
  if((ret=aaMemoryUnitReAllocate(&ccall->mun,ccall->mun.bytes+realsize+_32K))!=YES) { oops; }
  }
 aaMemoryCopy(&ccall->mun.mem[ccall->mun.used],realsize,contents);
 ccall->mun.used+=realsize;
 ccall->mun.mem[ccall->mun.used]=NULL_CHAR;
 return realsize;
 }






 B curlyCallThread                     (H handle)
 {
 B ret;
 _threadunit tu;
 _curly*curly;
 H ci,li,chars;
 B buf[_8K];
 _textreader tr;
 B etc[_1K];
 B use_socks;
 H socks_port;
 B is_post;
 B url[_1K];
 B post_data[_8K];
 B content_type[_1K];
 B key[_8K];
 B val[_8K];
 CURLcode cres;

 tu.handle=handle;
 if((ret=aaThreadStatus(tu.handle,&tu.status))!=RET_YES) { oops; }
 curly=(_curly*)tu.status.data;
 for(ci=0;ci<curly->max_calls;ci++)
  {
  if(curly->call[ci].magic==0) { continue; }
  if(curly->call[ci].thread.handle==handle) {  break;   }
  }
 if(ci==curly->max_calls) { oof; }
 is_post=NO;
 use_socks=NO;
 socks_port=0;
 aaStringNull(url);
 aaStringNull(post_data);
 aaStringNull(content_type);
 if((ret=aaTextReaderNew(&tr,0,curly->call[ci].cmd))!=YES) { oops; }
 for(li=0;li<tr.line_count;li++)
  {
  if((ret=aaTextReaderLineGet(&tr,li,&chars,buf))!=YES) { oops; }
  if((ret=aaStringSplitChar(buf,chars,'=',NO,YES,0,YES,key,val))!=YES) { oops; }
  if(aaStringICompare(key,"meth",0)==YES)
   {
   if(aaStringICompare(val,"post",0)==YES) { is_post=YES; }    else
   if(aaStringICompare(val,"get",0)==YES) { is_post=NO; }
   continue;
   }
  if(aaStringICompare(key,"socks5",0)==YES)
   {
   aaStringToNumber(val,0,&socks_port,NULL,NULL,NULL);
   socks_port=8000+ci;
   use_socks=YES;
   continue;
   }
  if(aaStringICompare(key,"url",0)==YES)
   {
   aaStringCopy(url,val);
   continue;
   }
  if(aaStringICompare(key,"contenttype",0)==YES)
   {
   aaStringCopy(content_type,val);
   continue;
   }
  if(aaStringICompare(key,"postdata",0)==YES)
   {
   aaStringCopy(post_data,val);
   continue;
   }
  aaDebugf("%i=%i/%i %-5i [%s=%s]",ci,li,tr.line_count,chars,key,val);
  }
 aaTextReaderDelete(&tr);
 curly->call[ci].curl_handle=curl_easy_init();
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_URL,url);
 if(is_post)
  {
  curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_POSTFIELDS,post_data);
  curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_POSTFIELDSIZE,(long)strlen((CP)post_data));
  }
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_SSL_VERIFYPEER,0);
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_SSL_VERIFYHOST,0);
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_COOKIEJAR,"cookie.txt");
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_COOKIEFILE,"cookie.txt");
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_DEBUGFUNCTION,curlyCallBackTrace);
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_DEBUGDATA,(VP)curly);
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_VERBOSE,1);
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_TIMEOUT_MS,10000);
 if(use_socks)
  {
  aaStringCopyf(etc,"socks5://127.0.0.1:%u",socks_port);
  curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_PROXY,etc);
  }
 if(content_type[0])
  {
  aaStringCopyf(etc,"Content-Type: %s",content_type);
  curly->call[ci].clist=curl_slist_append(curly->call[ci].clist,(CP)etc);
  curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_HTTPHEADER,curly->call[ci].clist);
  }
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_WRITEFUNCTION,curlyCallBackWrite);
 curl_easy_setopt(curly->call[ci].curl_handle,CURLOPT_WRITEDATA,(VP)&curly->call[ci]);
 cres=curl_easy_perform(curly->call[ci].curl_handle);
 if(cres!=CURLE_OK)  { ret=RET_FAILED; aaEzyLog(&ezy,2,"curl_easy_perform() failed: %s\n",curl_easy_strerror(cres)); }
 else                { ret=RET_YES; }
 curl_slist_free_all(curly->call[ci].clist);
 curl_easy_cleanup(curly->call[ci].curl_handle);
 curly->call[ci].is_fin=YES;
 curly->fin_calls++;
 return RET_YES;
 }





 B curlyNew                            (_curly*curly)
 {
 H retries;

 if(curly==NULL) { return RET_MISSINGPARM; }
 aaMemoryFill(curly,sizeof(_curly),0);
 curly->magic=aaHPP(curlyNew);
 curl_global_init(CURL_GLOBAL_ALL);
 curly->share_handle=curl_share_init();
 curl_share_setopt(curly->share_handle,CURLSHOPT_LOCKFUNC,curlyCallBackLock);
 curl_share_setopt(curly->share_handle,CURLSHOPT_USERDATA,curly);
 curl_share_setopt(curly->share_handle,CURLSHOPT_UNLOCKFUNC,curlyCallBackUnLock);
 curl_share_setopt(curly->share_handle,CURLSHOPT_SHARE,CURL_LOCK_DATA_CONNECT);
 aaSyncNew(&curly->sync,1,1,"connlock");
 curly->max_calls=aaElementCount(curly->call);
 curly->cur_calls=0;
 return RET_YES;
 }




 B curlyDelete                         (_curly*curly)
 {
 H ci;

 if(curly==NULL) { return RET_MISSINGPARM; }
 if(curly->magic!=aaHPP(curlyNew)) { return RET_NOTINITIALIZED; }
 for(ci=0;ci<curly->max_calls;ci++)
  {
  curlyCallDelete(curly,ci);
  }
 curl_share_cleanup(curly->share_handle);
 curl_global_cleanup();
 //while(aaSyncLock(&curly->sync)!=RET_YES);
 aaSyncDelete(&curly->sync);
 aaMemoryFill(curly,sizeof(_curly),0);
 return RET_YES;
 }




 B curlyYield                          (_curly*curly)
 {
 H ci;
 _curlycall*ccall;

 if(curly==NULL) { return RET_MISSINGPARM; }
 if(curly->magic!=aaHPP(curlyNew)) { return RET_NOTINITIALIZED; }
 for(ci=0;ci<curly->max_calls;ci++)
  {
  curly->pf_call++;
  curly->pf_call%=curly->max_calls;
  ccall=&curly->call[curly->pf_call];
  if(ccall->magic==0) { continue; }
  if(ccall->thread.handle!=0)
   {
   aaThreadStatus(ccall->thread.handle,&ccall->thread.status);
   }
  ccall->el=aaMsRunning()-ccall->ms;
  break;
  }
 return RET_YES;
 }





 B curlyCallNew                        (_curly*curly,HP index,VP fmt,...)
 {
 B ret;
 H ci;

 if(curly==NULL) { return RET_MISSINGPARM; }
 if(curly->magic!=aaHPP(curlyNew)) { return RET_NOTINITIALIZED; }
 aaVargsf8K(fmt);
 if(index) { *index=0; }
 for(ci=0;ci<curly->max_calls;ci++)
  {
  if(curly->call[ci].magic==0)  { break; }
  }
 if(ci==curly->max_calls) { return RET_FAILED; }
 aaMemoryFill(&curly->call[ci],sizeof(_curlycall),0);
 curly->call[ci].magic=aaHPP(curlyCallNew);
 curly->call[ci].stage=0;
 curly->call[ci].curly_ptr=curly;
 curly->call[ci].ms=aaMsRunning();
 aaStringNCopy(curly->call[ci].cmd,str8k.buf,str8k.len,YES);
 if((ret=aaThreadCreateRelayed(&curly->call[ci].thread.handle,curlyCallThread,(H)curly,0,NULL,YES-1))!=YES) { oops; }
 if((ret=aaThreadStatus(curly->call[ci].thread.handle,&curly->call[ci].thread.status))!=YES) { oops; }
 curly->cur_calls++;
 if(index) { *index=ci; }
 return RET_YES;
 }




 B curlyCallDelete                     (_curly*curly,H index)
 {
 H ci;

 if(curly==NULL) { return RET_MISSINGPARM; }
 if(curly->magic!=aaHPP(curlyNew)) { return RET_NOTINITIALIZED; }
 ci=index;
 if(ci>=curly->max_calls) { return RET_BADHANDLE; }
 if(curly->call[ci].magic==0) { return RET_BADHANDLE; }
 if(curly->call[ci].is_fin)
  {
  curly->fin_calls--;
  }
 if(curly->call[ci].thread.handle!=0)
  {
  if(curly->call[ci].is_fin!=YES)
   {
   while(aaSyncLock(&curly->sync)!=RET_YES);
   aaThreadExit(curly->call[ci].thread.handle);
   }
  aaThreadDestroy(curly->call[ci].thread.handle);
  curly->call[ci].thread.handle=0;
  }
 if(curly->call[ci].is_fin!=YES)
  {
  curl_easy_cleanup(curly->call[ci].curl_handle);
  }
 curly->call[ci].curl_handle=0;
 if(curly->call[ci].mun.bytes)
  {
  aaMemoryUnitRelease(&curly->call[ci].mun);
  }
 aaMemoryFill(&curly->call[ci],sizeof(_curlycall),0);
 curly->cur_calls--;
 return RET_YES;
 }







/*-----------------------------------------------------------------------*/


 structure
 {
 B search_mode;         // 1=name,2=otn,3=details
 B query[65];           // name,otn,detailid
 H number;
 H field_bit;
 B hit_count[17];                      // 0
 B transfer_date[17];                  // 1
 B account_name[65];                   // 2
 B amount[17];                         // 3
 B address[129];                       // 4
 B address1[129];                      // 5
 B address2[129];                      // 6
 B owner_name[129];                    // 7
 B owner_surname[129];                 // 8
 B owner_given_name[65];               // 9
 B detail_id[33];                      // 10
 B related_institution[129];           // 11
 B institution_name[129];              // 12
 B section[33];                        // 13
 B otn[33];                            // 14
 B surname[33];                        // 15
 B given_name[65];                     // 16
 B money_type[33];                     // 17
 B status[33];                         // 18
 B return_year[17];                    // 19
 B shares[17];                         // 20
 B date_interest_paid_from[17];        // 21
 B policy_number[33];                  // 22
 B policy_due_date[17];                // 23
 B address3[129];                      // 24
 B address4[129];                      // 25
 }
 _money;







 V moneyProc                           (_money*money)
 {
 B ret;
 B out[_16K];
 H num,go;
 B hash[129];

 if(money->search_mode<1||money->search_mode>3) { oof; }
 aaStringNull(out);
 aaStringAppendf(out,"---------------\n");
               switch(money->search_mode)
                {
                case 1: aaStringAppendf(out,"            search_mode=%d NAME %s\n",money->search_mode,money->query); break;
                case 2: aaStringAppendf(out,"            search_mode=%d OTN %s\n",money->search_mode,money->query); break;
                case 3: aaStringAppendf(out,"            search_mode=%d DETAILS %s\n",money->search_mode,money->query); break;
                default: aaNote(0,"??? %i",money->search_mode);   aaStringAppendf(out,"search_mode=%d ???????????????????????????????\n",money->search_mode);    break;
                }
               //aaStringAppendf(out,"                  query=%s\n",money->query);
               aaStringAppendf(out,"                 number=%d\n",money->number);
               aaStringAppendf(out,"              field_bit=%i\n",money->field_bit);

               if(aaBitGet(money->field_bit, 0)) aaStringAppendf(out,"              hit_count=%s\n",money->hit_count);
               if(aaBitGet(money->field_bit, 1)) aaStringAppendf(out,"          transfer_date=%s\n",money->transfer_date);
               if(aaBitGet(money->field_bit, 2)) aaStringAppendf(out,"           account_name=%s\n",money->account_name);
               if(aaBitGet(money->field_bit, 3)) aaStringAppendf(out,"                 amount=%s\n",money->amount);
               if(aaBitGet(money->field_bit, 4)) aaStringAppendf(out,"                address=%s\n",money->address);
               if(aaBitGet(money->field_bit, 5)) aaStringAppendf(out,"               address1=%s\n",money->address1);
               if(aaBitGet(money->field_bit, 6)) aaStringAppendf(out,"               address2=%s\n",money->address2);
               if(aaBitGet(money->field_bit,24)) aaStringAppendf(out,"               address3=%s\n",money->address3);
               if(aaBitGet(money->field_bit,25)) aaStringAppendf(out,"               address4=%s\n",money->address4);
               if(aaBitGet(money->field_bit, 7)) aaStringAppendf(out,"             owner_name=%s\n",money->owner_name);
               if(aaBitGet(money->field_bit, 8)) aaStringAppendf(out,"          owner_surname=%s\n",money->owner_surname);
               if(aaBitGet(money->field_bit, 9)) aaStringAppendf(out,"       owner_given_name=%s\n",money->owner_given_name);
               if(aaBitGet(money->field_bit,10)) aaStringAppendf(out,"              detail_id=%s\n",money->detail_id);
               if(aaBitGet(money->field_bit,11)) aaStringAppendf(out,"    related_institution=%s\n",money->related_institution);
               if(aaBitGet(money->field_bit,12)) aaStringAppendf(out,"       institution_name=%s\n",money->institution_name);
               if(aaBitGet(money->field_bit,13)) aaStringAppendf(out,"                section=%s\n",money->section);
               if(aaBitGet(money->field_bit,14)) aaStringAppendf(out,"                    otn=%s\n",money->otn);
               if(aaBitGet(money->field_bit,15)) aaStringAppendf(out,"                surname=%s\n",money->surname);
               if(aaBitGet(money->field_bit,16)) aaStringAppendf(out,"             given_name=%s\n",money->given_name);
               if(aaBitGet(money->field_bit,17)) aaStringAppendf(out,"             money_type=%s\n",money->money_type);
               if(aaBitGet(money->field_bit,18)) aaStringAppendf(out,"                 status=%s\n",money->status);
               if(aaBitGet(money->field_bit,19)) aaStringAppendf(out,"            return_year=%s\n",money->return_year);
               if(aaBitGet(money->field_bit,20)) aaStringAppendf(out,"                 shares=%s\n",money->shares);
               if(aaBitGet(money->field_bit,21)) aaStringAppendf(out,"date_interest_paid_from=%s\n",money->date_interest_paid_from);
               if(aaBitGet(money->field_bit,22)) aaStringAppendf(out,"          policy_number=%s\n",money->policy_number);
               if(aaBitGet(money->field_bit,23)) aaStringAppendf(out,"        policy_due_date=%s\n",money->policy_due_date);
               /*
               if(money->field_bit>3)
                {
                aaEzyLog(&ezy,2,"%s",out);
                non_empty_added++;
                }
               */
 aaLog(-555,"%s",out);
 }







 B moneyDecode                         (VP query,_memoryunit*mun,HP moneycount,_money**moneyarray)
 {
 B ret;
 _textreader tr;
 H li,chars,pos,i,line,state;
 B text[_8K];
 B cmd[_1K];
 N which;
 N of_which;
 B of_text[_1K];
 B mode;
 B qry[_1K];
 B buf[_8K];
 B key[_8K];
 B val[_8K];
 _money tmoney;
 _money*money;
 _htmlparser html;
 _htmlrorg rorg;
 H number;
 _money*money_array;
// _bifimoneyhdr bimohe;
 //_bificontenthdr bicohd;


 H money_array_count=0;
 H money_array_length=0;

 if(moneyarray) { *moneyarray=0; }
 if(moneycount) { *moneycount=0; }
 mode=0;
 qry[0]=NULL_CHAR;
 if((ret=aaTextReaderNew(&tr,0,query))!=YES) { oops; }
 for(li=0;li<tr.line_count;li++)
  {
  if((ret=aaTextReaderLineGet(&tr,li,&chars,buf))!=YES) { oops; }
  if((ret=aaStringSplitChar(buf,chars,'=',NO,YES,0,YES,key,val))!=YES) { oops; }
  if(aaStringICompare(key,"postdata",0)!=YES) { continue; }
  if(aaStringFindFirstIString(val,0,"<m:accountName>",0,&pos)==YES)
   {
   mode=1;
   }
  else
  if(aaStringFindFirstIString(val,0,"<m:otn>",0,&pos)==YES)
   {
   mode=2;
   }
  else
  if(aaStringFindFirstIString(val,0,"<m:detailId>",0,&pos)==YES)
   {
   mode=3;
   }
  if(mode==0) { oof; continue; }
  aaStringDeleteChars(val,0,0,pos);
  if(aaStringFindChar(val,0,&pos,'>',YES,0,YES)!=YES) { oof; continue; }
  aaStringDeleteChars(val,0,0,pos+1);
  if(aaStringFindChar(val,0,&pos,'<',YES,0,YES)!=YES) { oof; continue; }
  val[pos]=NULL_CHAR;
  aaStringCopy(qry,val);
  break;
  }
 aaTextReaderDelete(&tr);
 if(mode==0||qry[0]==NULL_CHAR) { oof; return RET_FAILED; }


 #if 0
 if(aaBifiRead(&app.fifi,0LL,sizeof(_bifimoneyhdr),&bimohe)!=YES) { oof; }
 aaMemoryFill(&bicohd,sizeof(_bificontenthdr),0);
 bicohd.search_mode=mode;
 aaStringCopy(bicohd.query,qry);
 bicohd.content_offset=bimohe.content_offset;
 bicohd.content_bytes=(Q)mun->used;
 if(aaBifiWrite(&app.fifi,1000LL+(bimohe.content_count*sizeof(_bificontenthdr)),sizeof(_bificontenthdr),&bicohd)!=YES) { oof; }
 if(aaBifiWrite(&app.fifi,bicohd.content_offset,(Q)mun->used,mun->mem)!=YES) { oof; }

 bimohe.content_offset+=(Q)mun->used;
 bimohe.content_count++;
 if(aaBifiWrite(&app.fifi,0LL,sizeof(_bifimoneyhdr),&bimohe)!=YES) { oof; }
 #endif

 /*
 aaMemoryFill(bificontenthdr,sizeof(_bificontenthdr),0);
 bificontenthdr->search_mode=smode;
 aaStringCopy(bificontenthdr->query,query);
 bificontenthdr->content_offset=bifimoneyhdr->content_offset;
 bificontenthdr->content_bytes=bytes;
 bifimoneyhdr->content_offset+=bytes;
 bifimoneyhdr->content_bytes+=bytes;
 bifimoneyhdr->content_count++;
 */



 if(mun->used>10)
  {



  /**
  aaMemoryFill(&bimohe,sizeof(bimohe),0);
  aaStringCopy(bimohe.query,qry);
  bimohe.search_mode=mode;
  bimohe.response_bifi_offset=(Q)searchfof;
  bimohe.response_bifi_bytes=(Q)mun->used+1;
  if((ret=aaBifiWrite(&app.fifi,searchnum*sizeof(bimohe),(Q)sizeof(bimohe),&bimohe))!=YES) { oops; }
  if((ret=aaBifiWrite(&app.fifi,bimohe.response_bifi_offset,bimohe.response_bifi_bytes,mun->mem))!=YES) { oops; }
  searchfof+=bimohe.response_bifi_bytes+1LL;
  searchnum+=1LL;
  if((ret=aaBifiVarNumSet(&app.fifi,"searchfof",searchfof))!=YES) { oops; }
  if((ret=aaBifiVarNumSet(&app.fifi,"searchnum",searchnum))!=YES) { oops; }
  aaEzyLog(&ezy,2,"search num=%I64d fof=%I64d used=%i",searchnum,searchfof,mun->used);
  */
  }
 return RET_YES;






 aaMemoryFill(&tmoney,sizeof(_money),0);
 money=(_money*)&tmoney;
 money->search_mode=mode;
 aaStringCopy(money->query,qry);


 number=0;
 if((ret=aaHtmlParserEzy(&html,mun->used,mun->mem))!=RET_YES) { oops; }
 i=0;
 line=0;
 state=0;
 while(1)
  {
  if(i>=html.row_count) { break; }
  if(aaHtmlParserReadEx(&html,i,&rorg)!=YES) { oof; }
  if(rorg.row.type!=1&&rorg.row.type!=4)
   {
   aaHtmlParserDelete(&html);
   if(money_array_length) { aaMemoryRelease(money_array); }
   money_array=NULL;
   money_array_length=0;
   if(moneyarray) { *moneyarray=0; }
   if(moneycount) { *moneycount=0; }
   return RET_FAILED;

   //aaNote(0,"ee %-4i/%-4i %-2i %-3i %s",i,html.row_count,rorg.row.type,rorg.row.flag,rorg.ptr); i++; break;
   }
  if(rorg.row.chars>_4K) { aaNote(0,"gg %i",rorg.row.chars); break; }
  aaStringNCopy(text,rorg.ptr,rorg.row.chars,YES);
  if(rorg.row.type==HTML_ROW_TYPE_TEXT)
   {
   which=-2;
   aaStringReplaceString(text,0,"&lt;",0,"<",0,YES,text);
   aaStringReplaceString(text,0,"&gt;",0,">",0,YES,text);
   aaStringReplaceString(text,0,"&amp;",0,">",0,YES,text);
   //aaEzyLog(&ezy,2,"%-4i/%-4i %-2i %-3i TEXT: %s",i,html.row_count,rorg.row.type,rorg.row.flag,text);
   }
  else
   {
   which=-1;
   while(1)
    {
    aaStringIsIString(text,&which,"<ns2:UnclaimedBasic>","<ns2:accountName>","<ns2:amount>","<ns2:hitCount>","<ns2:transferDate>",NULL);
    if(which!=-1) { which+=1000; break; }
    aaStringIsIString(text,&which,"<ns2:ownerName>","<ns2:address>","<ns2:status>","<ns2:detailID>","<ns2:relatedInstitution>",NULL);
    if(which!=-1) { which+=1100; break; }
    break;
    }
   while(1)
    {
    if(which!=-1) { break; }
    aaStringIsIString(text,&which,"<ns2:otn>","<ns2:surname>","<ns2:givenName>","<ns2:ownerSurname>","<ns2:ownerGivenName>","<ns2:policyNumber>",NULL);
    if(which!=-1) { which+=2000; break; }
    aaStringIsIString(text,&which,"<ns2:policyDueDate>","<ns2:address1>","<ns2:address2>","<ns2:moneyType>",NULL);
    if(which!=-1) { which+=2100; break; }
    aaStringIsIString(text,&which,"<ns2:institutionName>","<ns2:section>","<ns2:returnYear>","<ns2:shares>","<ns2:dateInterestPaidFrom>","<ns2:address3>","<ns2:address4>",NULL);
    if(which!=-1) { which+=2200; break; }
    break;
    }
   while(1)
    {
    if(which!=-1) { break; }
    aaStringIsIString(text,&which,"<ns2:ownerName/>",NULL);
    if(which!=-1) { which+=4000; break; }
    break;
    }
   while(1)
    {
    if(which!=-1) { break; }
    aaStringIsIString(text,&which,"</ns2:UnclaimedBasic>","</ns2:accountName>","</ns2:amount>","</ns2:hitCount>","</ns2:transferDate>",NULL);
    if(which!=-1) { which+=5000; break; }
    aaStringIsIString(text,&which,"</ns2:ownerName>","</ns2:address>","</ns2:status>","</ns2:detailID>","</ns2:relatedInstitution>",NULL);
    if(which!=-1) { which+=5100; break; }
    break;
    }
   while(1)
    {
    if(which!=-1) { break; }
    aaStringIsIString(text,&which,"</ns2:otn>","</ns2:surname>","</ns2:givenName>","</ns2:ownerSurname>","</ns2:ownerGivenName>","</ns2:policyNumber>",NULL);
    if(which!=-1) { which+=6000; break; }
    aaStringIsIString(text,&which,"</ns2:policyDueDate>","</ns2:address1>","</ns2:address2>","</ns2:moneyType>",NULL);
    if(which!=-1) { which+=6100; break; }
    aaStringIsIString(text,&which,"</ns2:institutionName>","</ns2:section>","</ns2:returnYear>","</ns2:shares>","</ns2:dateInterestPaidFrom>","</ns2:address3>","</ns2:address4>",NULL);
    if(which!=-1) { which+=6200; break; }
    break;
    }

   while(1)
    {
    if(which!=-1) { break; }
    if(aaStringNICompare(text,"<SOAP-ENV:",F32,0)==YES) { which=-3; break; }
    if(aaStringNICompare(text,"</SOAP-ENV:",F32,0)==YES) { which=-3; break; }
    if(aaStringNICompare(text,"<ns2:simpleQueryResponse",F32,0)==YES) { which=-3; break; }
    if(aaStringNICompare(text,"</ns2:simpleQueryResponse",F32,0)==YES) { which=-3; break; }
    if(aaStringNICompare(text,"<ns2:UnclaimedDetailResponse",F32,0)==YES) { which=-3; break; }
    if(aaStringNICompare(text,"</ns2:UnclaimedDetailResponse",F32,0)==YES) { which=-3; break; }
    ///aaEzyLog(&ezy,2,"?????? %i %s",i,text);
    break;
    }
   }

  //if(which==4000) {  aaEzyLog(&ezy,2,"     %s",text); } // self closing
  //aaEzyLog(&ezy,2,"  %-4i   %s",which,text);

  if(which>=1000)//&&which<5000)
   {
   if(aaStringFindChar(text,0,&pos,':',YES,0,YES)==YES)
    {
    aaStringDeleteChars(text,0,0,pos+1);
    }
   aaStringLastCharSet(text,0,0,1);
   if(which==4000) { aaStringLastCharSet(text,0,0,1); }
   }

  switch(state)
   {
   case 0:
   if(which==-3) { break; }
   if(which==1000)
    {
    if(money->field_bit!=0)
     {
     if(money_array_length==0)
      {
      money_array_length=10;
      if((ret=aaMemoryAllocate((VP)&money_array,money_array_length*sizeof(_money)))!=YES) { oops; }
      }
     if((money_array_length-money_array_count)<2)
      {
      money_array_length=(money_array_length/2)+5;
      if((ret=aaMemoryReAllocate((VP)&money_array,money_array_length*sizeof(_money)))!=YES) { oops; }
      }
     aaMemoryCopy(&money_array[money_array_count],sizeof(_money),money);
     money_array_count++;

    // proc(money);
     }
    number++;
    line=0;
    aaMemoryFill(money,sizeof(_money),0);
    money->search_mode=mode;
    aaStringCopyf(money->query,"%s",qry);
    money->number=number;
    break;
    }
   if(which==5000) { break; }
   if(which==4000) { break; }
   if(which<4000)
    {
    aaStringCopyf(of_text,"%30s ",text);
    of_which=which;
    state=10;
    break;
    }
   aaEzyLog(&ezy,2,"state=%i      %s",state,text);
   break;


   case 10:
   if(which>=5000)
    {
    if((which-4000)!=of_which) { oof; }
    state=0;
    break;
    }

   if(which!=-2) { aaEzyLog(&ezy,2,"state=%i      %s",state,text); state=0; }

        switch(of_which)
         {
         default:
//          aaEzyLog(&ezy,2,"%-3i ofwhich=%i %s %s",line,of_which,of_text,text);
         break;

         case 1003:         aaStringCopy(money->hit_count,text);              money->field_bit=aaBitSet(money->field_bit,0);         break;
         case 1004:         aaStringCopy(money->transfer_date,text);          money->field_bit=aaBitSet(money->field_bit,1);         break;
         case 1001:         aaStringCopy(money->account_name,text);           money->field_bit=aaBitSet(money->field_bit,2);         break;
         case 1002:         aaStringCopy(money->amount,text);                 money->field_bit=aaBitSet(money->field_bit,3);         break;
         case 1101:         aaStringCopy(money->address,text);                money->field_bit=aaBitSet(money->field_bit,4);         break;
         case 2101:         aaStringCopy(money->address1,text);               money->field_bit=aaBitSet(money->field_bit,5);         break;
         case 2102:         aaStringCopy(money->address2,text);               money->field_bit=aaBitSet(money->field_bit,6);         break;
         case 1100:         aaStringCopy(money->owner_name,text);             money->field_bit=aaBitSet(money->field_bit,7);         break;
         case 2003:         aaStringCopy(money->owner_surname,text);          money->field_bit=aaBitSet(money->field_bit,8);         break;
         case 2004:         aaStringCopy(money->owner_given_name,text);       money->field_bit=aaBitSet(money->field_bit,9);         break;
         case 1103:         aaStringCopy(money->detail_id,text);              money->field_bit=aaBitSet(money->field_bit,10);         break;
         case 1104:         aaStringCopy(money->related_institution,text);    money->field_bit=aaBitSet(money->field_bit,11);         break;
         case 2200:         aaStringCopy(money->institution_name,text);       money->field_bit=aaBitSet(money->field_bit,12);         break;
         case 2201:         aaStringCopy(money->section,text);                money->field_bit=aaBitSet(money->field_bit,13);         break;
         case 2000:         aaStringCopy(money->otn,text);                    money->field_bit=aaBitSet(money->field_bit,14);         break;
         case 2001:         aaStringCopy(money->surname,text);                money->field_bit=aaBitSet(money->field_bit,15);         break;
         case 2002:         aaStringCopy(money->given_name,text);             money->field_bit=aaBitSet(money->field_bit,16);         break;
         case 2103:         aaStringCopy(money->money_type,text);             money->field_bit=aaBitSet(money->field_bit,17);         break;
         case 1102:         aaStringCopy(money->status,text);                 money->field_bit=aaBitSet(money->field_bit,18);         break;
         case 2202:         aaStringCopy(money->return_year,text);            money->field_bit=aaBitSet(money->field_bit,19);         break;
         case 2203:         aaStringCopy(money->shares,text);                 money->field_bit=aaBitSet(money->field_bit,20);         break;
         case 2204:         aaStringCopy(money->date_interest_paid_from,text);    money->field_bit=aaBitSet(money->field_bit,21);         break;
         case 2005:         aaStringCopy(money->policy_number,text);    money->field_bit=aaBitSet(money->field_bit,22);         break;
         case 2100:         aaStringCopy(money->policy_due_date,text);    money->field_bit=aaBitSet(money->field_bit,23);         break;
         case 2205:         aaStringCopy(money->address3,text);    money->field_bit=aaBitSet(money->field_bit,24);         break;
         case 2206:         aaStringCopy(money->address4,text);    money->field_bit=aaBitSet(money->field_bit,25);         break;
         }
   line++;
   state=20;
   break;

   case 20:
   if(which<5000)
    {
    aaEzyLog(&ezy,2,"kk %i %s",which,text);;
    aaDebugf("kk %i %s",which,text);;
    break;
    }
   state=0;
   break;
   }
  i++;
  }

 aaHtmlParserDelete(&html);
 if(money->field_bit!=0)
  {
  if(money_array_length==0)
   {
   money_array_length=10;
   if((ret=aaMemoryAllocate((VP)&money_array,money_array_length*sizeof(_money)))!=YES) { oops; }
   }
  if((money_array_length-money_array_count)<2)
   {
   money_array_length=(money_array_length/2)+5;
   if((ret=aaMemoryReAllocate((VP)&money_array,money_array_length*sizeof(_money)))!=YES) { oops; }
   }
  aaMemoryCopy(&money_array[money_array_count],sizeof(_money),money);
  money_array_count++;
  //proc(money);
  }

 if(moneycount) { *moneycount=money_array_count; }
 if(moneyarray) { *moneyarray=money_array; }


 return RET_YES;
 }




/*-----------------------------------------------------------------------*/




 _curly kerl;
 B booboo[_2MEG];


 V aaMain                              (V)
 {
 B ret;
 H ci;
 Q age;
 H bytes;
 H i,pf,ti,m;
 _tor tor;
 _money*money_array;
 H money_count;
 H mused,mavail;
 G cnum,coff,hoff;
 Q roff,rlen;
 B tmp[100];
//  _bifimoneyhdr bimohe;
 // _bificontenthdr bicohd;
  Q bb;
 Q did=101000;

 kerl.magic=0;
 while(aaEzyYield(&ezy))
  {
  if(aa_stage==0)
   {
   if(appStart()!=YES) { oof; }
   aaStageSet(100);
   }

  switch(aa_stage)
   {
   case 100:
  aaStageSet(500);
  aaStageSet(110);
   break;

   case 110:
   aaEzyLog(&ezy,2,"checking tor");
   if((ret=aaTorNew(&tor,"c:/fuckoff"))!=YES) { oops; }
   for(i=0;i<40;i++)
    {
    ret=aaTorFind(&tor,7000+i,8000+i,&ti,&age);
    if(ret==RET_YES)
     {
     if(age<600) { continue; }
     if(aaTorKill(&tor,ti)!=YES) { oof; }
     aaEzyLog(&ezy,2,"tor[%i]=%I64d killing",i,age);
     }
    ret=aaTorLaunch(&tor,7000+i,8000+i);
    aaEzyLog(&ezy,2,"? %i=%s",i,arets);
    }
   aaTorDelete(&tor);
   aaEzyLog(&ezy,2,"tor check completed");
   //if(app.is_space==3)
   app.is_space=0;
   aaStageSet(200);
   break;



   case 200:
   if(kerl.magic==0) curlyNew(&kerl);
   aaStageSet(220);
   break;

   case 220:
   if(app.is_space==0)
    {
    for(i=0;i<40;i++)
     {
     //curlyCallNew(&kerl,&ci,"meth=POST\ncontenttype=text/xml\nsocks5=x\nurl=https://www.moneysmart.gov.au/unclaimedws/moneysmart/UnclaimedMoneySearch\npostdata=<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><SOAP-ENV:Body><m:simpleQueryRequest xmlns:m=\"uri:ebusiness.asic.gov.au\"><m:accountName><![CDATA[%s]]></m:accountName></m:simpleQueryRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>\n","u smith");
     curlyCallNew(&kerl,&ci,"meth=POST\ncontenttype=text/xml\nsocks5=x\nurl=https://www.moneysmart.gov.au/unclaimedws/moneysmart/UnclaimedMoneySearch\npostdata=<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><SOAP-ENV:Body><m:simpleQueryRequest xmlns:m=\"uri:ebusiness.asic.gov.au\"><m:otn>%I64d</m:otn></m:simpleQueryRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>\n",did);
   //  curlyCallNew(&kerl,&ci,"meth=POST\ncontenttype=text/xml\nsocks5=x\nurl=https://www.moneysmart.gov.au/unclaimedws/moneysmart/UnclaimedMoneySearch\npostdata=<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"  SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><SOAP-ENV:Body><m:UnclaimedDetailRequest xmlns:m=\"uri:ebusiness.asic.gov.au\"><m:detailID>%I64d</m:detailID><m:internalOnly>false</m:internalOnly></m:UnclaimedDetailRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>\n",did);
     did+=1LL;//aaMathRand64(10,100);

     //curlyCallNew(&kerl,&i,"meth=GET\nurl=https://www.moneysmart.gov.au\n");
     }
    }
   aaStageSet(240);
   break;


   case 240:
   //if(app.is_space==0&&aaMathRand32(0,100000)==0) { app.is_space=3; }
   if(kerl.cur_calls==0) { aaStageSet(300); break; }
   pf++;
   pf%=kerl.max_calls;
   if(kerl.cur_calls<kerl.max_calls)
    {
    if(app.is_space==0)
     {
     curlyCallNew(&kerl,&ci,"meth=POST\ncontenttype=text/xml\nsocks5=x\nurl=https://www.moneysmart.gov.au/unclaimedws/moneysmart/UnclaimedMoneySearch\npostdata=<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><SOAP-ENV:Body><m:simpleQueryRequest xmlns:m=\"uri:ebusiness.asic.gov.au\"><m:otn>%I64d</m:otn></m:simpleQueryRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>\n",did);
     //curlyCallNew(&kerl,&ci,"meth=POST\ncontenttype=text/xml\nsocks5=x\nurl=https://www.moneysmart.gov.au/unclaimedws/moneysmart/UnclaimedMoneySearch\npostdata=<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"  SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><SOAP-ENV:Body><m:UnclaimedDetailRequest xmlns:m=\"uri:ebusiness.asic.gov.au\"><m:detailID>%I64d</m:detailID><m:internalOnly>false</m:internalOnly></m:UnclaimedDetailRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>\n",did);
     did+=1LL;//aaMathRand64(10,100);
     break;
     }
    }
   if(kerl.call[pf].is_fin!=YES) { break; }
   mydbContentWrite(kerl.call[pf].cmd,kerl.call[pf].mun.used,kerl.call[pf].mun.mem);
   /*
   moneyDecode(kerl.call[pf].cmd,&kerl.call[pf].mun,&money_count,&money_array);
   if(money_count)
    {
    if(money_count>1) { aaEzyLog(&ezy,2,"%i",money_count); }
    for(m=0;m<money_count;m++)    {    moneyProc(money_array+m);    }
    if(aaMemoryBytesGet(money_array,&bytes)!=YES) oof;
    if(bytes==0) oof;
    if(aaMemoryRelease(money_array)!=YES) oof;
    }
   */
   curlyCallDelete(&kerl,pf);
   break;

   case 300:
   if(app.is_space!=0) { aaStageSet(400); break; }
   if(aaMathRand32(0,100)==0) { aaStageSet(110); }
   else                       { aaStageSet(240); }
   break;


   case 500:
   //if(aaBifiRead(&app.fifi,0,sizeof(_bifimoneyhdr),&bimohe)!=YES) { oof; }
   //aaEzyLog(&ezy,2,"count=%I64d conoff=%I64d ",bimohe.content_count,bimohe.content_offset);
   bb=0;
   aaStageSet(510);
   break;

   case 510:
   mydbContentVarsGet(&cnum,&hoff,&coff);
   if(bb>=cnum) { aaStageSet(520); break; }
   if((ret=aaBifiRead(&app.my_db,bb*80LL,80LL,tmp))!=YES) { oops; }
   roff=*(QP)&tmp[64];
   rlen=*(QP)&tmp[72];
   booboo[0]=0;
   if(rlen>0)
    {
    if((ret=aaBifiRead(&app.my_db,roff,rlen,booboo))!=YES) { oops; }
    }
   booboo[100]=0;
   aaEzyLog(&ezy,2,"%I64d %i %s %I64d %I64d %I64d %I64d %s",bb,tmp[0],&tmp[1],*(QP)&tmp[64],*(QP)&tmp[72],roff,rlen,booboo);



   #if 0
   if(aaBifiRead(&app.fifi,0,sizeof(_bifimoneyhdr),&bimohe)!=YES) { oof; }
   if(bb>=bimohe.content_count) { aaStageSet(520); break; }
   if(aaBifiRead(&app.fifi,1000LL+(bb*sizeof(_bificontenthdr)),sizeof(_bificontenthdr),&bicohd)!=YES) { oof; }
   if(bicohd.content_bytes>0LL)
   aaEzyLog(&ezy,2,"%I64d/%I64d sm=%i query=%s off=%I64d bytes=%I64d",bb,bimohe.content_count,bicohd.search_mode,bicohd.query,bicohd.content_offset,bicohd.content_bytes);
if(bicohd.content_bytes>_1MEG) oof;
   if((ret=aaBifiRead(&app.fifi,bicohd.content_offset,bicohd.content_bytes,booboo))!=YES) { oops; }
   booboo[bicohd.content_bytes]=0;
   booboo[255]=0;
   if(bicohd.content_bytes>0LL)
   aaEzyLog(&ezy,2,"%s",booboo);
#endif
   //if(bb>=searchnum) { aaStageSet(520); break; }
//   if((ret=aaBifiRead(&app.fifi,bb*sizeof(bimohe),(Q)sizeof(bimohe),&bimohe))!=YES) { oops; }
  // aaEzyLog(&ezy,2,"%I64d/%I64d %I64d %I64d",bb,searchnum,bimohe.response_bifi_offset,bimohe.response_bifi_bytes);

   bb++;
   break;



   }

  if(aa_stage>=220)
   {
      if(aaAboutEvery(100))
       {
       mydbContentVarsGet(&cnum,&hoff,&coff);
///        aaMemoryTableGet(0,&mused,&mavail);
//   if((ret=aaBifiRead(&app.fifi,0,sizeof(_bifimoneyhdr),&bimohe))!=YES) { oops; }

        aaEzyTitleSet(&ezy,"%i %i,%i %I64d %I64d  %i %I64d %I64d %I64d",aa_stage,kerl.cur_calls,kerl.fin_calls,did,aaMsRunning(),app.is_space,cnum,hoff,coff);
        }
   }

  appYield();
  }
 curlyDelete(&kerl);
 appStop();
 }





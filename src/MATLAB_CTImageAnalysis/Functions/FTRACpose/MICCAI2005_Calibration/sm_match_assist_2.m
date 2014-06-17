function [mm,mmmm]=sm_match_assist_2(mmm,pt,tr,ex,ey,xo,yo,f)

[mm1,e1]=sm_match_assist12(mmm,pt,tr,ex,ey,xo,yo,f);
[mm2,e2]=sm_match_assist22(mmm,pt,tr,ex,ey,xo,yo,f);
[mm3,e3]=sm_match_assist32(mmm,pt,tr,ex,ey,xo,yo,f);

[tmp,ind]=min([e1,e2,e3]);
tmp=[mm1;mm2;mm3];
mm=tmp(ind,:);

[k,N]=size(mmm);
tmp1=mmm(:,1);
tmp2=find(tmp1~=mm(1));
mmmm(:,1)=tmp1(tmp2);

tmp1=mmm(:,2);
tmp2=find(tmp1~=mm(2));
mmmm(:,2)=tmp1(tmp2);

tmp1=mmm(:,3);
tmp2=find(tmp1~=mm(3));
mmmm(:,3)=tmp1(tmp2);

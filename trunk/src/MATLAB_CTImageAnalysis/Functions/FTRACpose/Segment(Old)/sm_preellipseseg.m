function [Ie1,Ie2]=sm_preellipseseg(Ie,x1,y1,x2,y2)
%get two areas of interest;

Ie1=Ie(y1(1):y1(2),x1(1):x1(2));
Ie2=Ie(y2(1):y2(2),x2(1):x2(2));

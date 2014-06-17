function [Es,Eo]=sm_enhancer2(Iin)

I1=double(Iin);
[Esx,Esy]=gradient(I1);
Es=sqrt(Esx.^2+Esy.^2);
Eo=mod(atan2(Esy,Esx),pi);
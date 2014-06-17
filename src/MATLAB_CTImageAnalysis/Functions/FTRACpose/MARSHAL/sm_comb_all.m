function res=sm_comb_all(nn)
%SM_COMB_ALL generates all the possible triplets from input triplets.
%
%RES=SM_COMB_ALL(NN), where
%NN is a K-by-3 matrix of input triplets,
%returns res as the matrix of all the possible triplets.

[m,n]=size(nn);
bb=zeros(m^3,n);

aa=repmat((1:m),m^2,1);
bb(:,1)=reshape(aa,m^3,1);
cc=repmat((1:m),m,1);
cc=reshape(cc,m^2,1);
bb(:,2)=repmat(cc,m,1);
bb(:,3)=repmat((1:m)',m^2,1);

res=[nn(bb(:,1),1),nn(bb(:,2),2),nn(bb(:,3),3)];

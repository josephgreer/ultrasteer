r1=load('res_bigsim_catst.txt');
r2=load('res_bigsim_catst_90.txt');

res=zeros(162,5);
for i=1:9
    res((i-1)*18+1:i*18-1,:)=r1((i-1)*17+1:i*17,:);
    res(i*18,:)=r2(i,:);
end

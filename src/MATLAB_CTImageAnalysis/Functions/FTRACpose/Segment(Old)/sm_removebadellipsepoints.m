function psnew=sm_removebadellipsepoints(ps,a,b,c,d,e,f,k)

[Np,tmp]=size(ps);
dis=zeros(Np,1);
for i=1:Np
    x=ps(i,2);
    y=ps(i,1);
    dis(i)=abs((a*x^2+b*x*y+c*y^2+d*x+e*y+f)*k);
end
psnew=ps(find(dis>1.5),:);

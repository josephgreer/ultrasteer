function psnew=sm_removebadlinepoints(ps,a,b,c)

[Np,tmp]=size(ps);
tag=zeros(Np,1);
for i=1:Np
    d=sm_dist_pointtoline(ps(i,1),ps(i,2),a,b,c);
    if d>1.5
        tag(i)=1;
    end
end
psnew=ps(find(tag==0),:);

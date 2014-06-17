function g=sm_ellipsegroup3(e,dr_thresh)
%group ellipses using thresholding;

r=sqrt(e(:,1).^2+e(:,2).^2);
[v,ind]=sort(r);
e2=e(ind,:);

[Ne,tmp]=size(e2);
Ng=1;
g{Ng,1}=e2(1,:);
epre=[e2(1,1:2)];

for i=2:Ne
    dr=norm(e2(i,1:2)-epre);
    if dr<=dr_thresh
        g{Ng,1}=[g{Ng,1};e2(i,:)];
    else
        Ng=Ng+1;
        g{Ng,1}=e2(i,:);
    end
    epre=[e2(i,1:2)];
end

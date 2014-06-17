function sm_h(c,e,m_truth)

[nc,xc,cstp]=sm_hist(c);
[ne,xe,estp]=sm_hist(e);
figure, plot(xc,nc);
hold on
plot(xe,ne,'k');

ind=find(m_truth==0);
cc=c(ind);
ee=e(ind);

k=length(ind);
kc=length(xc);
ke=length(xe);

for i=1:k
    for j=1:kc
        if cc(i)<=xc(j)+cstp/2
            xxc=xc(j);
            nnc=nc(j);
            break;
        end
    end
    for j=1:ke
        if ee(i)<=xe(j)+estp/2
            xxe=xe(j);
            nne=ne(j);
            break;
        end
    end
    plot([xxc,xxe],[nnc,nne],'r');
end
title('4 images');

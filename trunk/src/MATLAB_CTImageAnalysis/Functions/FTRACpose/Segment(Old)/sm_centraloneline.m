function [Lc,ps]=sm_centraloneline(L,Ig,Ie)
%find the central line from one edge line;

[imax,jmax]=size(Ig);
ps=sm_centerpoints2(L(1:2),L(3:4),Ig,Ie);

while 1>0
    [Np,tmp]=size(ps);
    Lc(1:3)=sm_ransac_line(ps);
    psnew=sm_removebadlinepoints(ps,Lc(1),Lc(2),Lc(3));
    [Npnew,tmp]=size(psnew);
    if Np==Npnew
        break;
    else
        ps=psnew;
    end
end

[Lc(4:5),Lc(6:7)]=sm_drawline2(imax,jmax,Lc(1),Lc(2),Lc(3));
[Lc(8),Lc(9)]=sm_rthofline(Lc(4:5),Lc(6:7));
%figure;
%imshow(Ie);
%hold on;
%axis on;
%for p=1:Np
%    plot(ps(p,2),ps(p,1),'*');
%end

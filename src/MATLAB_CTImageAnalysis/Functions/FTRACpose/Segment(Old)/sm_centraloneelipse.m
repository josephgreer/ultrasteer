function [ec,psc]=sm_centraloneelipse(x,y,th,a,b,Ig,Ie)
%find the central ellipse from one edge ellipse;

pse=sm_pointsofelipse(x,y,th,a,b);
psc=sm_centerpointselips(pse,Ig);

while 1>0
    [Np,tmp]=size(psc);
    ec=sm_ransac_elip(psc);
    pscnew=sm_removebadellipsepoints(psc,ec(1),ec(2),ec(3),ec(4),ec(5),ec(6),ec(7));
    [Npnew,tmp]=size(pscnew);
    if Np==Npnew
        break;
    else
        psc=pscnew;
    end
end
%figure;
%imshow(Ie);
%hold on;
%axis on;
%for p=1:Np
%    plot(ps(p,2),ps(p,1),'*');
%end

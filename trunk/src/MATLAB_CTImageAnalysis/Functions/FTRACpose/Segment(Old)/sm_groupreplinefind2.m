function [Lgc,ps]=sm_groupreplinefind2(pslist,imax,jmax)
%find the representative line for a group of lines;

[Nps,tmp]=size(pslist);
ps=pslist{1};
for i=2:Nps
    ps=sm_pscat(ps,pslist{i});
end

while 1>0
    [Np,tmp]=size(ps);
    Lgc(1:3)=sm_ransac_line(ps);
    psnew=sm_removebadlinepoints(ps,Lgc(1),Lgc(2),Lgc(3));
    [Npnew,tmp]=size(psnew);
    if Np==Npnew
        break;
    else
        ps=psnew;
    end
end

[Lgc(4:5),Lgc(6:7)]=sm_drawline2(imax,jmax,Lgc(1),Lgc(2),Lgc(3));
[Lgc(8),Lgc(9)]=sm_rthofline(Lgc(4:5),Lgc(6:7));

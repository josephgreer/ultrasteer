function Lc=sm_centrallinesfind(g_rep,Ig,Ie)
%find the central line for each group;
[imax,jmax]=size(Ig);
[N_g,tmp]=size(g_rep);
Lc=zeros(N_g,9);
for k=1:N_g
    [N,tmp]=size(g_rep{k});
    if N==1
        ps=sm_centerpoints2(g_rep{k}(1,1:2),g_rep{k}(1,3:4),Ig,Ie);
    else
        ps1=sm_centerpoints2(g_rep{k}(1,1:2),g_rep{k}(1,3:4),Ig,Ie);
        ps2=sm_centerpoints2(g_rep{k}(2,1:2),g_rep{k}(2,3:4),Ig,Ie);
        ps=sm_pscat(ps1,ps2);
    end
    Lc(k,1:3)=sm_ransac_line(ps);
    [Lc(k,4:5),Lc(k,6:7)]=sm_drawline2(imax,jmax,Lc(k,1),Lc(k,2),Lc(k,3));
    [Lc(k,8),Lc(k,9)]=sm_rthofline(Lc(k,4:5),Lc(k,6:7));
end


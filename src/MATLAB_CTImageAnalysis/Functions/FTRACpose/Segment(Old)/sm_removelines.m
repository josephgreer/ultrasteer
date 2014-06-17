function Ib_new=sm_removelines(L,Ib)

Ib_new=Ib;
[N_lines,tmp]=size(L);
for i=1:N_lines
    ps=round(sm_pointsofline(L(i,1:2),L(i,3:4)));
    [N_points,tmp]=size(ps);
    for j=1:N_points
        Ib_new(ps(j,1),ps(j,2))=0;
    end
end

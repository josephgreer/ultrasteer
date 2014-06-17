function cts=sm_intersectfind(ax,imax,jmax)
%find the intersection of all the mutually perpendicular lines;
dth_thresh=pi/180*5;
[N_lines,tmp]=size(ax);
c=0;
cts=[];
for i=1:N_lines
    [p1,p2]=sm_drawline(imax,jmax,ax(i,1),ax(i,2));
    Li=least_squares_line([p1;p2]);
    for j=i+1:N_lines
        dth=mod(ax(j,2)-ax(i,2),2*pi);
        if abs(dth-pi/2)<=dth_thresh | abs(dth-pi*3/2)<=dth_thresh
            [p3,p4]=sm_drawline(imax,jmax,ax(j,1),ax(j,2));
            Lj=least_squares_line([p3;p4]);
            p5=inv([Li(1),Li(2);Lj(1),Lj(2)])*[-Li(3);-Lj(3)];
            if p5(1)>=1 & p5(1)<=imax & p5(2)>=1 & p5(2)<=jmax
                c=c+1;
                cts(c,:)=[p5(1),p5(2),ax(i,1),ax(i,2),ax(j,1),ax(j,2)];
            end
        end
    end    
end


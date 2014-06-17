function eh=sm_fidellipsesdetect(Ie)

disp('Detecting the ellipses of the fiducial...');
[imax,jmax]=size(Ie);
%1.find the possible symmetric axes using hough transform;
Ien=imclose(Ie,strel('disk',2));
Ien=imerode(Ien,strel('disk',1));
imshow(Ien);
axis on;
hold on;

%2. find the symmetric axes;
%t0=clock;
ax=sm_houghsymaxis2(Ien);
[imax,jmax]=size(Ien);
%[Nx,tmp]=size(ax);
%for i=1:Nx
%    [p1,p2]=sm_drawline(imax,jmax,ax(i,1),ax(i,2));
%    line([p1(2),p2(2)],[p1(1),p2(1)]);
%end
%etime(clock,t0)

%3. find the centers of possible ellipses;
cts=sm_intersectfind(ax,imax,jmax);
if length(cts)==0
    eh=[];
    return;
end
%[Nc,tmp]=size(cts);
%for i=1:Nc
%    plot(cts(i,2),cts(i,1),'*');
%end

%4. find the possible ellipses;
ps=sm_getallcontourpoints(Ien);
[Nc,tmp]=size(cts);
e=[];
for c=1:Nc
    c
    e=[e;sm_houghaxislength2(cts(c,:),ps,imax,jmax)];
end
[Ne,tmp]=size(e);
%for k=1:Ne
%    sm_drwelips(e(k,2),e(k,1),e(k,4),e(k,7),e(k,8));
%end

%5. find top 10 ellipses;
tmp=e(:,10);
[v,ind]=sort(tmp);
Neh=10;
if Ne<=Neh
    eh=e;
else
    for i=1:Neh
        eh(i,:)=e(ind(Ne-i+1),:);
    end
end
for k=1:Neh
    sm_drwelips(eh(k,2),eh(k,1),eh(k,4),eh(k,7),eh(k,8));
end

%2.find centers of possible ellipses,i.e. the intersection of the axes;
%[imax,jmax]=size(Ie);
%cts=sm_intersectfind(ax,imax,jmax);

%3.find possible ellipses using hough tranform;
%ps=sm_getallcontourpoints(Ie);
%[Nc,tmp]=size(cts);
%ec=[];
%for i=1:Nc
%    i
%    ec=[ec;sm_houghaxislength2(cts(i,:),ps,imax,jmax)];
%end
%[Ne,tmp]=size(ec);
%ee=zeros(Ne,6);
%for k=1:Ne
%    if ec(k,7)<=ec(k,8)
%        ee(k,:)=[ec(k,2),ec(k,1),ec(k,4),ec(k,7),ec(k,8),ec(k,9)];
%    else
%        ee(k,:)=[ec(k,2),ec(k,1),ec(k,6),ec(k,8),ec(k,7),ec(k,9)];
%    end
%end

%4.group ellipses according to the centers;
%drct_thresh=5;
%ge=sm_ellipsegroup3(ee,drct_thresh);

%5.find two representative ellipses for each group;
%[Ng,tmp]=size(ge);
%gerep=cell(Ng,1);
%for i=1:Ng
%    [Ntmp,tmp]=size(ge{i});
%    if Ntmp<=1
%        gerep(i)=ge(i);
%    else
%        etmp=ge{i};
%        a=etmp(:,4);
%        b=etmp(:,5);
%        u=pi*(1.5*(a+b)-sqrt(a.*b));
%        v=etmp(:,6)./u;
%        [vtmp,ind]=sort(v);
%        gerep{i}=etmp(ind(Ntmp-1:Ntmp),:);
%    end
%end

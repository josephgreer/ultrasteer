function sm_autoseg(dir,fim,x,y,x1,y1,x2,y2)

%t0=clock;
%1.preprocess to small image, gaussian image, edge image, and gradient image;
[Is,Ig,Ie,Ies]=sm_preseg(dir,fim,x,y);

%2.detect straight lines;
[Lf,Lgcr,L]=sm_fidlinesdetect5(Is,Ig,Ie,Ies);
%tline=etime(clock,t0)
%figure;
%imshow(Ie);
%axis on;
%hold on;
%for i=1:3
%    line([Lf(i,5),Lf(i,7)],[Lf(i,4),Lf(i,6)]);
%end
%saveas(h2,strcat(dir,fim,'_finallines','.jpg'));

%3.prepare for ellipse detection, remove the major straight lines from edge image;
Ie=sm_postlineseg(Ie,Lgcr,L);
%figure;
%imshow(Ie);
%axis on;
%hold on;

%4.get two areas of interest;
[Ie1,Ie2]=sm_preellipseseg(Ie,x1,y1,x2,y2);

%5.detect two ellipses;
figure;
subplot(1,2,1);
eh1=sm_fidellipsesdetect(Ie1);
subplot(1,2,2);
eh2=sm_fidellipsesdetect(Ie2);

%4.detect ellipses;
%[cts,ee,gerep]=sm_fidellipsesdetect(Ienew);
%figure;
%imshow(Ienew);
%axis on;
%hold on;
%[Nc,tmp]=size(cts);
%for i=1:Nc
%    plot(cts(i,2),cts(i,1),'*');
%end
%[Ne,tmp]=size(ee);
%for k=1:Ne
%    sm_drwelips(ee(k,1),ee(k,2),ee(k,3),ee(k,4),ee(k,5));
%end
%figure;
%imshow(Ienew);
%axis on;
%hold on;
%[Ng,tmp]=size(gerep);
%for i=1:Ng
%    [N,tmp]=size(gerep{i});
%    for k=1:N
%        sm_drwelips(gerep{i}(k,1),gerep{i}(k,2),gerep{i}(k,3),gerep{i}(k,4),gerep{i}(k,5));
%    end
%end

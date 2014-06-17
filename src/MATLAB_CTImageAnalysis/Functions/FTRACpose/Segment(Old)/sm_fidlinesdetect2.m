function Lf=sm_fidlinesdetect2(Is,Ig,Ie,Ies)
%detect straight lines in the fiducial;

disp('Detecting the straight lines of the fiducial...');

%1.Limit the range of the search angles using radon transform;
angle=sm_radon(Ie);

%2.detect edge lines using hough transform;
ang_range=pi/180*5;
dis_stp=0.5;
ang_stp=pi/360;
N_lines=20;
res=sm_houghline4(Ie,angle,ang_range,dis_stp,ang_stp,N_lines);
[imax,jmax]=size(Ie);
[N_lines,tmp]=size(res);
L=zeros(N_lines,4);
for k=1:N_lines
    [L(k,1:2),L(k,3:4)]=sm_drawline(imax,jmax,res(k,1),res(k,2));
    [L(k,5),L(k,6)]=sm_rthofline(L(k,1:2),L(k,3:4));
end

%3.find the central line for each edge line;
Lc=zeros(N_lines,9);
ps=cell(N_lines,1);
for i=1:N_lines
    [Lc(i,:),ps{i}]=sm_centraloneline(L(i,:),Ig,Ie);
end

%4.group central lines into groups using thresholding;
dis_thresh=3;
ang_thresh=pi/180*5;
g=sm_linesgroup3(Lc,dis_thresh,ang_thresh,ang_range);

%5.find one representative central line for each group;
[Ng,tmp]=size(g);
for i=1:Ng
    Lgc(i,:)=sm_groupreplinefind(ps(g{i}(:,10)),imax,jmax);
end

%6.find the 3 desired central lines;
%work on this !!!
Lf=sm_deslinesfind(Lgc,Ies);

figure;
imshow(Ie);
axis on;
hold on;
for i=1:Ng
    line([Lgc(i,5),Lgc(i,7)],[Lgc(i,4),Lgc(i,6)]);
end


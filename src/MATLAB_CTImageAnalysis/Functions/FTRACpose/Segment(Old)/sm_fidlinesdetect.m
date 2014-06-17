function Lf=sm_fidlinesdetect(Is,Ig,Ie,Ies)
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
figure;
imshow(Ie);
axis on;
hold on;
for i=1:N_lines
    line([L(i,2),L(i,4)],[L(i,1),L(i,3)]);
end

%3.group edge lines into groups using thresholding;
dis_thresh=8;
ang_thresh=pi/180*5;
g=sm_linesgroup(L,dis_thresh,ang_thresh);

%4.find two representative lines for each group;
g_rep=sm_replinesfind(g);

%5.find the central line for each group;
Lc=sm_centrallinesfind(g_rep,Ig,Ie);

%6.find the central lines following the general trend;
%trend_thresh=pi/180*15;
%Lt=sm_trendfind(Lc,ang_thresh,trend_thresh);
Lt=Lc;

%7.find the 3 desired central lines;
Lf=sm_deslinesfind(Lt,Ies);

figure;
imshow(Ie);
axis on;
hold on;
for i=1:3
    line([Lf(i,5),Lf(i,7)],[Lf(i,4),Lf(i,6)]);
end

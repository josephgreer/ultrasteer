function [Is,Ig,Ie,Ies]=sm_preseg(dir,fim,x,y)

disp('Preprocessing the image...');

%l.load the original image;
Io=imread(strcat(dir,fim,'.jpeg'));

%2.cut the area of interest from the original image;
Is=Io(y(1):y(2),x(1):x(2));
%hs=figure;
%imshow(Is);
%saveas(hs,strcat(dir,fim,'_small','.jpg'));

%3.preprocess the original image with gaussian filter;
Iog=sm_gaussfilter(Io);
Ig=Iog(y(1):y(2),x(1):x(2));
%hg=figure;
%imshow(Ig);
%saveas(hg,strcat(dir,fim,'_gaussian','.jpg'));

%4.get the edge image using edge detection;
Ie=edge(Ig,'canny');
%he=figure;
%imshow(Ie);
%saveas(he,strcat(dir,fim,'_edge','.jpg'));

%5.get the gradient image using enhancer;
[Ioes,Ioeo]=sm_enhancer2(Iog);
Ies=Ioes(y(1):y(2),x(1):x(2));
Ieo=Ioeo(y(1):y(2),x(1):x(2));
%hes=figure;
%imshow(Ies);
%saveas(hes,strcat(dir,fim,'_gradient','.jpg'));

function ShowAccuracyOfPointIndex(pt, fitPoints, fgps, fim, poly, descriptor, degree, mpp, index, indices)

for ind=1:size(pt,1)
    imheader = ReadHeader(fim);
    header = ReadHeader(fgps);
    im = RPreadframeindex(fim, imheader, index);
    gps = RPreadgpsindex(fgps, header, index);

    
    tvals = TvalsFromDescriptor(fitPoints, descriptor);
    range = [min(tvals), max(tvals)];
    intersection = IntersectFrameAndPoly(header, gps, mpp, poly, degree, range, transpose(pt(ind,[1:3])));


    point = MapWorldCoordToImageCoord(intersection, header, mpp, gps);

    ptIm = MapWorldCoordToImageCoord(transpose(pt(ind,[1:3])), header, mpp, gps);

    hold off;
    figure(10);
    imshow(im, [0 255]);
    hold on;
    title(num2str(index));
    scatter(point(1), point(2), 200, 'b');
    scatter(ptIm(1), ptIm(2), 200, 'r');
    hold off;



    ul = header.ul;
    br = header.br;
    ur = [br(1) ul(2)];
    bl = [ul(1) br(2)];


    ul = MapImageCoordToWorldCoord(ul, header, mpp, gps);
    bl = MapImageCoordToWorldCoord(bl, header, mpp, gps);
    ur = MapImageCoordToWorldCoord(ur, header, mpp, gps);

    figure(11);

     sobj = PlotPlane(ul, ur-ul, bl-ul);
     colormap bone;
%     set(gca,... 
% 'XTickLabel','');
%     set(gca,... 
% 'YTickLabel','');
%     set(gca,... 
% 'ZTickLabel','');
   % set(sobj,'CData',im,'FaceColor','texturemap');
   % colormap gray;
%     b1 = ur-ul;
%     b2 = bl-ul;
%     [u v] = ndgrid(0:1, 0:1);
%     X = ul(1)+u*b1(1)+v*b2(1);
%     Y = ul(2)+u*b1(2)+v*b2(2);
%     Z = ul(3)+u*b1(3)+v*b2(3);
%     surface(X,Y,Z,transpose(im([header.ul(2):1:header.br(2)], [header.ul(1):1:header.ur(1)])));
    
    hold on;
    pPoints = PolyPoints(range, poly, 200);
    plot3(pPoints(:,1), pPoints(:,2), pPoints(:,3), 'r');

    scatter3(intersection(1), intersection(2), intersection(3), 200, 'bx');
    scatter3(pt(ind,1), pt(ind,2), pt(ind,3), 200, 'rx');

    title(num2str(index));
    %xlim(range);
    axis equal;

    hold off;
end
figure(1);
hold on;
end
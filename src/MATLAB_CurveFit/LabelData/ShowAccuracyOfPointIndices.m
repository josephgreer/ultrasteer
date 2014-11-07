function ShowAccuracyOfPointIndices(fitPoints, fgps, fim, poly, descriptor, degree, mpp, indices)

idxs = [41];
% for i=1:5
%     id = i*round(size(fitPoints,1)/20);
%     idxs = [idxs id];
% end

tvals = TvalsFromDescriptor(fitPoints, descriptor);
range = [min(tvals), max(tvals)];


gcf = figure(11);
hold on;
pPoints = PolyPoints(range, poly, 200);
plot3(pPoints(:,1), pPoints(:,2), pPoints(:,3), 'Color', [0 1 0], 'LineWidth', 2);
axis equal;
grid on;

for i=1:size(idxs,2)
    imheader = ReadHeader(fim);
    header = ReadHeader(fgps);
    id = indices(idxs(i));
    im = RPreadframeindex(fim, imheader, id);
    
    im = imread('C:/test.bmp');
    gps = RPreadgpsindex(fgps, header, id);

    intersection = IntersectFrameAndPoly(header, gps, mpp, poly, degree, range, transpose(fitPoints(idxs(i),:)));


    point = MapWorldCoordToImageCoord(intersection, header, mpp, gps);

    ptIm = MapWorldCoordToImageCoord(transpose(fitPoints(idxs(i),[1:3])), header, mpp, gps);


    ul = header.ul;
    br = header.br;
    ur = [br(1) ul(2)];
    bl = [ul(1) br(2)];


    ul = MapImageCoordToWorldCoord(ul, header, mpp, gps);
    bl = MapImageCoordToWorldCoord(bl, header, mpp, gps);
    ur = MapImageCoordToWorldCoord(ur, header, mpp, gps);

%     sobj = PlotPlane(ul, ur-ul, bl-ul);
%     set(sobj,'CData',im,'FaceColor','texturemap');
%     colormap gray;
    b1 = ur-ul;
    b2 = bl-ul;
    [u v] = ndgrid(0:1, 0:1);
    X = ul(1)+u*b1(1)+v*b2(1);
    Y = ul(2)+u*b1(2)+v*b2(2);
    Z = ul(3)+u*b1(3)+v*b2(3);
    %surface(X,Y,Z,transpose(im([header.ul(2):1:header.br(2)], [header.ul(1):1:header.ur(1)])), 'FaceColor', 'texturemap', 'EdgeColor','none');
    
    subim = im([header.ul(2):1:header.br(2)], [header.ul(1):1:header.ur(1)],:);
    s = surface(X,Y,Z,permute(subim,[2 1 3]), 'FaceColor', 'texturemap', 'EdgeColor','none');
    rgb_colors = get(s, 'CData');
    [ispy, map] = rgb2ind(rgb_colors, 64);
    set(s, 'CData', double(ispy));
    colormap(map);
    %colormap default;
    hold on;
    

    %scatter3(intersection(1), intersection(2), intersection(3), 200, 'bx');
    %scatter3(fitPoints(idxs(i),1), fitPoints(idxs(i),2), fitPoints(idxs(i),3), 200, 'rx');

    %xlim(range);

end
axis equal;
xlim([90 125]);
ylim([95 120]);
%zlim([140 110]);
%zlim([110 140]);
xlabel('x (mm)');
ylabel('y (mm)');
zlabel('z (mm)');
set(gcf,'color',[.85 .85 .85]);
figure(1);
hold on;
end
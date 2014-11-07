function MovePointAlongCurveTangent(fitPoints, fgps, fim, poly, descriptor, degree, mpp, indices)
index = str2double(inputdlg('frame index please'));

fheader = ReadHeader(fim);
gheader = ReadHeader(fgps);
im = RPreadframeindex(fim, fheader, index);
gps = RPreadgpsindex(fgps, gheader, index);

figure(20);
imshow(im, [0 255]);
pt = ginput(1);

close 20;


fitPoint = MapImageCoordToWorldCoord(pt, gheader, mpp, gps);
tvals = TvalsFromDescriptor(fitPoints,descriptor);
range = [min(tvals), max(tvals)];
tangent = TangentAtFrame(gheader, gps, mpp, poly, degree, range, fitPoint);

gps_2 = RPreadgpsindex(fgps, gheader, index+1);
pt2 = PredictPoint(gps, gps_2, mpp, fheader, pt, transpose(tangent));

im2 = RPreadframeindex(fim, fheader, index+1);

pt2correct = IntersectFrameAndPoly(gheader, gps_2, mpp, poly, degree, range, fitPoint);
pt2correct = MapWorldCoordToImageCoord(pt2correct, gheader, mpp, gps_2);
figure(20);
imshow(im2, [0 255]);
hold on;
scatter(pt2(1), pt2(2), 200, 'r');
scatter(pt2correct(1),pt2correct(2), 200, 'b');




figure(21);

ul = fheader.ul;
br = fheader.br;
ur = [br(1) ul(2)];
bl = [ul(1) br(2)];

ul2 = MapImageCoordToWorldCoord(ul, gheader, mpp, gps_2);
bl2 = MapImageCoordToWorldCoord(bl, gheader, mpp, gps_2);
ur2 = MapImageCoordToWorldCoord(ur, gheader, mpp, gps_2);

ul = MapImageCoordToWorldCoord(ul, gheader, mpp, gps);
bl = MapImageCoordToWorldCoord(bl, gheader, mpp, gps);
ur = MapImageCoordToWorldCoord(ur, gheader, mpp, gps);

PlotPlane(ul, ur-ul, bl-ul);
hold on;
PlotPlane(ul2, ur2-ul2, bl2-ul2);
pPoints = PolyPoints(range, poly, 200);
plot3(pPoints(:,1), pPoints(:,2), pPoints(:,3), 'r');

intersection = IntersectFrameAndPoly(gheader, gps, mpp, poly, degree, range, fitPoint);
normal = cross(bl-ul, ur-ul);
normal = 10*normal/norm(normal);

tangent = 10*tangent/norm(tangent);
plot3([intersection(1)-tangent(1); intersection(1)+tangent(1)], [intersection(2)-tangent(2); intersection(2)+tangent(2)], [intersection(3)-tangent(3); intersection(3)+tangent(3)], 'g');
plot3([intersection(1)-normal(1); intersection(1)+normal(1)], [intersection(2)-normal(2); intersection(2)+normal(2)], [intersection(3)-normal(3); intersection(3)+normal(3)], 'k');

axis equal;

end
%%%% output is [ulx uly ulz
%%%%            blx bly blz
%%%%            brx bry brz
%%%%            urx ury urz
%%%%            ulx uly ulz]
%%%% in world coords

function [ x y z ] = FrameBoundariesWorldCoords(header, gps, mpp)
ul = [header.ul(1) header.ul(2)];
br = [header.br(1) header.br(2)];
ur = [br(1) ul(2)];
bl = [ul(1) br(2)];

output = [MapImageCoordToWorldCoord(ul, header, mpp, gps) MapImageCoordToWorldCoord(bl, header, mpp, gps) MapImageCoordToWorldCoord(br, header, mpp, gps) MapImageCoordToWorldCoord(ur, header, mpp, gps)];
output = transpose(output);
output = [output;output(1,:)];

x = output(:,1);
y = output(:,2);
z = output(:,3);
end


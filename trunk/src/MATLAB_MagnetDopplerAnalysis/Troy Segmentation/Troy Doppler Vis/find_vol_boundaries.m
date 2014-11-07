function p_vol = find_vol_boundaries(imgDepth,probeR3D_mm,micr_x,micr_y,o_x,o_y,sect_ang,sliceInt)
%FIND_VOL_BOUNDARIES Finds boundaries of US volume in image coordinates

rpix = probeR3D_mm/(micr_y/1000); % 3D radius of probe in pix
Rpix = rpix + imgDepth/(micr_y/1000); % radius of bottom of image in pix
o_circ = [o_x o_y - rpix];

n = 50; % Number of points along each arc
angsweep = linspace(-sect_ang/2,sect_ang/2,n)'; % Angles along the arcs
dsweep = linspace(rpix,Rpix,n)'; % Depth between inner and outer radius


i = 1;
% Outlines of 2D image slices
for slice = [1:sliceInt:61]
    % top
    p_vol{i} = [sind(angsweep)*rpix + o_circ(1)...
        cosd(angsweep)*rpix + o_circ(2) slice*ones(n,1)];
    i = i+1;
    % bottom
    p_vol{i} = [sind(angsweep)*Rpix + o_circ(1)...
        cosd(angsweep)*Rpix + o_circ(2) slice*ones(n,1)];
    i = i+1;
    % left
    p_vol{i} = [sind(-sect_ang/2).*dsweep + o_circ(1)...
        cosd(-sect_ang/2).*dsweep + o_circ(2) slice*ones(n,1)];
    i = i+1;
    % right
     p_vol{i} = [sind(sect_ang/2).*dsweep + o_circ(1)...
        cosd(sect_ang/2).*dsweep + o_circ(2) slice*ones(n,1)];
    i = i+1;
end
% Arcs between slices (along sweep curve)
ul = []; ur = []; bl = []; br = [];
for slice = 1:61
    ul = [ul; sind(-sect_ang/2)*rpix+o_circ(1) cosd(-sect_ang/2)*rpix+o_circ(2) slice];
    ur = [ur; sind(sect_ang/2)*rpix+o_circ(1) cosd(sect_ang/2)*rpix+o_circ(2) slice];
    bl = [bl; sind(-sect_ang/2)*Rpix+o_circ(1) cosd(-sect_ang/2)*Rpix+o_circ(2) slice];
    br = [br; sind(sect_ang/2)*Rpix+o_circ(1) cosd(sect_ang/2)*Rpix+o_circ(2) slice];
end
    p_vol{i} = ul;
    i = i+1;
    p_vol{i} = ur;
    i = i+1;
    p_vol{i} = bl;
    i = i+1;
    p_vol{i} = br;
end


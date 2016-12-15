function psis = FindActuatorAngles(xs, plane, R0)
p1 = projectPointOntoPlane(plane, xs{2}.pos);
p2 = projectPointOntoPlane(plane, xs{3}.pos);
p3 = projectPointOntoPlane(plane, xs{4}.pos);

d1 = p1-xs{1}.pos;
d2 = p2-xs{1}.pos;
d3 = p3-xs{1}.pos;

psis(1) = atan2(dot(R0(:,2),d1), dot(R0(:,1),d1));
psis(2) = atan2(dot(R0(:,2),d2), dot(R0(:,1),d2));
psis(3) = atan2(dot(R0(:,2),d3), dot(R0(:,1),d3));
end
function xs = RotatePoints(x,R0,scale)
if(isempty(scale))
    scale = 1;
end
for i=1:length(x)
    xs{i}.pos = scale*R0.'*x{i}.pos;
    xs{i}.q = RotationMatrixToQuat(R0.'*QuatToRotationMatrix(x{i}.q));
end
end
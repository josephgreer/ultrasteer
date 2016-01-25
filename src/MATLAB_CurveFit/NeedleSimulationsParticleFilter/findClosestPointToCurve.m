function uv = findCurvePointOnFrame(curvePoints, invA, measurement)    % find flagella point with minimum distance projection onto ultrasound
% frame
ds = zeros(3,size(curvePoints,2));
for j=1:size(curvePoints,2)
    
    % find distance between particle flagella position and plane
    ds(:,j) = invA*(curvePoints(:,j)-measurement.ful);
end;

[minAbsVal, minAbsIdx] = min(abs(ds(3,:)));
% particle projection onto us frame in pixels
uv = ds([1 2], minAbsIdx);
end
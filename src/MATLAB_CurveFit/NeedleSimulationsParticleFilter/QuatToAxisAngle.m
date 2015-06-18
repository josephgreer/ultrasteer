function v = QuatToAxisAngle(q)
if (q(1) > 1)
    q = q/norm(q); % if w>1 acos and sqrt will produce errors, this cant happen if quaternion is normalised
end
angle = 2 * acos(q(1));
s = sqrt(1-q(1)*q(1)); % assuming quaternion normalised then w is less than 1, so term always positive.
if (s < 1e-2)  % test to avoid divide by zero, s is always positive due to sqrt
    % if s close to zero then direction of axis not important
    x = q(2); % if it is important that axis is normalised then replace with x=1; y=z=0;
    y = q(3);
    z = q(4);
else
    x = q(2) / s; % normalise axis
    y = q(3) / s;
    z = q(4) / s;
end

if(angle > pi || angle < 0)
    assert(0);
end
v = [x;y;z]*angle;
end
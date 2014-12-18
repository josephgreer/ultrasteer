% assumes q (quaternion) is column vector
function qi = quatinverse(q)
qi = [q(1); -q(2); -q(3); -q(4)]/(q'*q);
end
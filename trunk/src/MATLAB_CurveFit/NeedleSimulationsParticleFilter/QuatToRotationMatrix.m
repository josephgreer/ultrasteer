function R = QuatToRotationMatrix(q)
sqw = q(1)*q(1);
sqx = q(2)*q(2);
sqy = q(3)*q(3);
sqz = q(4)*q(4);

if(abs(norm(q)-1) > 1e-1)
    x = 0;
end
assert(abs(norm(q)-1) < 1e-1)

R = zeros(3,3);

% invs (inverse square length) is only required if quaternion is not already normalised
R(1,1) = ( sqx - sqy - sqz + sqw) ; 
R(2,2) = (-sqx + sqy - sqz + sqw);
R(3,3) = (-sqx - sqy + sqz + sqw);

tmp1 = q(2)*q(3);
tmp2 = q(4)*q(1);
R(2,1) = 2.0 * (tmp1 + tmp2) ;
R(1,2) = 2.0 * (tmp1 - tmp2) ;

tmp1 = q(2)*q(4);
tmp2 = q(3)*q(1);
R(3,1) = 2.0 * (tmp1 - tmp2) ;
R(1,3) = 2.0 * (tmp1 + tmp2) ;
tmp1 = q(3)*q(4);
tmp2 = q(2)*q(1);
R(3,2) = 2.0 * (tmp1 + tmp2) ;
R(2,3) = 2.0 * (tmp1 - tmp2) ;
end
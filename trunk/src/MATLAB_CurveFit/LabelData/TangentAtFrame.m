function tangent = TangentAtFrame(header, gps, mpp, poly, degree, dRange, fitPoint)
[fx fy fz] = FrameBoundariesWorldCoords(header, gps, mpp);
ul = [fx(1); fy(1); fz(1)];
bl = [fx(2); fy(2); fz(2)];
br = [fx(3); fy(3); fz(3)];
ur = [fx(4); fy(4); fz(4)];

%%%% Plane normal
n = cross(ur-ul, bl-ul);
n = n/norm(n);

%%%%% Project polynomial onto plane normal to get 1D polynomial
polyx = poly(1,:); polyy = poly(2,:); polyz = poly(3,:);
polyproj = n(1)*polyx+n(2)*polyy+n(3)*polyz;

%%%% distance from plane to origin
odist = transpose(ul)*n;

polyproj(degree+1) = polyproj(degree+1)-odist;

r = roots(polyproj);


derivx = [];
derivy = [];
derivz = [];
for i=1:degree
    derivx = [derivx polyx(i)*(degree-(i-1))];
    derivy = [derivy polyy(i)*(degree-(i-1))];
    derivz = [derivz polyz(i)*(degree-(i-1))];
end

bestFit = 1000;
tangent = [];
for i=1:size(r,1)
    %%%%%% don't want imaginary
    if(imag(r(i)) == 0)
        mul = [];
        for j=degree:-1:0
            mul = [mul; r(i)^j];
        end
        pt = [polyx*mul; polyy*mul; polyz*mul];
        mat = [(ur-ul) (bl-ul)];
        mat = mat([1 2],:);
        my = [pt(1)-ul(1); pt(2)-ul(2)];
        res = inv(mat)*my;
        
        planePoint = ul+res(1)*(ur-ul)+res(2)*(bl-ul);
        if(abs(planePoint(3)-pt(3)) < .1)
            dist = norm(fitPoint-planePoint);
            if(dist < bestFit)
                bestFit = dist;
                tangent = [derivx*mul([2:degree+1]) derivy*mul([2:degree+1]) derivz*mul([2:degree+1])];
            end
        end
    end
end

end
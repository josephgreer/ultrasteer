%A function to check if the ellipses are good. The ratio of the majoraxis
%divided by the minor axis, if <10, implies it is a good ellipse. Not more
%than 1 ellipse is discarded. Sometimes least squares ellipse fitting is
%inaccurate and give a hyperbola as an answer, and hence we also need to 
%check for that too. The function finally returns atleast one acceptable
%ellipse.
function good_ellps = Ellipse_Check(ells)

if(isempty(ells))
    good_ellps = [];
    return;
end

ell1 = ells(1,:);
ell2 = ells(2,:);

%Check acceptability for the first ellipse. Also calculate the ratio of
%majoraxs/minoraxis.
A = [ell1(1) ell1(2)/2 ell1(4)/2 ; ell1(2)/2 ell1(3) ell1(5)/2 ; ell1(4)/2 ell1(5)/2 ell1(6) ];
B = A(1:2, 1:2);
C = ell1(1) + ell1(3);
a_dash = (C + sqrt( (ell1(1) - ell1(3))^2 + ell1(2)^2 ))/2;
b_dash = (C - sqrt( (ell1(1) - ell1(3))^2 + ell1(2)^2 ))/2;
c_dash = det(A)/det(B);
major_axis = sqrt(-c_dash/a_dash);
minor_axis = sqrt(-c_dash/b_dash);
ell1_ratio = major_axis/minor_axis;
if(det(A)~=0 & det(B)>0 & isreal(major_axis) & isreal(minor_axis))
    e1_flag = 1;
else
    e1_flag = 0;
end

%Check acceptability for the second ellipse.
A = [ell2(1) ell2(2)/2 ell2(4)/2 ; ell2(2)/2 ell2(3) ell2(5)/2 ; ell2(4)/2 ell2(5)/2 ell2(6) ];
B = A(1:2, 1:2);
C = ell2(1) + ell2(3);
a_dash = (C + sqrt( (ell2(1) - ell2(3))^2 + ell2(2)^2 ))/2;
b_dash = (C - sqrt( (ell2(1) - ell2(3))^2 + ell2(2)^2 ))/2;
c_dash = det(A)/det(B);
major_axis = sqrt(-c_dash/a_dash);
minor_axis = sqrt(-c_dash/b_dash);
ell2_ratio = major_axis/minor_axis;
if(det(A)~=0 & det(B)>0 & isreal(major_axis) & isreal(minor_axis))
    e2_flag = 1;
else
    e2_flag = 0;
end

%Appropriately return only acceptable ellipses. i.e. ellipses that have a
%valid equation and are not too sqewed.
if(e1_flag==0 & e2_flag==0)
    good_ellps = [];
elseif(e1_flag==1 & e2_flag==0)
    if(ell1_ratio>10)
        good_ellps = [];
    else
        good_ellps = 1;
    end
elseif(e1_flag==0 & e2_flag==1)    
    if(ell2_ratio>10)
        good_ellps = [];
    else
        good_ellps = 2;
    end
elseif(e1_flag==1 & e2_flag==1)
    if(ell1_ratio<10 & ell2_ratio<10)
        good_ellps = [1 ; 2];
    else
        [tmp good_ellps] = min([ell1_ratio ; ell2_ratio]);
    end
else
    display 'Error in ellipse chosing function.'
end

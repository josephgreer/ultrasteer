%$Id: ComputeBezierTerms.m,v 1.3 2007/07/19 23:17:14 anton Exp $
function B = ComputeBezierTerms(pts, polyorder)
% function B = ComputeBezierTerms(pts, polyorder)
% ComputeBezierTerms calculates the bezier multinomial factors of pts for a
% given polynomial degree. Bezier terms are represented in tensor notation.
%
% B - Bezier matrix of size N x coeff where N is the number of points and
%                coeff is the number of coefficients


u = pts(:,1);
v = pts(:,2);

u1 = 1 -u;
v1 = 1-v;
nTerms = polyorder + 1;
Bu = [];
for i = 1:polyorder + 1
    exp = polyorder - (i-1);
    
     temp = u.^(i-1) .* u1.^(exp);
     temp = nchoosek(polyorder, i-1) * temp;
     Bu = cat(2, Bu, repmat(temp, [1, nTerms]));
 
    temp = v.^(i-1) .* v1.^(exp);
    Bv(:,i) = nchoosek(polyorder, i-1) * temp;
    clear temp;
end

Bv = repmat(Bv, [1,nTerms]);
B = Bu.*Bv;


% $Log: ComputeBezierTerms.m,v $
% Revision 1.3  2007/07/19 23:17:14  anton
% ComputeBezierTerms.m: Added a couple of clear to free memory
%
% Revision 1.2  2006/01/30 22:11:47  gouthami
% This function is changed to run faster, it takes 2.3 to 2.5 sec to dewarp an image.
% Most of the time is spent in computing the bezier terms.
%
% Revision 1.1  2005/12/07 22:37:38  gouthami
% Adding the file to CVS for the first time
%

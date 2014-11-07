% return rmserror between Poly1 and Poly2

function error = RmsError(Poly1, Poly2)
error = 0;
error = (Poly1(:,[2 3])-Poly2(:,[2 3]))*transpose((Poly1(:,[2 3])-Poly2(:,[2 3])));
%error = transpose(error)*error;
error = trace(error);
error = sqrt(error)/size(Poly1,1);
end
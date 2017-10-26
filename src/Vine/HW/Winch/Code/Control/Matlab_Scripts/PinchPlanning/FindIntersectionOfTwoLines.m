function [xy] = FindIntersectionOfTwoLines(a,b,c,d)
ll = [b-a d-c]\(c-a);

xy = a+(b-a)*ll(1);
end
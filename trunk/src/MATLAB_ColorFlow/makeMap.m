function cmap = makeMap(startCol, endCol, num)
% The following script create RGB map using linear interpolation between
% the predefined values.
%
% Copyright Ultrasonix Medical corporation
% Author: Reza Zahiri Azar - July 2010

for i = 1:3
    cmap(1:num,i) = linspace(startCol(i),endCol(i), num)';
end;
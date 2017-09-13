function res = repelem(m, siz) % repeats elements
% DESCRIPTION res = repelem(m, siz)
%  Repeats each element in m siz times where siz is a vector 
%  with one element for each dimension. Much like repmat.
% INPUT 
%  m --     Matrix elements to repeat. 
%  siz --   Number of repetitions. 
% OUTPUT
%  res --   A matrix siz times the size of m where repetitions are made 
%           per element.
% TRY 
%  repelem(rand(2,3),[3 2]) 
%  repelem(rand(2,3),[3])
%  repelem(rand(2,3),[3 2 2])
 
% by Magnus Almgren 98110

sm = sizem_(m); % the size of the source
% run over dims of m or for each element of siz
for i = 1:max(ndimsm(m),length(siz)) 
 % assume 1 if not specified
 if i>length(sm) sm(i) = 1; end
 if i>length(siz) siz(i) = 1; end
 % make the vector point in dimension i
 res{i} = flatten_(repmat(1:size(m,i),[siz(i) 1]),i);
end
% the {:} expands the cellarray as separate arguments
res = m(index(res{:}, sm));
%sample from distrubtion
function s = sample(dist, n)
s = rand(n,1);
cum = cumsum(dist);
for i=1:n
    s(i) = find(s(i) <= cum,1);
end
end
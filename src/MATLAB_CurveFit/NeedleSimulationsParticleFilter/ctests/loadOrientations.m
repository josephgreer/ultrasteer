function Rs = loadOrientations(path)
Rs = {};
RR = load(path);
for i=1:size(RR,2)
    Rs = vertcat(Rs, reshape(RR(:,i), 3, 3));
end
end
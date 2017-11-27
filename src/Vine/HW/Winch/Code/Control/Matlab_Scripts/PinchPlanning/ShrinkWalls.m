function shrunkWalls = ShrinkWalls(walls)
deltas = walls(:,3:4)-walls(:,1:2);
norms = sqrt(sum(deltas.^2,2));

deltas = 1e-3*deltas./repmat(norms,1,2);

deltas = [deltas -deltas];

shrunkWalls = walls+deltas;
end
function length = RobotLength(xs)
deltas = xs(2:end,:)-xs(1:end-1,:); length = sqrt(sum(deltas.^2,2)); 
length = sum(length);
end
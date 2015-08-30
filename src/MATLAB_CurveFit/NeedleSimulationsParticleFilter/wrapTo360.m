function deg = wrapTo360(angle)
fmodf(angle, 0, 360);
end
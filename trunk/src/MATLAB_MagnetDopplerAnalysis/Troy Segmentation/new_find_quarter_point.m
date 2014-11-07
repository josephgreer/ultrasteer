function y_qrt = new_find_quarter_point(I,p)
[h,w] = size(I);

y_qrt = 0;
totalMass = 0;
RowMass = zeros(h,1);
CumRowMass = zeros(h,1);

for row = 1:h
    for col = 1:w
        grayPixel = I(row,col);
        totalMass = totalMass + grayPixel;
        RowMass(row) = RowMass(row) + grayPixel;
    end
end

CumRowMass(1) = RowMass(1);

for row = h:-1:2
    CumRowMass(row-1) = CumRowMass(row) + RowMass(row-1);
    if( (CumRowMass(row-1) > 3*totalMass/4) && ~y_qrt)
        y_qrt = row;
    end
end

end
function cost = generate_cost_matrices_4imgs(pts_2d, trs, ex, ey, xo, yo, focal)

% generate the cost matrices
cost = cell(3,1);
cost{1} = generate_2img_cost_4(pts_2d(1:length(pts_2d)),trs,ex,ey,xo,yo,focal);
cost{2} = generate_2img_cost_4({pts_2d{2};pts_2d{3};pts_2d{1};pts_2d{4}},{trs{2};trs{3};trs{1};trs{4}},[ex(2);ex(3);ex(1);ex(4)],[ey(2);ey(3);ey(1);ey(4)],[xo(2);xo(3);xo(1);xo(4)],[yo(2);yo(3);yo(1);yo(4)],[focal(2);focal(3);focal(1);focal(4)]);
cost{3} = generate_2img_cost_4({pts_2d{3};pts_2d{1};pts_2d{2};pts_2d{4}},{trs{3};trs{1};trs{2};trs{4}},[ex(3);ex(1);ex(2);ex(4)],[ey(3);ey(1);ey(2);ey(4)],[xo(3);xo(1);xo(2);xo(4)],[yo(3);yo(1);yo(2);yo(4)],[focal(3);focal(1);focal(2);focal(4)]);

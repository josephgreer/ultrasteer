function [XX YY rr] = GenerateGrid(params)
[XX YY rr] = meshgrid(linspace(0,params.gridWidth, params.nGridPoints),linspace(0,params.gridHeight, params.nGridPoints), linspace(0, 2*pi, params.nRadPoints));
end
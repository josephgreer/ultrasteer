clear; clc; close all;

W = [.41 .99 .51 .32 .15 .45 .38 .32 .36 .29 .21];
DG = sparse([6 1 2 2 3 4 4 5 5 6 1],[2 6 3 5 4 1 6 3 4 3 5],W)

negDG = DG;
negDG(3,4) = -1;

weights = nonzeros(negDG);
weights(weights == -1) = 0;


h = view(biograph(negDG,[],'ShowWeights','on'))

[dist,path,pred] = graphshortestpath(DG,1,6,'Weights',weights)


set(h.Nodes(path),'Color',[1 0.4 0.4])
edges = getedgesbynodeid(h,get(h.Nodes(path),'ID'));
set(edges,'LineColor',[1 0 0])
set(edges,'LineWidth',1.5)
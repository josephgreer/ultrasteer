function PlotFrameTravelAmount(header, mpp, fgps, indices)
travels = [];
for i=1:size(indices,1)
    index = indices(i);
    
    travel = 0;
    if(index - 1 > 0 && index + 1 < size(indices,1))
        gps1 = RPreadgpsindex(fgps, header, index-1);
        gps2 = RPreadgpsindex(fgps, header, index+1);
        travel = max(travel,FrameTravel(header, mpp, gps1,gps2));
    elseif(index - 1 > 0)
        gps1 = RPreadgpsindex(fgps, header, index-1);
        gps2 = RPreadgpsindex(fgps, header, index);
        travel = max(travel,FrameTravel(header, mpp, gps1,gps2));
    else
        gps1 = RPreadgpsindex(fgps, header, index);
        gps2 = RPreadgpsindex(fgps, header, index+1);
        travel = max(travel,FrameTravel(header, mpp, gps1,gps2));
    end
    travels = [travels; travel];
end
figure(14);
plot([1:size(indices,1)], travels);
end
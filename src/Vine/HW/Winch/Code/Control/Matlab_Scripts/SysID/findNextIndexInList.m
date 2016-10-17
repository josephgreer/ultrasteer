function nextGreater = findNextIndexInList(idxs, idx)
idxs = sort(idxs);
nextGreater = -1;
for ii=1:length(idxs)
    if(idxs(ii) >= idx)
        nextGreater = ii;
        break;
    end
end
end
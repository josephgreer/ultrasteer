function s=sm_pscat(s1,s2)

[a,tmp]=size(s1);
[b,tmp]=size(s2);
s=s1;
c=a;
for i=1:b
%    tmp=s1-repmat(s2(i,:),a,1);
%    if isempty(find(tmp(:,1)==0)) | isempty(find(tmp(:,2)==0))
    ind1=find(s(:,1)==s2(i,1));
    if ~isempty(ind1)
        ind2=find(s(ind1,2)==s2(i,2));
        if ~isempty(ind2);
            continue;
        end
    else
        c=c+1;
        s(c,:)=s2(i,:);
    end
end


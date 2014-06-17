function [r,matched,unmatched,m_truth]=sm_matchingrate2(M)
%SM_MATCHINGRATE calulates the percentage of successful matching.
%
%R=SM_MATCHINGRATE(M), where
%M is the matrix of matching,
%returns r as the rate of successful matching,
%matched as the list of good matches,
%unmatched as the list of bad matches,
%m_truth as the list of tags of good and bad matches.

matched=[];
unmatched=[];
[n_seeds,n_tag]=size(M);
n_ims=n_tag-1;
m_truth=zeros(n_seeds,1);

r=0;
r2=0;
tag=0;
for i=1:n_seeds
    if M(i,1)==M(i,2) & M(i,2)==M(i,3)
        r=r+1;
        matched(r,:)=M(i,:);
        m_truth(M(i,1))=1;
    else
        r2=r2+1;
        unmatched(r2,:)=M(i,:);
    end
end

r=r/n_seeds;

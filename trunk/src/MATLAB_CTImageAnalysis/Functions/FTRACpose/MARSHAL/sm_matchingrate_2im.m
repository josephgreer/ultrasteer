function [r,matched,unmatched,m_truth]=sm_matchingrate_2im(M)
%SM_MATCHINGRATE_2im calulates the percentage of successful matching from 2 images.
%
%R=SM_MATCHINGRATE_2im(M), where
%M is the matrix of matching,
%returns the rate of matching, R.

matched=[];
unmatched=[];
[n_seeds,tmp]=size(M);
m_truth=zeros(n_seeds,1);

r=0;
r2=0;
for i=1:n_seeds
    if M(i,1)==M(i,2)
        r=r+1;
        matched(r,:)=M(i,:);
        m_truth(M(i,1))=1;
    else
        r2=r2+1;
        unmatched(r2,:)=M(i,:);
    end
end

r=r/n_seeds;

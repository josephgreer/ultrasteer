function s=sm_cat(s1,s2)

a=length(s1);
b=length(s2);
s=s1;
for i=1:b
    tmp=abs(s1-s2(i));
    if isempty(find(tmp<1.0e-5))
        a=a+1;
        s(a)=s2(i);
    end
end

